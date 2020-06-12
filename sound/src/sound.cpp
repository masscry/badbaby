#include <sound.hpp>
#include <common.hpp>

#include <stdexcept>
#include <bass.h>

namespace bb
{

  sound_t::sound_t()
  {
    if (HIWORD(BASS_GetVersion()) != BASSVERSION)
    {
      throw std::runtime_error("Invalid BASS Version!");
    }

    if (BASS_Init(-1, 44100, BASS_DEVICE_DMIX, nullptr, nullptr) == FALSE)
    {
      bb::Error("BASS_Init failed: error %d", BASS_ErrorGetCode());
      throw std::runtime_error("Can't initialize BASS library!");
    }
  }

  sound_t::~sound_t()
  {
    BASS_Free();
  }

  sound_t::stream_t::stream_t() noexcept
  : handle(0)
  {
    ;
  }

  sound_t::stream_t::~stream_t()
  {
    if (this->handle != 0)
    {
      if (BASS_ChannelIsActive(this->handle))
      {
        BASS_ChannelStop(this->handle);
      }
      BASS_StreamFree(this->handle);
    }
  }

  sound_t::stream_t::stream_t(sound_t::stream_t&& stream) noexcept
  : handle(stream.handle)
  {
    stream.handle = 0;
  }

  sound_t::stream_t::stream_t(uint32_t handle) noexcept
  : handle(handle)
  {
    ;
  }

  sound_t::stream_t& sound_t::stream_t::operator=(sound_t::stream_t&& stream) noexcept
  {
    if (this != &stream)
    {
      if (this->handle != 0)
      {
        if (BASS_ChannelIsActive(this->handle))
        {
          BASS_ChannelStop(this->handle);
        }
        BASS_StreamFree(this->handle);
      }

      this->handle = stream.handle;
      stream.handle = 0;
    }
    return *this;
  }

  sound_t::stream_t sound_t::CreateStream(const char* filename, bool loop)
  {
    DWORD flags = 0;

    if (loop)
    {
      flags |= BASS_SAMPLE_LOOP;
    }

    auto stream = BASS_StreamCreateFile(
      FALSE,
      filename,
      0, 0, flags
    );

    if (stream == 0)
    {
      bb::Error("BASS_StreamCreateFile failed: error %d", BASS_ErrorGetCode());
    }
    return sound_t::stream_t(stream);
  }

  int sound_t::Play(const stream_t& stream)
  {
    if (stream.IsGood())
    {
      auto result = BASS_ChannelPlay(
        stream.handle,
        FALSE
      ); 

      if (result == FALSE)
      {
        bb::Error("BASS_ChannelPlay failed: error %d", BASS_ErrorGetCode());
        return -1;
      }

      return 0;
    }
    return -1;
  }

  sound_t& sound_t::Instance()
  {
    static sound_t sound;
    return sound;
  }


  sound_t::sample_t::sample_t(uint32_t handle) noexcept
  : handle(handle)
  {
    ;
  }

  sound_t::sample_t::sample_t()
  : handle(0)
  {
    ;
  }

  sound_t::sample_t::~sample_t()
  {
    if (this->handle != 0)
    {
      if (BASS_SampleStop(this->handle))
      {
        BASS_SampleFree(this->handle);
      }
    }
  }

  sound_t::sample_t::sample_t(sound_t::sample_t&& src) noexcept
  : handle(src.handle)
  {
    src.handle = 0;
  }

  sound_t::sample_t& sound_t::sample_t::operator=(sound_t::sample_t&& src) noexcept
  {
    if (this != &src)
    {
      if (this->handle != 0)
      {
        if (BASS_SampleStop(this->handle))
        {
          BASS_SampleFree(this->handle);
        }
      }
      this->handle = src.handle;
      src.handle = 0;
    }
    return *this;
  }

  int sound_t::Play(const sound_t::sample_t& sample)
  {
    if (!sample.IsGood())
    {
      return -1;
    }

    auto channel = BASS_SampleGetChannel(sample.handle, FALSE);
    if (channel == 0)
    {
      bb::Error("BASS_SampleGetChannel failed: error: %d", BASS_ErrorGetCode());
      return -1;
    }

    auto playResult = BASS_ChannelPlay(channel, FALSE);
    if (playResult == FALSE)
    {
      BASS_ChannelStop(channel);
      bb::Error("BASS_ChannelPlay failed: error: %d", BASS_ErrorGetCode());
      return -1;
    }
    return 0;
  }

  sound_t::sample_t sound_t::CreateSample(const char* filename, uint32_t maxSimPlays, bool loop)
  {
    DWORD flags = BASS_SAMPLE_OVER_VOL;

    if (loop)
    {
      flags |= BASS_SAMPLE_LOOP;
    }

    auto sample = BASS_SampleLoad(
      FALSE, filename,
      0, 0, maxSimPlays, flags
    );

    if (sample == 0)
    {
      bb::Error("BASS_SampleLoad failed: errro: %d", BASS_ErrorGetCode());
      return bb::sound_t::sample_t();
    }

    return bb::sound_t::sample_t(sample);
  }

  int sound_t::Stop(const stream_t& stream)
  {
    auto result = BASS_ChannelStop(stream.handle);
    if (result == FALSE)
    {
      bb::Error("BASS_ChannelStop failed: errro: %d", BASS_ErrorGetCode());
      return -1;
    }
    return 0;
  }

  int sound_t::Stop(const sample_t& sample)
  {
    auto result = BASS_SampleStop(sample.handle);
    if (result == FALSE)
    {
      bb::Error("BASS_SampleStop failed: errro: %d", BASS_ErrorGetCode());
      return -1;
    }
    return 0;
  }

} // namespace bb
