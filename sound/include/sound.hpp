/**
 * @file sound.hpp
 * 
 * Simple BASS library wrapper.
 * 
 */

#pragma once
#ifndef __BB_SOUND_HEADER__
#define __BB_SOUND_HEADER__

#include <cstdint>

namespace bb
{

  class sound_t
  {

    sound_t();
    ~sound_t();

  public:

    class stream_t
    {
      friend class sound_t;

      uint32_t handle;

      stream_t(uint32_t handle) noexcept;

    public:

      stream_t() noexcept;
      ~stream_t();

      stream_t(const stream_t&) = delete;
      stream_t& operator=(const stream_t&) = delete;

      stream_t(stream_t&&) noexcept;
      stream_t& operator=(stream_t&&) noexcept;

      bool IsGood() const
      {
        return this->handle != 0;
      }

    };

    class sample_t
    {
      friend class sound_t;

      uint32_t handle;

      sample_t(uint32_t handle) noexcept;

    public:

      sample_t();
      ~sample_t();

      sample_t(const sample_t&) = delete;
      sample_t& operator=(const sample_t&) = delete;

      sample_t(sample_t&&) noexcept;
      sample_t& operator=(sample_t&&) noexcept;

      bool IsGood() const
      {
        return (this->handle != 0);
      }

    };

    int Play(const stream_t& stream);
    int Stop(const stream_t& stream);

    int Play(const sample_t& sample);
    int Stop(const sample_t& sample);

    stream_t CreateStream(const char* filename, bool loop);

    sample_t CreateSample(const char* filename, uint32_t maxSimPlays, bool loop);

    sound_t(const sound_t&) = delete;
    sound_t(sound_t&&) = delete;

    sound_t& operator=(const sound_t&) = delete;
    sound_t& operator=(sound_t&&) = delete;

    static sound_t& Instance();
  };

} // namespace bb

#endif /* __BB_SOUND_HEADER__ */
