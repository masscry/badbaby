#include <binstore.hpp>
#include <common.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <WinSock2.h>
#endif

#include <cassert>
#include <cstdint>
#include <cstring>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/file.h>
#include <unistd.h>
#endif

#ifdef _WIN32

#include <io.h>

#define O_NOFOLLOW (0)
#define O_CLOEXEC (0)

int strerror_r(int errnum, char *buf, size_t buflen)
{
  return strerror_s(buf, buflen, errnum);
}

#define LOCK_UN (0x0)
#define LOCK_EX (0x1)
#define LOCK_SH (0x2)
#define LOCK_MASK (0x3)
#define LOCK_NB (0x4)

#define S_IRUSR (_S_IREAD)
#define S_IWUSR (_S_IWRITE)
#define S_IRGRP (0)
#define S_IROTH (0)

int flock(int fd, int operation)
{
  OVERLAPPED overlapped = {0};
  HANDLE osHandle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
  DWORD lockFlag = 0;

  switch (operation & LOCK_MASK)
  {
    case LOCK_UN:
      if (UnlockFileEx(osHandle, 0, 0xFFFFFFFF, 0xFFFFFFFF, &overlapped) == 0)
      { // TODO: make proper error handling
        errno = EINVAL;
        return -1;
      }
      return 0;
    case LOCK_EX:
      lockFlag = LOCKFILE_EXCLUSIVE_LOCK;
      /* FALLTHROUGH */
    case LOCK_SH:
      if ((operation & LOCK_NB) != 0)
      {
        lockFlag |= LOCKFILE_FAIL_IMMEDIATELY;
      }
      if (LockFileEx(osHandle, lockFlag, 0, 0xFFFFFFFF, 0xFFFFFFFF, &overlapped) == 0)
      { // TODO: make proper error handling
        errno = EINVAL;
        return -1;
      }
      return 0;
  }

  return 0;
}

int ftruncate(int fd, off_t length)
{
  auto result = _chsize_s(fd, length);
  if (result != 0)
  {
    errno = result;
    return -1;
  }
  return 0;
}

#define open _open
#define close _close
#define fdopen _fdopen

#else

#define O_BINARY (0)

#endif

namespace
{
  enum
  {
    BINSTORE_MAGIC = 0xABADBABEL,
    BINSTORE_VERSION = 0x1L
  };

  struct serialHeader_t
  {
    uint32_t magic;
    uint32_t version;
    uint32_t dataSize;
    uint32_t tag;
  };

  static_assert(sizeof(serialHeader_t) == 16, "serialHeader_t must be 16 bytes long");
} // namespace

namespace bb
{

  namespace ext
  {

    binstore_t::binstore_t()
        : om(binstore_t::openMode_t::undef),
          stream(nullptr),
          headPos(),
          tag(0),
          dirty(false),
          dataSize(0)
    {
      ;
    }

    binstore_t::binstore_t(FILE *stream, binstore_t::openMode_t om)
        : om(om),
          stream(stream),
          tag(0),
          dirty(false),
          dataSize(0)
    {
      // programmer's mistakes
      BB_ASSERT(this->om != openMode_t::undef);
      BB_ASSERT(this->stream != nullptr);
      if (fgetpos(this->stream, &this->headPos) != 0)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
      }

      switch (this->om)
      {
        case binstore_t::openMode_t::create:
          this->PutHeader();
          break;
        case binstore_t::openMode_t::read:
          this->GetHeader();
          break;
        default: // programmer's mistake
          BB_PANIC();
      }
    }

    binstore_t::~binstore_t()
    {
      this->Reset();
    }

    binstore_t::binstore_t(binstore_t &&mv) noexcept
        : om(mv.om),
          stream(mv.stream),
          headPos(mv.headPos),
          tag(mv.tag),
          dirty(mv.dirty),
          dataSize(mv.dataSize)
    {
      mv.om = binstore_t::openMode_t::undef;
      mv.stream = nullptr;
      mv.tag = 0;
      mv.dirty = false;
      mv.dataSize = 0;
    }

