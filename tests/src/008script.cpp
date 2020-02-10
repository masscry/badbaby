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

char* ReadWholeFile(const char* filename)
{
  FILE* input = fopen(filename, "rb");
  if (input == nullptr)
  {
    return nullptr;
  }

  fseek(input, 0, SEEK_END);
  auto inputFileSize = static_cast<size_t>(ftell(input));
  fseek(input, 0, SEEK_SET);

  char* result = reinterpret_cast<char*>(malloc(inputFileSize + 1));
  if (result == nullptr)
  {
    fclose(input);
    return nullptr;
  }

  fread(result, 1, inputFileSize, input);
  fclose(input);

  result[inputFileSize] = 0;
  return result;
}

int main(int argc, char* argv[])
{
  printVM_t printVM;
  for (int i = 1; i < argc; ++i)
  {
    printf("PROCESS \"%s\"\n", argv[i]);
    auto inputData = ReadWholeFile(argv[i]);
    if (inputData == nullptr)
    {
      continue;
    }
    BB_DEFER(free(inputData));

    printf("RESULT: %d\n", bb::ExecuteScript(printVM, inputData));
  }
}
