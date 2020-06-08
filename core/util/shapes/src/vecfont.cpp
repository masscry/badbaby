#include <vecfont.hpp>
#include <common.hpp>

namespace
{

  const auto BreakPoint = glm::vec2(2.0f, 0.0f);

  const glm::vec2 tZero[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    BreakPoint,
    { 0.0f, 1.0f },
    { 0.5f, 0.0f },
  };

  const glm::vec2 tOne[] = {
    { 0.0f, 0.5f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f }
  };

  const glm::vec2 tTwo[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 0.5f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f }
  };

  const glm::vec2 tThree[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 tFour[] = {
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.0f, 0.5f },
    { 0.5f, 0.5f }
  };

  const glm::vec2 tFive[] = {
    { 0.5f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
    { 0.5f, 1.0f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 tSix[] = {
    { 0.5f, 0.0f },
    { 0.0f, 0.5f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.5f },
    { 0.0f, 0.5f }
  };

  const glm::vec2 tSeven[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.0f, 0.5f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 tEight[] = {
    { 0.0f, 0.5f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.5f },
    { 0.5f, 0.5f }
  };

  const glm::vec2 tNine[] = {
    { 0.5f, 0.5f },
    { 0.0f, 0.5f },
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 0.5f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 tMinus[] = {
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
  };

  const glm::vec2 tGraveAccent[] = {
    { 0.10f, 0.0f },
    { 0.30f, 0.3f },
  };

  const glm::vec2 tTilde[] = {
    { 0.0f, 0.5f },
    { 0.15f, 0.4f },
    { 0.35f, 0.6f },
    { 0.5f, 0.5f },
  };

  const glm::vec2 tEqual[] = {
    { 0.0f, 0.4f },
    { 0.5f, 0.4f },
    BreakPoint,
    { 0.0f, 0.6f },
    { 0.5f, 0.6f },
  };

  const glm::vec2 tPlus[] = {
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
    BreakPoint,
    { 0.25f, 0.25f },
    { 0.25f, 0.75f }
  };

  const glm::vec2 tUndef[] = {
    { 0.0f, 0.75f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.75f },
  };

  const glm::vec2 tUnderscore[] = {
    { 0.0f, 1.0f },
    { 0.5f, 1.0f }
  };

  const glm::vec2 tLetA[] = {
    { 0.00f, 1.00f },
    { 0.25f, 0.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.05f, 0.75f },
    { 0.45f, 0.75f },
  };

  const glm::vec2 tLetCyrB[] = {
    { 0.40f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.50f },
    { 0.00f, 0.50f }
  };

  const glm::vec2 tLetB[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    { 0.40f, 0.00f },
    { 0.40f, 0.50f },
    { 0.00f, 0.50f },
    { 0.50f, 0.50f },
    { 0.50f, 1.00f },
    { 0.00f, 1.00f }
  };

  const glm::vec2 tLetC[] = {
    { 0.50f, 0.30f },
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.70f },
  };

  const glm::vec2 tLetD[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.40f, 1.00f },
    { 0.50f, 0.70f },
    { 0.50f, 0.30f },
    { 0.40f, 0.00f },
    { 0.00f, 0.00f },
  };

  const glm::vec2 tLetE[] = {
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.00f, 0.50f },
    { 0.40f, 0.50f },
  };
  
  const glm::vec2 tLetCyrEA[] = {
    { 0.00f, 0.00f },
    { 0.40f, 0.10f },
    { 0.50f, 0.50f },
    { 0.40f, 0.90f },
    { 0.00f, 1.00f },
    BreakPoint,
    { 0.50f, 0.50f },
    { 0.10f, 0.50f },
  };

  const glm::vec2 tLetIE[] = {
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.00f, 0.50f },
    { 0.40f, 0.50f },
    BreakPoint,
    { 0.05f, -0.15f },
    { 0.20f, -0.15f },
    BreakPoint,
    { 0.30f, -0.15f },
    { 0.45f, -0.15f },
  };

  const glm::vec2 tLetF[] = {
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    BreakPoint,
    { 0.00f, 0.50f },
    { 0.40f, 0.50f },
  };

  const glm::vec2 tLetG[] = {
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.50f },
    { 0.30f, 0.50f }
  };

  const glm::vec2 tLetH[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    BreakPoint,
    { 0.50f, 1.00f },
    { 0.50f, 0.00f },
    BreakPoint,
    { 0.00f, 0.50f },
    { 0.50f, 0.50f },
  };

  const glm::vec2 tLetCyrP[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    { 0.50f, 0.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetCyrC[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.60f, 1.00f },
    { 0.50f, 1.15f },
    BreakPoint,
    { 0.50f, 0.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetCyrCH[] = {
    { 0.00f, 0.00f },
    { 0.00f, 0.50f },
    { 0.50f, 0.50f },
    BreakPoint,
    { 0.50f, 0.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetCyrSH[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.00f },
    BreakPoint,
    { 0.25f, 1.00f },
    { 0.25f, 0.25f }
  };

  const glm::vec2 tLetCyrSHA[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.65f, 1.00f },
    { 0.65f, 1.15f },
    BreakPoint,
    { 0.50f, 1.00f },
    { 0.50f, 0.00f },
    BreakPoint,
    { 0.25f, 1.00f },
    { 0.25f, 0.25f }
  };


  const glm::vec2 tLetI[] = {
    { 0.40f, 0.00f },
    { 0.10f, 0.00f },
    BreakPoint,
    { 0.40f, 1.00f },
    { 0.10f, 1.00f },
    BreakPoint,
    { 0.25f, 0.00f },
    { 0.25f, 1.00f },
  };

  const glm::vec2 tLetJ[] = {
    { 0.10f, 0.00f },
    { 0.40f, 0.00f },
    { 0.40f, 1.00f },
    { 0.10f, 1.00f },
    { 0.10f, 0.70f }
  };

  const glm::vec2 tLetK[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    BreakPoint,
    { 0.40f, 0.00f },
    { 0.00f, 0.40f },
    { 0.50f, 1.00f },
  };

  const glm::vec2 tLetL[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetCyrG[] = {
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f }
  };

  const glm::vec2 tLetM[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    { 0.25f, 0.40f },
    { 0.50f, 0.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetN[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tNumero[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    { 0.20f, 1.00f },
    { 0.20f, 0.00f },
    BreakPoint,
    { 0.30f, 1.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.30f, 0.80f },
    { 0.50f, 0.80f },
    { 0.50f, 0.50f },
    { 0.30f, 0.50f },
    { 0.30f, 0.80f },
  };

  const glm::vec2 tLetCyrI[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 0.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetCyrShortI[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 0.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.00f, -0.15f },
    { 0.25f,  0.00f },
    { 0.50f, -0.15f }
  };

  const glm::vec2 tLetO[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f }
  };

  const glm::vec2 tLetCyrUY[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    BreakPoint,
    { 0.00f, 0.50f },
    { 0.25f, 0.50f },
    BreakPoint,
    { 0.25f, 0.00f },
    { 0.50f, 0.00f },
    { 0.50f, 1.00f },
    { 0.25f, 1.00f },
    { 0.25f, 0.00f }
  };

  const glm::vec2 tLetP[] = {
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 0.5f },
    { 0.0f, 0.5f }
  };

  const glm::vec2 tLetCyrD[] = {
    { 0.1f, 0.8f },
    { 0.1f, 0.0f },
    { 0.4f, 0.0f },
    { 0.4f, 0.8f },
    BreakPoint,
    { 0.1f, 1.0f },
    { 0.0f, 0.8f },
    { 0.5f, 0.8f },
    { 0.4f, 1.0f },
  };

  const glm::vec2 tLetCyrF[] = {
    { 0.25f, 1.0f },
    { 0.25f, 0.0f },
    BreakPoint,
    { 0.00f, 0.1f },
    { 0.00f, 0.6f },
    { 0.50f, 0.6f },
    { 0.50f, 0.1f },
    { 0.00f, 0.1f }
  };

  const glm::vec2 tLetCyrSoft[] = {
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.5f },
    { 0.0f, 0.5f }
  };

  const glm::vec2 tLetCyrStrong[] = {
    { 0.00f, 0.00f },
    { 0.20f, 0.00f },
    { 0.20f, 1.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.50f },
    { 0.20f, 0.50f }
  };

  const glm::vec2 tLetCyrHardI[] = {
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
    { 0.35f, 1.0f },
    { 0.35f, 0.5f },
    { 0.0f, 0.5f },
    BreakPoint,
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
  };

  const glm::vec2 tLetQ[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 0.7f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    BreakPoint,
    { 0.5f, 1.0f },
    { 0.2f, 0.6f }
  };


  const glm::vec2 tLetR[] = {
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    { 0.4f, 0.0f },
    { 0.4f, 0.5f },
    { 0.0f, 0.5f },
    BreakPoint,
    { 0.0f, 0.5f },
    { 0.5f, 1.0f }
  };

  const glm::vec2 tLetCyrYA[] = {
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.1f, 0.0f },
    { 0.1f, 0.5f },
    { 0.5f, 0.5f },
    BreakPoint,
    { 0.5f, 0.5f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 tLetS[] = {
    { 0.5f,  0.25f },
    { 0.5f,  0.00f },
    { 0.0f,  0.00f },
    { 0.0f,  0.50f },
    { 0.5f,  0.50f },
    { 0.5f,  1.00f },
    { 0.0f,  1.00f },
    { 0.0f,  0.75f },
  };

  const glm::vec2 tDollar[] = {
    { 0.5f,  0.25f },
    { 0.5f,  0.15f },
    { 0.0f,  0.15f },
    { 0.0f,  0.45f },
    { 0.5f,  0.45f },
    { 0.5f,  0.85f },
    { 0.0f,  0.85f },
    { 0.0f,  0.75f },
    BreakPoint,
    { 0.20f,  0.00f },
    { 0.20f,  1.00f },
    BreakPoint,
    { 0.30f,  0.00f },
    { 0.30f,  1.00f }
  };

  const glm::vec2 tLetT[] = {
    { 0.25f, 1.00f },
    { 0.25f, 0.00f },
    BreakPoint,
    { 0.00f, 0.00f },
    { 0.50f, 0.00f },
  };

  const glm::vec2 tLetU[] = {
    { 0.00f, 0.00f },
    { 0.00f, 0.80f },
    { 0.25f, 1.00f },
    { 0.50f, 0.80f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tLetCyrL[] = {
    { 0.00f, 1.00f },
    { 0.25f, 0.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetV[] = {
    { 0.00f, 0.00f },
    { 0.25f, 1.00f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tPower[] = {
    { 0.00f, 0.20f },
    { 0.25f, 0.00f },
    { 0.50f, 0.20f }
  };

  const glm::vec2 tLetW[] = {
    { 0.00f, 0.00f },
    { 0.15f, 1.00f },
    { 0.25f, 0.70f },
    { 0.35f, 1.00f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tLetX[] = {
    { 0.00f, 0.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.00f, 1.00f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tLetCyrZH[] = {
    { 0.00f, 0.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.00f, 1.00f },
    { 0.50f, 0.00f },
    BreakPoint,
    { 0.25f, 0.00f },
    { 0.25f, 1.00f },
  };

  const glm::vec2 tLetY[] = {
    { 0.00f, 0.00f },
    { 0.25f, 0.50f },
    { 0.50f, 0.00f },
    BreakPoint,
    { 0.25f, 0.50f },
    { 0.25f, 1.00f },
  };

  const glm::vec2 tLetCyrU[] = {
    { 0.50f, 0.00f },
    { 0.00f, 1.00f },
    BreakPoint,
    { 0.00f, 0.00f },
    { 0.25f, 0.50f },
  };

  const glm::vec2 tLetZ[] = {
    { 0.00f, 0.00f },
    { 0.50f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
  };

  const glm::vec2 tExclamation[] = {
    { 0.25f, 0.00f },
    { 0.25f, 0.70f },
    BreakPoint,
    { 0.25f, 0.80f },
    { 0.25f, 1.00f },
  };

  const glm::vec2 tQuestion[] = {
    { 0.00f, 0.20f },
    { 0.00f, 0.00f },
    { 0.50f, 0.00f },
    { 0.50f, 0.20f },
    { 0.25f, 0.70f },
    BreakPoint,
    { 0.25f, 0.80f },
    { 0.25f, 1.00f },
  };

  const glm::vec2 tOctotorp[] = {
    { 0.20f, 0.1f },
    { 0.10f, 0.9f },
    BreakPoint,
    { 0.40f, 0.1f },
    { 0.30f, 0.9f },
    BreakPoint,
    { 0.00f, 0.4f },
    { 0.50f, 0.4f },
    BreakPoint,
    { 0.00f, 0.6f },
    { 0.50f, 0.6f }
  };

  const glm::vec2 tSquareOpen[] = {
    { 0.30f, 0.0f },
    { 0.10f, 0.2f },
    { 0.10f, 0.8f },
    { 0.30f, 1.0f }
  };

  const glm::vec2 tSquareClose[] = {
    { 0.20f, 0.0f },
    { 0.40f, 0.2f },
    { 0.40f, 0.8f },
    { 0.20f, 1.0f }
  };

  const glm::vec2 tRoundOpen[] = {
    { 0.30f, 0.0f },
    { 0.10f, 0.0f },
    { 0.10f, 1.0f },
    { 0.30f, 1.0f }
  };

  const glm::vec2 tRoundClose[] = {
    { 0.20f, 0.0f },
    { 0.40f, 0.0f },
    { 0.40f, 1.0f },
    { 0.20f, 1.0f }
  };

  const glm::vec2 tCurlyOpen[] = {
    { 0.40f, 0.00f },
    { 0.20f, 0.10f },
    { 0.25f, 0.30f },
    { 0.10f, 0.50f },
    { 0.25f, 0.70f },
    { 0.20f, 0.90f },
    { 0.40f, 1.00f }
  };

  const glm::vec2 tCurlyClose[] = {
    { 0.10f, 0.00f },
    { 0.30f, 0.10f },
    { 0.25f, 0.30f },
    { 0.40f, 0.50f },
    { 0.25f, 0.70f },
    { 0.30f, 0.90f },
    { 0.10f, 1.00f }
  };

  const glm::vec2 tLess[] = {
    { 0.40f, 0.25f },
    { 0.10f, 0.50f },
    { 0.40f, 0.75f }
  };

  const glm::vec2 tGreater[] = {
    { 0.10f, 0.25f },
    { 0.40f, 0.50f },
    { 0.10f, 0.75f }
  };

  const glm::vec2 tSlash[] = {
    { 0.5f, 0.0f },
    { 0.0f, 1.0f },
  };
  const glm::vec2 tReverseSlash[] = {
    { 0.0f, 0.0f },
    { 0.5f, 1.0f },
  };

  const glm::vec2 tVertical[] = {
    { 0.25f, 0.0f },
    { 0.25f, 1.0f },
  };

  const glm::vec2 tDotComma[] = {
    { 0.25f, 0.2f },
    { 0.25f, 0.4f },
    BreakPoint,
    { 0.25f, 0.5f },
    { 0.25f, 0.8f },
    { 0.15f, 0.9f },
  };
  
  const glm::vec2 tDoubleDot[] = {
    { 0.25f, 0.2f },
    { 0.25f, 0.4f },
    BreakPoint,
    { 0.25f, 0.5f },
    { 0.25f, 0.7f },
  };

  const glm::vec2 tDot[] = {
    { 0.25f, 0.7f },
    { 0.25f, 0.9f },
  };

  const glm::vec2 tComma[] = {
    { 0.25f, 0.7f },
    { 0.25f, 0.9f },
    { 0.15f, 1.1f }
  };

  const glm::vec2 tQuote[] = {
    { 0.25f, 0.2f },
    { 0.25f, 0.4f }
  };

  const glm::vec2 tDoubleQuote[] = {
    { 0.25f, 0.2f },
    { 0.25f, 0.4f },
    BreakPoint,
    { 0.45f, 0.2f },
    { 0.45f, 0.4f },
  };

  const glm::vec2 tPercent[] = {
    { 0.5f, 0.0f },
    { 0.0f, 1.0f },
    BreakPoint,
    { 0.0f, 0.0f },
    { 0.0f, 0.3f },
    BreakPoint,
    { 0.5f, 0.7f },
    { 0.5f, 1.0f },
  };

  const glm::vec2 tStar[] = {
    {  0.25f, 0.75f},
    {  0.25f, 0.25f},
    BreakPoint,
    {  0.00f, 0.67f},
    {  0.50f, 0.33f},
    BreakPoint,
    {  0.00f, 0.33f},
    {  0.50f, 0.67f}
  };

  const glm::vec2 tAt[] = {
    { 0.35f, 0.80f },
    { 0.35f, 0.30f },
    { 0.10f, 0.30f },
    { 0.10f, 0.80f },
    { 0.50f, 0.80f },
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tAmpersand[] = {
    { 0.50f, 0.20f },
    { 0.40f, 0.00f },
    { 0.10f, 0.10f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.50f, 0.70f },
    { 0.25f, 1.00f },
    { 0.00f, 0.70f },
    { 0.20f, 0.40f },
  };

  struct symbol_t
  {
    const glm::vec2* v;
    size_t size;
  };

  using vectorFont_t = std::map<wint_t, symbol_t>;

  #define SYMBOL(SMB, ARRAY) { (SMB), { (ARRAY), bb::countof(ARRAY) } }

  vectorFont_t vectorFont = {
    SYMBOL(U'0', tZero),
    SYMBOL(U'1', tOne),
    SYMBOL(U'2', tTwo),
    SYMBOL(U'3', tThree),
    SYMBOL(U'4', tFour),
    SYMBOL(U'5', tFive),
    SYMBOL(U'6', tSix),
    SYMBOL(U'7', tSeven),
    SYMBOL(U'8', tEight),
    SYMBOL(U'9', tNine),
    SYMBOL(U'A', tLetA),
    SYMBOL(U'a', tLetA),
    SYMBOL(U'B', tLetB),
    SYMBOL(U'b', tLetB),
    SYMBOL(U'C', tLetC),
    SYMBOL(U'c', tLetC),
    SYMBOL(U'D', tLetD),
    SYMBOL(U'd', tLetD),
    SYMBOL(U'E', tLetE),
    SYMBOL(U'e', tLetE),
    SYMBOL(U'F', tLetF),
    SYMBOL(U'f', tLetF),
    SYMBOL(U'G', tLetG),
    SYMBOL(U'g', tLetG),
    SYMBOL(U'H', tLetH),
    SYMBOL(U'h', tLetH),
    SYMBOL(U'I', tLetI),
    SYMBOL(U'i', tLetI),
    SYMBOL(U'J', tLetJ),
    SYMBOL(U'j', tLetJ),
    SYMBOL(U'K', tLetK),
    SYMBOL(U'k', tLetK),
    SYMBOL(U'L', tLetL),
    SYMBOL(U'l', tLetL),
    SYMBOL(U'M', tLetM),
    SYMBOL(U'm', tLetM),
    SYMBOL(U'N', tLetN),
    SYMBOL(U'n', tLetN),
    SYMBOL(U'O', tLetO),
    SYMBOL(U'o', tLetO),
    SYMBOL(U'P', tLetP),
    SYMBOL(U'p', tLetP),
    SYMBOL(U'Q', tLetQ),
    SYMBOL(U'q', tLetQ),
    SYMBOL(U'R', tLetR),
    SYMBOL(U'r', tLetR),
    SYMBOL(U'S', tLetS),
    SYMBOL(U's', tLetS),
    SYMBOL(U'T', tLetT),
    SYMBOL(U't', tLetT),
    SYMBOL(U'U', tLetU),
    SYMBOL(U'u', tLetU),
    SYMBOL(U'V', tLetV),
    SYMBOL(U'v', tLetV),
    SYMBOL(U'W', tLetW),
    SYMBOL(U'w', tLetW),
    SYMBOL(U'X', tLetX),
    SYMBOL(U'x', tLetX),
    SYMBOL(U'Y', tLetY),
    SYMBOL(U'y', tLetY), 
    SYMBOL(U'Z', tLetZ),
    SYMBOL(U'z', tLetZ),
    SYMBOL(U'_', tUnderscore),
    SYMBOL(U'-', tMinus),
    SYMBOL(U'`', tGraveAccent),
    SYMBOL(U'~', tTilde),
    SYMBOL(U'=', tEqual),
    SYMBOL(U'+', tPlus),
    SYMBOL(U'A', tLetA),
    SYMBOL(U'а', tLetA),
    SYMBOL(U'Б', tLetCyrB),
    SYMBOL(U'б', tLetCyrB),
    SYMBOL(U'В', tLetB),
    SYMBOL(U'в', tLetB),
    SYMBOL(U'Г', tLetCyrG),
    SYMBOL(U'г', tLetCyrG),
    SYMBOL(U'Д', tLetCyrD),
    SYMBOL(U'д', tLetCyrD),
    SYMBOL(U'Е', tLetE),
    SYMBOL(U'е', tLetE),
    SYMBOL(U'Ё', tLetIE),
    SYMBOL(U'ё', tLetIE),
    SYMBOL(U'Ж', tLetCyrZH),
    SYMBOL(U'ж', tLetCyrZH),
    SYMBOL(U'З', tThree),
    SYMBOL(U'з', tThree),
    SYMBOL(U'И', tLetCyrI),
    SYMBOL(U'и', tLetCyrI),
    SYMBOL(U'Й', tLetCyrShortI),
    SYMBOL(U'й', tLetCyrShortI),
    SYMBOL(U'К', tLetK),
    SYMBOL(U'к', tLetK),
    SYMBOL(U'Л', tLetCyrL),
    SYMBOL(U'л', tLetCyrL),
    SYMBOL(U'М', tLetM),
    SYMBOL(U'м', tLetM),
    SYMBOL(U'Н', tLetH),
    SYMBOL(U'н', tLetH),
    SYMBOL(U'Р', tLetP),
    SYMBOL(U'р', tLetP),
    SYMBOL(U'О', tLetO),
    SYMBOL(U'о', tLetO),
    SYMBOL(U'П', tLetCyrP),
    SYMBOL(U'п', tLetCyrP),
    SYMBOL(U'С', tLetC),
    SYMBOL(U'с', tLetC),
    SYMBOL(U'Т', tLetT),
    SYMBOL(U'т', tLetT),
    SYMBOL(U'У', tLetCyrU),
    SYMBOL(U'у', tLetCyrU),
    SYMBOL(U'Ф', tLetCyrF),
    SYMBOL(U'ф', tLetCyrF),
    SYMBOL(U'Х', tLetX),
    SYMBOL(U'х', tLetX),
    SYMBOL(U'Ц', tLetCyrC),
    SYMBOL(U'ц', tLetCyrC),
    SYMBOL(U'Ч', tLetCyrCH),
    SYMBOL(U'ч', tLetCyrCH),
    SYMBOL(U'Ш', tLetCyrSH),
    SYMBOL(U'ш', tLetCyrSH),
    SYMBOL(U'Щ', tLetCyrSHA),
    SYMBOL(U'щ', tLetCyrSHA),
    SYMBOL(U'Ы', tLetCyrHardI),
    SYMBOL(U'ы', tLetCyrHardI),
    SYMBOL(U'Ь', tLetCyrSoft),
    SYMBOL(U'ь', tLetCyrSoft),
    SYMBOL(U'Ъ', tLetCyrStrong),
    SYMBOL(U'ъ', tLetCyrStrong),
    SYMBOL(U'Э', tLetCyrEA),
    SYMBOL(U'э', tLetCyrEA),
    SYMBOL(U'Ю', tLetCyrUY),
    SYMBOL(U'ю', tLetCyrUY),
    SYMBOL(U'Я', tLetCyrYA),
    SYMBOL(U'я', tLetCyrYA),
    SYMBOL(U'!', tExclamation),
    SYMBOL(U'?', tQuestion),
    SYMBOL(U'#', tOctotorp),
    SYMBOL(U'[', tSquareOpen),
    SYMBOL(U']', tSquareClose),
    SYMBOL(U'(', tRoundOpen),
    SYMBOL(U')', tRoundClose),
    SYMBOL(U'{', tCurlyOpen),
    SYMBOL(U'}', tCurlyClose),
    SYMBOL(U'/', tSlash),
    SYMBOL(U'\\', tReverseSlash),
    SYMBOL(U'|', tVertical),
    SYMBOL(U';', tDotComma),
    SYMBOL(U':', tDoubleDot),
    SYMBOL(U'.', tDot),
    SYMBOL(U',', tComma),
    SYMBOL(U'\'', tQuote),
    SYMBOL(U'\"', tDoubleQuote),
    SYMBOL(U'$', tDollar),
    SYMBOL(U'^', tPower),
    SYMBOL(U'%', tPercent),
    SYMBOL(U'*', tStar),
    SYMBOL(U'@', tAt),
    SYMBOL(U'&', tAmpersand),
    SYMBOL(U'<', tLess),
    SYMBOL(U'>', tGreater),
    SYMBOL(U'№', tNumero),
  };

  symbol_t dummy = {
    tUndef,
    bb::countof(tUndef)
  };

} // namespace

namespace bb
{

  const glm::vec2* VectorFontSymbol(wint_t smb, size_t* pSize)
  {
    if (pSize == nullptr)
    { // programmer's error
      assert(0);
      return nullptr;
    }

    auto it = vectorFont.find(smb);
    if (it == vectorFont.end())
    {
      *pSize = dummy.size;
      return dummy.v;
    }

    *pSize = it->second.size;
    return it->second.v;
  }

} // namespace bb