    binstore_t &binstore_t::operator=(binstore_t &&mv) noexcept
    {
      if (this != &mv)
      {
        this->Reset();
        this->om = mv.om;
        this->stream = mv.stream;
        this->headPos = mv.headPos;
        this->tag = mv.tag;
        this->dirty = mv.dirty;
        this->dataSize = mv.dataSize;

        mv.om = binstore_t::openMode_t::undef;
        mv.stream = nullptr;
        mv.tag = 0;
        mv.dirty = false;
        mv.dataSize = 0;
      }
      return *this;
    }

    binstore_t binstore_t::Read(const char *filename)
    {
      int inputHandle = open(filename, O_RDONLY | O_BINARY | O_CLOEXEC | O_NOFOLLOW);
      if (inputHandle == -1)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        return binstore_t();
      }

      if (flock(inputHandle, LOCK_SH | LOCK_NB) != 0)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        close(inputHandle);
        return binstore_t();
      }

      FILE *input = fdopen(inputHandle, "rb");
      if (input == nullptr)
      {
        flock(inputHandle, LOCK_UN);
        close(inputHandle);
        return binstore_t();
      }

      return binstore_t(
        input,
        binstore_t::openMode_t::read);
    }

    binstore_t binstore_t::Create(const char *filename)
    {
      int outputHandle = open(filename,
        O_WRONLY | O_BINARY | O_CREAT | O_CLOEXEC | O_NOFOLLOW,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

      if (outputHandle == -1)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        return binstore_t();
      }

      if (flock(outputHandle, LOCK_EX | LOCK_NB) != 0)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        close(outputHandle);
        return binstore_t();
      }

      if (ftruncate(outputHandle, 0) != 0)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        flock(outputHandle, LOCK_UN);
        close(outputHandle);
        return binstore_t();
      }

      FILE *output = fdopen(outputHandle, "wb");
      if (output == nullptr)
      {
        flock(outputHandle, LOCK_UN);
        close(outputHandle);
        return binstore_t();
      }

      return binstore_t(
        output,
        binstore_t::openMode_t::create);
    }

    int binstore_t::GetHeader()
    {
      serialHeader_t head;

      if (fread(&head, sizeof(head), 1, this->stream) != 1)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        return -1;
      }

      if ((ntohl(head.magic) != BINSTORE_MAGIC) || (ntohl(head.version) != BINSTORE_VERSION))
      {
        bb::Error("%s:%d: Invalid binstore version", __FILE__, __LINE__);
        this->Reset();
        return -1;
      }

      this->dataSize = ntohl(head.dataSize) - sizeof(serialHeader_t);
      this->tag = ntohl(head.tag);
      return 0;
    }

    int binstore_t::Setup()
    {
      fpos_t filePos;
      if (fgetpos(this->stream, &filePos) != 0)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        return -1;
      }
      BB_DEFER(fsetpos(this->stream, &filePos));

      if (fsetpos(this->stream, &this->headPos) != 0)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        return -1;
      }

      return this->GetHeader();
    }

    int binstore_t::PutHeader()
    {
      serialHeader_t head;
      head.magic = htonl(BINSTORE_MAGIC);
      head.version = htonl(BINSTORE_VERSION);
      head.dataSize = htonl((this->dataSize + sizeof(serialHeader_t)) & 0xFFFFFFFF);
      head.tag = htonl(this->tag);

      if (fwrite(&head, sizeof(head), 1, this->stream) != 1)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        return -1;
      }
      return 0;
    }

    int binstore_t::Flush()
    {
      if (this->dirty)
      {
        BB_ASSERT(this->stream != nullptr);

        fpos_t filePos;
        if (fgetpos(this->stream, &filePos) != 0)
        {
          char errorbuf[1024];
          bb::Error("%s:%d: error: %s (%d)",
            __FILE__,
            __LINE__,
            strerror_r(errno, errorbuf, sizeof(errorbuf)),
            errno);
          return -1;
        }
        BB_DEFER(fsetpos(this->stream, &filePos));

        if (fsetpos(this->stream, &this->headPos) != 0)
        {
          char errorbuf[1024];
          bb::Error("%s:%d: error: %s (%d)",
            __FILE__,
            __LINE__,
            strerror_r(errno, errorbuf, sizeof(errorbuf)),
            errno);
          return -1;
        }

        if (this->dataSize > (0xFFFFFFFF - sizeof(serialHeader_t)))
        {
          bb::Error("%s:%d: error: File is too large", __FILE__, __LINE__);
          return -1;
        }

        this->dirty = false;
        auto putHeaderResult = this->PutHeader();
        fflush(this->stream);
        return putHeaderResult;
      }

      return 0;
    }

    bool binstore_t::Reset()
    {
      if (this->stream != nullptr)
      {
        this->Flush();
        fclose(this->stream);

        this->om = binstore_t::openMode_t::undef;
        this->stream = nullptr;
        this->dirty = false;
        this->dataSize = 0;
      }
      return false;
    }

    bool binstore_t::IsGood() const
    {
      return (this->stream != nullptr) && (this->om != binstore_t::openMode_t::undef);
    }

    binstore_t::openMode_t binstore_t::OpenMode() const
    {
      return this->om;
    }

    int binstore_t::Write(const void *buffer, size_t bufferSize)
    {
      if (this->om != binstore_t::openMode_t::create)
      {
        bb::Error("%s:%d: error: Can't write to file", __FILE__, __LINE__);
        return -1;
      }

      if (fwrite(buffer, bufferSize, 1, this->stream) != 1)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        return -1;
      }

      this->dataSize += bufferSize;
      this->dirty = true;
      return 0;
    }

    int binstore_t::Read(void *buffer, size_t bufferSize)
    {
      if (this->om != binstore_t::openMode_t::read)
      {
        bb::Error("%s:%d: error: Can't read from file", __FILE__, __LINE__);
        return -1;
      }

      if (this->dataSize < bufferSize)
      {
        bb::Error("%s:%d: error: No Data left", __FILE__, __LINE__);
        return -1;
      }

      if (fread(buffer, bufferSize, 1, this->stream) != 1)
      {
        char errorbuf[1024];
        bb::Error("%s:%d: error: %s (%d)",
          __FILE__,
          __LINE__,
          strerror_r(errno, errorbuf, sizeof(errorbuf)),
          errno);
        return -1;
      }

      this->dataSize -= bufferSize;
      return 0;
    }

    int binstore_t::Write(const char *str)
    {
      auto strLen = strlen(str);
      if (strLen > 0xFFFF)
      {
        return -1;
      }

      auto result = this->Write(static_cast<uint16_t>(strLen & 0xFFFF));
      if (result != 0)
      {
        return result;
      }
      return this->Write(str, strLen);
    }

    int binstore_t::Write(const std::string &str)
    {
      if (str.size() > 0xFFFF)
      {
        return -1;
      }

      auto result = this->Write(static_cast<uint16_t>(str.size() & 0xFFFF));
      if (result != 0)
      {
        return result;
      }
      return this->Write(str.c_str(), str.size());
    }

    int binstore_t::Read(std::string &str)
    {
      uint16_t strSize;

      auto result = this->Read(strSize);
      if (result != 0)
      {
        return result;
      }

      char *tempResult = new (std::nothrow) char[static_cast<size_t>(strSize) + 1];
      if (tempResult == nullptr)
      {
        return -1;
      }
      BB_DEFER(delete[] tempResult);

      result = this->Read(tempResult, strSize);
      if (result != 0)
      {
        return result;
      }
      tempResult[strSize] = 0;

      str.assign(tempResult);
      return 0;
    }

    uint32_t binstore_t::Tag() const
    {
      return this->tag;
    }

    int binstore_t::SetTag(uint32_t tag)
    {
      if (!this->IsGood())
      {
        return -1;
      }

      this->tag = tag;
      this->dirty = true;
      return 0;
    }

  } // namespace ext

} // namespace bb
