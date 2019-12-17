/**
 * @file splash.h
 * @author splash scene
 */

#pragma once
#ifndef __SUB3000_SPLASH_HEADER__
#define __SUB3000_SPLASH_HEADER__

#include <scene.hpp>
#include <string>

#include <context.hpp>

namespace sub3000
{

  class splashScene_t final: public scene_t
  {
    double      duration;
    std::string shader_vp;
    std::string shader_fp;
    std::string logo;

    bb::shader_t  shader;
    bb::vao_t     object;
    bb::texture_t texture;
    int           timePassedUniform;

    double currentTimePassed;

    bb::context_t* pContext;

    void OnPrepare() override;
    void OnUpdate(double delta) override;
    void OnRender() override;
    void OnCleanup() override;

  public:

    splashScene_t();
    ~splashScene_t() override;

  };

  
} // namespace sub3000

#endif /* __SUB3000_SPLASH_HEADER__ */