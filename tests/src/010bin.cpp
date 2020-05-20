#include <binstore.hpp>

#include <cassert>

int main(int, char*[])
{
  using namespace bb::ext;

  binstore_t store = binstore_t::Create("test.bbf");
  assert(store.IsGood());
  assert(store.Write(100) == 0);
  assert(store.Write("sample") == 0);
  assert(store.SetTag(0x12345678) == 0);

  store.Reset(); // only one writer can use given bbf

  store = binstore_t::Read("test.bbf");
  assert(store.IsGood());
  assert(store.Tag() == 0x12345678);

  int tmp;
  assert(store.Read(tmp) == 0);
  assert(tmp == 100);

  std::string tempStr;
  assert(store.Read(tempStr) == 0);
  assert(tempStr.compare("sample") == 0);

  return 0;
}