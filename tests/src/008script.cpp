#include <common.hpp>
#include <script.hpp>

#include <cstdio>

class printVM_t final: public bb::vm_t
{
  int OnCommand(int cmd, const bb::listOfRefs_t& refs)
  {
    printf("\t%c", cmd);
    for (auto& item: refs)
    {
      printf(":%f", item.Number());
    }
    printf(";\n");
    return 0;
  }
};

int main(int argc, char* argv[])
{
  printVM_t printVM;
  for (int i = 1; i < argc; ++i)
  {
    printf("PROCESS \"%s\"\n", argv[i]);
    auto inputData = bb::ReadWholeFile(argv[i], "rt", nullptr);
    if (inputData == nullptr)
    {
      continue;
    }
    BB_DEFER(free(inputData));

    printf("RESULT: %d\n", bb::ExecuteScript(printVM, inputData));
  }
}
