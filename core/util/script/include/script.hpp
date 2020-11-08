/**
 * @file script.hpp
 *
 * Simple scripting.
 */

#pragma once
#ifndef __BB_UTILS_SCRIPT_HEADER__
#define __BB_UTILS_SCRIPT_HEADER__

#include <value.hpp>

#include <list>

namespace bb
{

  using listOfRefs_t = std::list<ref_t>;

  double Argument(const listOfRefs_t& refs, uint32_t id);

  std::string StringArg(const listOfRefs_t& refs, uint32_t id);

  class vm_t
  {
    listOfRefs_t listOfRefs;

    virtual int OnCommand(int cmd, const listOfRefs_t& refs) = 0;

    vm_t(const vm_t& vm) = delete;
    vm_t& operator = (const vm_t& vm) = delete;

  public:

    int Push(ref_t&& ref);
    int Command(int cmd);

    vm_t();
    virtual ~vm_t();

    vm_t(vm_t&& vm);
    vm_t& operator = (vm_t&& vm);
  };

  int ExecuteScript(vm_t& vm, const char* script);

  int ExecuteScriptFile(vm_t& vm, const char* filename);

  /**
   * Read whole content of file to memory.
   *
   * @param[in] filename input filename to read
   * @param[out] pSize if != nullptr, fill with size of allocated memory
   *
   * @return content of read file or nullptr on errors
   */
  char* ReadWholeFile(const char* filename, const char* openMode, size_t* pSize);

  char* ReadWholeFile(FILE* input, size_t* pSize);

} // namespace bb

#endif /* __BB_UTILS_SCRIPT_HEADER__  */
