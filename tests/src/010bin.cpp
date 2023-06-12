#include <binstore.hpp>
#include <common.hpp>

#include <cassert>

int main(int, char*[])
{
  using namespace bb::ext;

  binstore_t store = binstore_t::Create("test.bbf");
  BB_ASSERT(store.IsGood());
  BB_ASSERT(store.Write(100) == 0);
  BB_ASSERT(store.Write("sample") == 0);
  BB_ASSERT(store.SetTag(0x12345678) == 0);

  store.Reset(); // only one writer can use given bbf

  store = binstore_t::Read("test.bbf");
  BB_ASSERT(store.IsGood());
  BB_ASSERT(store.Tag() == 0x12345678);

  int tmp = 0;
  BB_ASSERT(store.Read(tmp) == 0);
  BB_ASSERT(tmp == 100);

  std::string tempStr;
  BB_ASSERT(store.Read(tempStr) == 0);
  BB_ASSERT(tempStr.compare("sample") == 0);

  return 0;
}