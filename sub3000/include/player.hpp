/**
 * @file player.hpp
 * 
 * Player routines and data
 * 
 */

#include <msg.hpp>
#include <heightMap.hpp>
#include <control.hpp>
#include <deci.hpp>

namespace sub3000
{
  namespace radar
  {
    enum mode_t
    {
      radius360 = 0,
      front90 = 1
    };
  }

  namespace player
  {

    struct data_t final
    {
      bb::vec2_t pos;
      bb::vec2_t vel;
      float angle;
      float aVel;

      engine::mode_t engine;
      rudder::mode_t rudder;
      radar::mode_t radar;

      float engineOutput;
      float rudderPos;

      float mass;
      float rotMoment;

      engine::modeList_t engineModeList;
      float maxOutputChange;
      float maxAngleChange;

      float width;
      float length;

      float dragCoeff;
      float crossSection;

      bool clip;
      bb::deci_t radarAngle;
      bb::deci_t radarAngleDelta;

      float depth;

      bb::vec2_t Dir() const;

      float RadarAngle() const;

      data_t();

      data_t(const data_t&) = default;
      data_t& operator=(const data_t&) = default;
      data_t(data_t&&) = default;
      data_t& operator=(data_t&&) = default;
      ~data_t() = default;

    };

    inline float data_t::RadarAngle() const
    {
      return static_cast<float>(static_cast<double>(this->radarAngle));
    }

    inline bb::vec2_t data_t::Dir() const
    {
      return bb::Dir(-this->angle);
    }

    inline data_t::data_t()
    : pos(0.0f),
      vel(0.0f),
      angle(0.0f),
      aVel(0.0f),
      engine(engine::stop),
      rudder(rudder::midship),
      radar(radar::radius360),
      engineOutput(0.0f),
      rudderPos(0.0f),
      mass(1.0f),
      rotMoment(1.0f),
      maxOutputChange(0.1f),
      maxAngleChange(0.3f),
      width(1.0f),
      length(1.0f),
      dragCoeff(0.0f),
      crossSection(0.0f),
      clip(true),
      radarAngle(0),
      radarAngleDelta(1),
      depth(28.0f)
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
      status_t(args_t&& ... args);

      status_t(const status_t&) = default;
      status_t& operator=(const status_t&) = default;
      status_t(status_t&&) = default;
      status_t& operator=(status_t&&) = default;
      ~status_t() override = default;

    };

    template<typename... args_t>
    inline status_t::status_t(args_t&& ... args)
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

    void Update(data_t* data, const bb::ext::heightMap_t& hmap, float dt);

    int Control(data_t* data, const bb::msg::keyEvent_t& key);

  } // namespace player

} // namespace sub3000
