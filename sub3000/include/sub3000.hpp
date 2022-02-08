/**
 * @file sub3000.hpp
 * @author timur
 * 
 * Globals and entry point
 * 
 */

#pragma once
#ifndef __SUB3000_HEADER__
#define __SUB3000_HEADER__

#include <GLFW/glfw3.h>

#include <memory>

#include <msg.hpp>
#include <monfs.hpp>

#include "actionTable.hpp"
#include "scene.hpp"

namespace sub3000
{

  class deltaTime_t final
  {
    double start;

    deltaTime_t(const deltaTime_t&) = delete;
    deltaTime_t(deltaTime_t&&) = delete;

    deltaTime_t& operator=(const deltaTime_t&) = delete;
    deltaTime_t& operator=(deltaTime_t&&) = delete;

  public:

    double Mark()
    {
      double now = glfwGetTime();
      double result = now - this->start;
      this->start = now;
      return result;
    }

    deltaTime_t()
    :start(glfwGetTime())
    {
      ;
    }

    ~deltaTime_t()
    {
      ;
    }

  };

  void PostToMain(bb::msg_t&& msg);

  class changeScene_t: public bb::msg::basic_t
  {
    sceneID_t sceneID;
  public:

    sceneID_t SceneID() const
    {
      return this->sceneID;
    }

    changeScene_t(sceneID_t sceneID)
    : sceneID(sceneID)
    {
      ;
    }

    changeScene_t(const changeScene_t&) = default;
    changeScene_t& operator=(const changeScene_t&) = default;
    changeScene_t(changeScene_t&&) = default;
    changeScene_t& operator=(changeScene_t&&) = default;
    ~changeScene_t() override = default;

  };

  namespace fs
  {

    class watch_t final: public bb::msg::basic_t
    {
      std::unique_ptr<bb::fs::processor_t>  processor;
      std::string filename;
    public:

      std::unique_ptr<bb::fs::processor_t>& Processor()
      {
        return this->processor;
      }

      const std::unique_ptr<bb::fs::processor_t>& Processor() const
      {
        return this->processor;
      }

      const std::string& Filename() const
      {
        return this->filename;
      }

      watch_t(bb::actorPID_t src, const std::string& filename, std::unique_ptr<bb::fs::processor_t>&& processor)
      : bb::msg::basic_t(src),
        processor(std::move(processor)),
        filename(filename)
      {
        ;
      }

      watch_t(const watch_t&) = delete;
      watch_t& operator=(const watch_t&) = delete;
      watch_t(watch_t&&) = default;
      watch_t& operator=(watch_t&&) = default;
      ~watch_t() override = default;
    };

    class status_t final: public bb::msg::basic_t
    {
      int status;
    public:

      int Status() const
      {
        return this->status;
      }

      status_t(int status)
      : status(status)
      {
        ;
      }

      status_t(const status_t&) = default;
      status_t& operator=(const status_t&) = default;

      status_t(status_t&&) = default;
      status_t& operator=(status_t&&) = default;

      ~status_t() override = default;
    };

    class rmWatch_t final: public bb::msg::basic_t
    {
      int watch;
    public:

      int Watch() const
      {
        return this->watch;
      }

      rmWatch_t(int watch)
      : watch(watch)
      {
        ;
      }

      rmWatch_t(const rmWatch_t&) = default;
      rmWatch_t& operator=(const rmWatch_t&) = default;

      rmWatch_t(rmWatch_t&&) = default;
      rmWatch_t& operator=(rmWatch_t&&) = default;

      ~rmWatch_t() override = default;
    };

  }


  class exit_t final: public bb::msg::basic_t
  {
  public:
    exit_t() { ; }
    ~exit_t() override = default;
  };

  class action_t: public bb::msg::basic_t
  {
    gameAction_t gameAction;
  public:

    gameAction_t GameAction() const
    {
      return this->gameAction;
    }

    action_t(bb::actorPID_t src, gameAction_t gameAction)
    : bb::msg::basic_t(src),
      gameAction(gameAction)
    {
      ;
    }

    action_t(const action_t&) = default;
    action_t& operator=(const action_t&) = default;
    action_t(action_t&&) = default;
    action_t& operator=(action_t&&) = default;
    ~action_t() override = default;
  };

  bool RequestGenerateMap(bb::actorPID_t sendResultToID);

} // namespace sub3000

#endif /* __SUB3000_HEADER__ */
