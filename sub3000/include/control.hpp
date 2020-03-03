/**
 * @file control.hpp
 * 
 * Control ship functions
 * 
 */

#pragma once
#ifndef __SUB3000_CONTROL_HEADER__
#define __SUB3000_CONTROL_HEADER__

#include <cassert>
#include <cmath>

namespace sub3000
{

  namespace engine
  {
    enum mode_t
    {
      full_ahead = 0,
      half_ahead,
      slow_ahead,
      dead_slow_ahead,
      stop,
      dead_slow_astern,
      slow_astern,
      half_astern,
      full_astern,
      total
    };

    inline const char* ToString(mode_t mode)
    {
      switch (mode)
      {
        case full_ahead:
          return "Full Ahead";
        case half_ahead:
          return "Half Ahead";
        case slow_ahead:
          return "Slow Ahead";
        case dead_slow_ahead:
          return "Dead Slow Ahead";
        case stop:
          return "Stop";
        case dead_slow_astern:
          return "Dead Slow Astern";
        case slow_astern:
          return "Slow Astern";
        case half_astern:
          return "Half Astern";
        case full_astern:
          return "Full Astern";
        default:
          assert(0);
          return "???";
      }
    }

    inline float Output(mode_t mode)
    {
      switch(mode)
      {
      case full_ahead:
        return 0.5f;
      case half_ahead:
        return 0.25f;
      case slow_ahead:
        return 0.125f;
      case dead_slow_ahead:
        return 0.05f;
      case stop:
        return 0.0f;
      case dead_slow_astern:
        return -0.025f;
      case slow_astern:
        return -0.05f;
      case half_astern:
        return -0.1f;
      case full_astern:
        return -0.2f;
      default:
        // Programmer's mistake!
        assert(0);
        return 0.0f;
      }
    }

  } // namespace engine

  namespace rudder
  {
    enum mode_t
    {
      left_40 = 0,
      left_20,
      left_10,
      midship,
      right_10,
      right_20,
      right_40,
      total
    };

    inline const char* ToString(mode_t mode)
    {
      switch(mode)
      {
      case left_40:
        return "LEFT 40°";
      case left_20:
        return "LEFT 20°";
      case left_10:
        return "LEFT 10°";
      case midship:
        return "MIDSHIP";
      case right_10:
        return "RIGHT 10°";
      case right_20:
        return "RIGHT 20°";
      case right_40:
        return "RIGHT 40°";
      default:
        assert(0);
        return "???";
      }
    }

    inline float Output(mode_t mode)
    {
      switch(mode)
      {
        case left_40:
          return static_cast<float>(40.0*M_PI/180.0);
        case left_20:
          return static_cast<float>(20.0*M_PI/180.0);
        case left_10:
          return static_cast<float>(10.0*M_PI/180.0);
        case midship:
          return 0.0f;
        case right_10:
          return static_cast<float>(-10.0*M_PI/180.0);
        case right_20:
          return static_cast<float>(-20.0*M_PI/180.0);
        case right_40:
          return static_cast<float>(-40.0*M_PI/180.0);
        default:
          // Programmer's mistake!
          assert(0);
          return 0.0f;
      }
    }

  } // namespace rudder

} // namespace sub3000

#endif /* __SUB3000_CONTROL_HEADER__ */