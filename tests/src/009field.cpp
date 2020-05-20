#include <heightMap.hpp>
#include <distanceMap.hpp>

#include <cstdio>
#include <string>

using namespace bb::ext;

namespace std
{

  inline string to_string(const bb::vec3_t& vec)
  {
    return string("[")
      + to_string(vec.x) + ";"
      + to_string(vec.y) + ";"
      + to_string(vec.z) + "]";
  }

} // namespace std

#define TEST_EQUAL(real, expect) do {\
  if ((real) != (expect))\
  {\
    fprintf(stderr, "%s:%d: error: %s expected %s, got %s\n", __FILE__, __LINE__, #real, std::to_string(expect).c_str(), std::to_string(real).c_str());\
    exit(-1);\
  }\
} while(0)

#define TEST(expr) do {\
  if (!(expr))\
  {\
    fprintf(stderr, "%s:%d: error: expression \"%s\" failed\n", __FILE__, __LINE__, #expr);\
    exit(-1);\
  }\
} while(0)


int main(int , char* [])
{
  auto hm = heightMap_t(2, 2);

  hm.Data(0, 0) = 1.0f;
  hm.Data(1, 0) = 2.0f;
  hm.Data(0, 1) = 3.0f;
  hm.Data(1, 1) = 4.0f;

  TEST_EQUAL(hm.Sample(0.0f, 0.0f), 1.0f);
  TEST_EQUAL(hm.Sample(1.0f, 0.0f), 2.0f);
  TEST_EQUAL(hm.Sample(0.0f, 1.0f), 3.0f);
  TEST_EQUAL(hm.Sample(1.0f, 1.0f), 4.0f);
  TEST_EQUAL(hm.Sample(0.5f, 0.5f), 2.5f);

  auto dm = distanceMap_t({2, 2, 2});

  dm.Data(0, 0, 0) = 1.0f;
  dm.Data(1, 0, 0) = 2.0f;
  dm.Data(0, 1, 0) = 3.0f;
  dm.Data(1, 1, 0) = 4.0f;
  dm.Data(0, 0, 1) = 5.0f;
  dm.Data(1, 0, 1) = 6.0f;
  dm.Data(0, 1, 1) = 7.0f;
  dm.Data(1, 1, 1) = 8.0f;

  TEST_EQUAL(dm.Sample(0.0f, 0.0f, 0.0f), 1.0f);
  TEST_EQUAL(dm.Sample(1.0f, 0.0f, 0.0f), 2.0f);
  TEST_EQUAL(dm.Sample(0.0f, 1.0f, 0.0f), 3.0f);
  TEST_EQUAL(dm.Sample(1.0f, 1.0f, 0.0f), 4.0f);
  TEST_EQUAL(dm.Sample(0.0f, 0.0f, 1.0f), 5.0f);
  TEST_EQUAL(dm.Sample(1.0f, 0.0f, 1.0f), 6.0f);
  TEST_EQUAL(dm.Sample(0.0f, 1.0f, 1.0f), 7.0f);
  TEST_EQUAL(dm.Sample(1.0f, 1.0f, 1.0f), 8.0f);
  TEST_EQUAL(dm.Sample(0.5f, 0.5f, 0.5f), 4.5f);

  auto dm2 = distanceMap_t({64, 32, 16});

  for (size_t z = 0; z < 16; ++z)
  {
    for (size_t y = 0; y < 32; ++y)
    {
      for (size_t x = 0; x < 64; ++x)
      {
        dm2.Data(x, y, z) = static_cast<float>(z) - 8;
      }
    }
  }

  dm2.Dump("dm2");
  for (float z = 0; z < 16.0f; z += 1.0f)
  {
    TEST_EQUAL(dm2.Sample(32.0f, 16.0f, z), z - 8.0f);
  }

  bb::vec3_t isec;

  TEST(dm2.CastRay({32.0f, 16.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, &isec, -1.0f));
  TEST_EQUAL(isec, bb::vec3_t(32.0f, 16.0f, 0.0f));

  TEST(dm2.CastRay({32.0f, 16.0f, 10.0f}, {0.0f, 0.0f, -1.0f}, &isec, -1.0f));
  TEST(glm::length(isec - bb::vec3_t(32.0f, 16.0f, 8.0f)) < 0.01f);


  return 0;
}