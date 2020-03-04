/**
 * @file player.hpp
 * 
 * Player routines and data
 * 
 */

#include <msg.hpp>

namespace sub3000
{

  namespace player
  {
    const float MAX_OUTPUT_CHANGE = 0.03f;

    const float MAX_ANGLE_CHANGE = 0.3f;

    struct data_t final
    {
      bb::vec2_t pos;
      bb::vec2_t vel;
      float angle;

      engine::mode_t engine;
      rudder::mode_t rudder;

      float engineOutput;
      float rudderPos;

      float mass;
      float rotMoment;

      data_t();

      data_t(const data_t&) = default;
      data_t& operator=(const data_t&) = default;
      data_t(data_t&&) = default;
      data_t& operator=(data_t&&) = default;
      ~data_t() = default;

    };

    inline data_t::data_t()
    : pos(0.0f),
      vel(0.0f),
      angle(0.0f),
      engine(engine::stop),
      rudder(rudder::midship),
      engineOutput(0.0f),
      rudderPos(0.0f),
      mass(1.0f),
      rotMoment(1.0f)
    {
      ;
    }

    class status_t: public bb::msg::basic_t
    {
      data_t data;

    public:

      data_t& Data();

      const data_t& Data() const;

      template<typename... args_t>
      status_t(args_t ... args);

      status_t(const status_t&) = default;
      status_t& operator=(const status_t&) = default;
      status_t(status_t&&) = default;
      status_t& operator=(status_t&&) = default;
      ~status_t() override = default;

    };

    template<typename... args_t>
    inline status_t::status_t(args_t ... args)
    : bb::msg::basic_t(-1),
      data(std::forward<args_t>(args)...)
    {
      ;
    }

    inline data_t& status_t::Data()
    {
      return this->data;
    }

    inline const data_t& status_t::Data() const
    {
      return this->data;
    }

    void Update(data_t* data, float dt);

    int Control(data_t* data, const bb::msg::keyEvent_t& key);

  } // namespace player

} // namespace sub3000
