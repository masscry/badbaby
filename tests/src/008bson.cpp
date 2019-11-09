#include <common.hpp>
#include <bson.hpp>

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  bb::bson::document_t test;
  test.Store("test.bson");

  return 0;
}