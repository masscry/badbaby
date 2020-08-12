/**
 * @file msg.hpp
 *
 * Simple message struct
 *
 */
#pragma once
#ifndef __BB_CORE_MSG_HEADER__
#define __BB_CORE_MSG_HEADER__

#include <cstring>
#include <cstdint>

#include <string>
#include <memory>
#include <utility>
#include <type_traits>

namespace bb
{

  using actorPID_t = int64_t;

  const actorPID_t INVALID_ACTOR = -1;

  namespace msg
  {
    enum class result_t
    {
      skipped = 0,
      complete = 1,
      error,
      poisoned
    };

    class basic_t
    {
      actorPID_t src;

    public:

      actorPID_t Source() const;

      basic_t();
      basic_t(actorPID_t src);

      basic_t(const basic_t&) = default;
      basic_t& operator=(const basic_t&) = default;

      basic_t(basic_t&&) noexcept = default;
      basic_t& operator=(basic_t&&) noexcept = default;

      virtual ~basic_t() = 0;
    };

    template<typename castType_t>
    const castType_t* As(const basic_t& msg)
    {
      static_assert(std::is_base_of<bb::msg::basic_t, castType_t>::value,
        "Can be used only with bb::msg::basic_t subclasses"
      );

      if (typeid(msg) != typeid(castType_t))
      {
        return nullptr;
      }

      return static_cast<const castType_t*>(&msg);
    }

    template<typename data_t>
    class dataMsg_t: public basic_t
    {
      data_t data;
    public:

      const data_t& Data() const
      {
        return this->data;
      }

      data_t& Data()
      {
        return this->data;
      }

      dataMsg_t(data_t&& data, actorPID_t src)
      : basic_t(src),
        data(std::move(data))
      {
        ;
      }

      dataMsg_t(const data_t& data, actorPID_t src)
      : basic_t(src),
        data(data)
      {
        ;
      }

      dataMsg_t(const dataMsg_t<data_t>&) = default;
      dataMsg_t& operator=(const dataMsg_t<data_t>&) = default;

      dataMsg_t(dataMsg_t<data_t>&&) noexcept = default;
      dataMsg_t& operator=(dataMsg_t<data_t>&&) noexcept= default;

      ~dataMsg_t() override = default;
    };

    class poison_t final: public basic_t
    {
    public:
      poison_t();
      poison_t(const poison_t&) = default;
      poison_t& operator=(const poison_t&) = default;

      poison_t(poison_t&&) noexcept = default;
      poison_t& operator=(poison_t&&) noexcept = default;

      ~poison_t() override = default;
    };

    class basicExecTask_t: public basic_t
    {
    public:

      virtual msg::result_t Execute() const = 0;

      basicExecTask_t(const basicExecTask_t&) = default;
      basicExecTask_t& operator= (const basicExecTask_t&) = default;

      basicExecTask_t(basicExecTask_t&&) noexcept = default;
      basicExecTask_t& operator=(basicExecTask_t&&) noexcept = default;

      ~basicExecTask_t() override = default;
    };

    template<typename func_t>
    class execTask_t final: public basicExecTask_t
    {
      func_t func;
    public:

      msg::result_t Execute() const override;

      execTask_t(func_t func);

      execTask_t(const execTask_t&) = default;
      execTask_t& operator= (const execTask_t&) = default;

      execTask_t(execTask_t&&) noexcept = default;
      execTask_t& operator=(execTask_t&&) noexcept = default;

      ~execTask_t() override = default;
    };

    template<typename func_t>
    inline msg::result_t execTask_t<func_t>::Execute() const
    {
      return this->func();
    }

    template<typename func_t>
    inline execTask_t<func_t>::execTask_t(func_t func)
    : func(func)
    {
      ;
    }

    class updateTitle_t final: public basic_t
    {
      std::string title;
    public:

      const std::string& Title() const
      {
        return this->title;
      }

      updateTitle_t(std::string&& title) noexcept
      : title(std::move(title))
      {
        ;
      }
      ~updateTitle_t() override = default;

    };

    class setName_t final: public basic_t
    {
      std::string name;
    public:

      const std::string& Name() const;

      setName_t(const std::string& name);
      setName_t(std::string&& name);

      setName_t(const setName_t&) = default;
      setName_t& operator=(const setName_t&) = default;

      setName_t(setName_t&&) noexcept = default;
      setName_t& operator=(setName_t&&) noexcept = default;

      ~setName_t() override = default;
    };

    class keyEvent_t final: public basic_t
    {
      int key;
      int press;

    public:

      int Key() const;
      int Press() const;

      keyEvent_t(int key, int press);

      keyEvent_t(const keyEvent_t&) = default;
      keyEvent_t& operator=(const keyEvent_t&) = default;

      keyEvent_t(keyEvent_t&&) noexcept = default;
      keyEvent_t& operator=(keyEvent_t&&) noexcept = default;

      ~keyEvent_t() override = default;
    };

    inline actorPID_t basic_t::Source() const
    {
      return this->src;
    }

    inline basic_t::basic_t()
    : src(INVALID_ACTOR)
    {
      ;
    }

    inline basic_t::basic_t(actorPID_t src)
    : src(src)
    {
      ;
    }

    inline poison_t::poison_t()
    {
      ;
    }

    inline const std::string& setName_t::Name() const
    {
      return this->name;
    }

    inline setName_t::setName_t(const std::string& name)
    : name(name)
    {
      ;
    }

    inline setName_t::setName_t(std::string&& name)
    : name(std::move(name))
    {
      ;
    }

    inline int keyEvent_t::Key() const
    {
      return this->key;
    }

    inline int keyEvent_t::Press() const
    {
      return this->press;
    }

    inline keyEvent_t::keyEvent_t(int key, int press)
    : key(key), press(press)
    {
      ;
    }

  }

  using msg_t = std::unique_ptr<bb::msg::basic_t>;

  msg_t IssuePoison();

  msg_t IssueSetName(const char* name);

  template<typename castType_t>
  castType_t* As(msg_t& msg)
  {
    static_assert(std::is_base_of<bb::msg::basic_t, castType_t>::value,
      "Can be used only with bb::msg::basic_t subclasses"
    );

    if (auto* ptr = msg.get())
    {
      if (typeid(*ptr) != typeid(castType_t))
      {
        return nullptr;
      }
      return static_cast<castType_t*>(msg.get());
    }
    return nullptr;
  }

  template<typename castType_t>
  const castType_t* As(const msg_t& msg)
  {
    return As<castType_t>(const_cast<msg_t&>(msg));
  }

  template<typename msgType_t, typename... args_t>
  msg_t Issue(args_t&& ... args)
  {
    static_assert(std::is_base_of<bb::msg::basic_t, msgType_t>::value,
      "Can be used only with bb::msg::basic_t subclasses"
    );
    return msg_t(new msgType_t(std::forward<args_t>(args)...));
  }


}

#endif /* __BB_CORE_MSG_HEADER__ */
