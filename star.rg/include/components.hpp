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
    uint32_t moveTime; // Сколько времени на движение 
    uint32_t curTime;  // Сколько уже есть времени
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

  enum unitSide_t
  {
    SIDE_UP = 0,
    SIDE_UP_RIGHT,
    SIDE_RIGHT,
    SIDE_DOWN_RIGHT,
    SIDE_DOWN,
    SIDE_DOWN_LEFT,
    SIDE_LEFT,
    SIDE_UP_LEFT,
    SIDE_TOTAL
  };

  const glm::vec2 sideVec[SIDE_TOTAL] = {
    {  0.0f, -1.0f },
    {  1.0f, -1.0f },
    {  1.0f,  0.0f },
    {  1.0f,  1.0f },
    {  0.0f,  1.0f },
    { -1.0f,  1.0f },
    { -1.0f,  0.0f },
    { -1.0f, -1.0f }
  };

  const glm::ivec2 iSideVec[SIDE_TOTAL] = {
    {  0, -1 },
    {  1, -1 },
    {  1,  0 },
    {  1,  1 },
    {  0,  1 },
    { -1,  1 },
    { -1,  0 },
    { -1, -1 }
  };

  struct userInputData_t
  {
    unitSide_t side;
    uint32_t steps;
  };

  using pos_t = component_t<posData_t>;
  using sprite_t = component_t<spriteData_t>;
  using melee_t = component_t<meleeData_t>;
  using update_t = component_t<updateData_t>;
  using ai_t = component_t<aiData_t>;
  using status_t = component_t<statusData_t>;
  using userInput_t = component_t<userInputData_t>;

}

#endif /* STARRG_COMPONENTS_HEADER */
