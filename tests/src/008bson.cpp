#include <common.hpp>
#include <bson.hpp>

void PrintDocument(const bb::bson::document_t& doc, int indent = 0)
{
  for (auto it = doc.Begin(), e = doc.End(); it != e; ++it)
  {
    switch(it.Type())
    {
    case bb::bson::type_t::number:
      bb::Debug("%*s\"%s\": %f", indent, "", it.Key().c_str(), it.Value<double>());
      break;
    case bb::bson::type_t::string:
      bb::Debug("%*s\"%s\": \"%s\"", indent, "", it.Key().c_str(), it.Value<std::string>().c_str());
      break;
    case bb::bson::type_t::document:
      bb::Debug("%*s\"%s\":", indent, "", it.Key().c_str());
      PrintDocument(it.Value<bb::bson::document_t>(), indent+2);
    default:
      break;
    }
  }
}

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  bb::bson::document_t example000;
  example000.Load("example000.bson");

  PrintDocument(example000);

  bb::bson::document_t test;
  test.Store("test.bson");

  return 0;
}