#include <common.hpp>
#include <config.hpp>

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  bb::config_t sample;

  sample["test number"] = bb::ref_t::Number(100.4);
  sample["test string"] = bb::ref_t::String("Hello, world!");

  sample.Save("sample.config");

  bb::config_t sample2;

  sample2.Load("sample.config");

  if (sample2["test number"] != sample["test number"])
  {
    throw std::runtime_error("Number comparison failed");
  }

  if (sample2["test string"] != sample["test string"])
  {
    throw std::runtime_error("String comparison failed");
  }

  if (sample2["test string"] == sample["test number"])
  {
    throw std::runtime_error("String/Number comparison failed");
  }

  return 0;
}