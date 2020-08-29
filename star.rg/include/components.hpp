#pragma once

#ifndef STARRG_COMPONENTS_HEADER
#define STARRG_COMPONENTS_HEADER

#include <ecs.hpp>

namespace sr
{

  /**
   * Положение
   */
  struct posData_t
  {
    glm::ivec2 v;
  };

  /**
   * Спрайт
   */
  struct spriteData_t
  {
    glm::ivec2 id;
  };

  /**
   * Ближний бой
   */
  struct meleeData_t
  {
    int skill;      // probability to hit
    int stren;      // hit strenght
    int tough;      // unit toughness
    int armor;      // armor rating
  };

  /**
   * Обработка ходов
   */
  struct updateData_t
  {
    float moveTime; // Сколько времени на движение 
    float curTime;  // Сколько уже есть времени
  };

  /**
   * Режимы ИИ
   */
  enum aiMode_t
  {
    AI_STALKER = 0, // Бродитель
    AI_SCARED = 1,  // Испуганный
    AI_ANGRY = 2    // Злой
  };

  /**
   * Обработка ИИ
   */
  struct aiData_t
  {
    aiMode_t ai;    // Режим ИИ
  };

  enum unitStatus_t
  {
    US_NONE = 0,
    US_MISS,
    US_ARMOR,
    US_SAVE,
    US_DEAD
  };

  struct statusData_t
  {
    unitStatus_t status;
    glm::vec2 side;
  };

  using pos_t = component_t<posData_t>;
  using sprite_t = component_t<spriteData_t>;
  using melee_t = component_t<meleeData_t>;
  using update_t = component_t<updateData_t>;
  using ai_t = component_t<aiData_t>;
  using status_t = component_t<statusData_t>;

}

#endif /* STARRG_COMPONENTS_HEADER */
