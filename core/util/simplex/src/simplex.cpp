#include <simplex.hpp>

#include <cstdlib>
#include <cstring>
#include <cassert>

template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept 
{
    return N;
}

namespace 
{

    const double STRETCH_3D = -1.0 / 6.0;            //(1/Math.sqrt(3+1)-1)/3;
    const double SQUISH_3D = 1.0 / 3.0;              //(Math.sqrt(3+1)-1)/3;
    const double NORM_3D = 1.0 / 103.0;

    inline int FastFloor(double x)
    {
        int xi = (int) x;
        return (x < xi) ? (xi - 1):(xi);
    }

    const double gradients3D[] =
    {
        -11,  4,  4,     -4,  11,  4,    -4,  4,  11,
            11,  4,  4,      4,  11,  4,     4,  4,  11,
            -11, -4,  4,     -4, -11,  4,    -4, -4,  11,
            11, -4,  4,      4, -11,  4,     4, -4,  11,
            -11,  4, -4,     -4,  11, -4,    -4,  4, -11,
            11,  4, -4,      4,  11, -4,     4,  4, -11,
            -11, -4, -4,     -4, -11, -4,    -4, -4, -11,
            11, -4, -4,      4, -11, -4,     4, -4, -11
        };

    typedef class contrib_t {
    public:
        double dx, dy, dz;
        int xsb, ysb, zsb;
        contrib_t* Next;

        contrib_t(double multiplier, int xsb, int ysb, int zsb):
            dx(-xsb - multiplier * SQUISH_3D),
            dy(-ysb - multiplier * SQUISH_3D),
            dz(-zsb - multiplier * SQUISH_3D),
            xsb(xsb), ysb(ysb), zsb(zsb),Next(nullptr) {}

        ~contrib_t()
        {
          delete this->Next;
        }

    } *ptrContrib_t;

    static std::unique_ptr<ptrContrib_t[]> lookup3D;

    static const int a1[] = { 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1 };
    static const int a2[] = { 2, 1, 1, 0, 2, 1, 0, 1, 2, 0, 1, 1, 3, 1, 1, 1 };
    static const int a3[] = { 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 2, 1, 1, 0, 2, 1, 0, 1, 2, 0, 1, 1};
    static const int* base3D[] = { a1, a2, a3 };
    static const size_t base3DSize[] = { countof(a1), countof(a2), countof(a3) };

    static const int p3D[] = 
    {
      0, 0, 1, -1, 0, 0, 1, 0, -1, 0, 0, -1, 1, 0, 0, 0, 1, -1, 0, 0,
      -1, 0, 1, 0, 0, -1, 1, 0, 2, 1, 1, 0, 1, 1, 1, -1, 0, 2, 1, 0, 1,
      1, 1, -1, 1, 0, 2, 0, 1, 1, 1, -1, 1, 1, 1, 3, 2, 1, 0, 3, 1, 2, 0,
      1, 3, 2, 0, 1, 3, 1, 0, 2, 1, 3, 0, 2, 1, 3, 0, 1, 2, 1, 1, 1, 0, 0,
      2, 2, 0, 0, 1, 1, 0, 1, 0, 2, 0, 2, 0, 1, 1, 0, 0, 1, 2, 0, 0, 2, 2,
      0, 0, 0, 0, 1, 1, -1, 1, 2, 0, 0, 0, 0, 1, -1, 1, 1, 2, 0, 0, 0, 0,
      1, 1, 1, -1, 2, 3, 1, 1, 1, 2, 0, 0, 2, 2, 3, 1, 1, 1, 2, 2, 0, 0,
      2, 3, 1, 1, 1, 2, 0, 2, 0, 2, 1, 1, -1, 1, 2, 0, 0, 2, 2, 1, 1, -1,
      1, 2, 2, 0, 0, 2, 1, -1, 1, 1, 2, 0, 0, 2, 2, 1, -1, 1, 1, 2, 0, 2,
      0, 2, 1, 1, 1, -1, 2, 2, 0, 0, 2, 1, 1, 1, -1, 2, 0, 2, 0
    };

    static const int lookupPairs3D[] = {
      0, 2, 1, 1, 2, 2, 5, 1, 6, 0, 7, 0, 32, 2, 34, 2, 129, 1, 133, 1,
      160, 5, 161, 5, 518, 0, 519, 0, 546, 4, 550, 4, 645, 3, 647, 3, 672, 5,
      673, 5, 674, 4, 677, 3, 678, 4, 679, 3, 680, 13, 681, 13, 682, 12, 685,
      14, 686, 12, 687, 14, 712, 20, 714, 18, 809, 21, 813, 23, 840, 20, 841,
      21, 1198, 19, 1199, 22, 1226, 18, 1230, 19, 1325, 23, 1327, 22, 1352, 15,
      1353, 17, 1354, 15, 1357, 17, 1358, 16, 1359, 16, 1360, 11, 1361, 10, 1362,
      11, 1365, 10, 1366, 9, 1367, 9, 1392, 11, 1394, 11, 1489, 10, 1493, 10, 1520,
      8, 1521, 8, 1878, 9, 1879, 9, 1906, 7, 1910, 7, 2005, 6, 2007, 6, 2032, 8, 2033,
      8, 2034, 7, 2037, 6, 2038, 7, 2039, 6
    };

    // TODO: fix ugly copy-paste!

    class initOpenSimplexNoise_t 
    {
    public:
        initOpenSimplexNoise_t() 
        {

            ptrContrib_t* contributions3D = new ptrContrib_t[countof(p3D) / 9];
            for (size_t i = 0; i < countof(p3D); i += 9)
            {
                auto* baseSet = base3D[p3D[i]];
                auto baseSetSize = base3DSize[p3D[i]];
                ptrContrib_t previous = nullptr;
                ptrContrib_t current = nullptr;

                for (size_t k = 0; k < baseSetSize; k += 4)
                {
                    current = new contrib_t(baseSet[k], baseSet[k + 1], baseSet[k + 2], baseSet[k + 3]);
                    if (previous == nullptr) {
                        contributions3D[i / 9] = current;
                    }
                    else
                    {
                        previous->Next = current;
                    }
                    previous = current;
                }
                current->Next = new contrib_t(p3D[i + 1], p3D[i + 2], p3D[i + 3], p3D[i + 4]);
                current->Next->Next = new contrib_t(p3D[i + 5], p3D[i + 6], p3D[i + 7], p3D[i + 8]);
            }

            lookup3D.reset(new ptrContrib_t[2048]);
            for (size_t i = 0; i < countof(lookupPairs3D); i += 2)
            {
                lookup3D[lookupPairs3D[i]] = contributions3D[lookupPairs3D[i + 1]];
            }
        }
    } initOpenSimplexNoise;
}

namespace bb
{

  simplex_t::simplex_t(long seed) 
  :perm(new uint8_t[256]),
   perm3D(new uint8_t[256])
  {
      std::unique_ptr<uint8_t[]> source(new uint8_t[256]);
      for (int i = 0; i < 256; i++)
      {
        source[i] = i;
      }

      seed = seed * 6364136223846793005L + 1442695040888963407L;
      seed = seed * 6364136223846793005L + 1442695040888963407L;
      seed = seed * 6364136223846793005L + 1442695040888963407L;

      for (int i = 255; i >= 0; i--)
      {
          seed = seed * 6364136223846793005L + 1442695040888963407L;
          int r = (int)((seed + 31) % (i + 1));
          if (r < 0)
          {
              r += (i + 1);
          }
          perm[i] = source[r];
          perm3D[i] = (uint8_t)((perm[i] % 24) * 3);
          source[r] = source[i];
      }
  }

  double simplex_t::operator()(double x, double y, double z)
  {
      auto stretchOffset = (x + y + z) * STRETCH_3D;
      auto xs = x + stretchOffset;
      auto ys = y + stretchOffset;
      auto zs = z + stretchOffset;

      auto xsb = FastFloor(xs);
      auto ysb = FastFloor(ys);
      auto zsb = FastFloor(zs);

      auto squishOffset = (xsb + ysb + zsb) * SQUISH_3D;
      auto dx0 = x - (xsb + squishOffset);
      auto dy0 = y - (ysb + squishOffset);
      auto dz0 = z - (zsb + squishOffset);

      auto xins = xs - xsb;
      auto yins = ys - ysb;
      auto zins = zs - zsb;

      auto inSum = xins + yins + zins;

      auto hash =
          (int)(yins - zins + 1) |
          (int)(xins - yins + 1) << 1 |
          (int)(xins - zins + 1) << 2 |
          (int)inSum << 3 |
          (int)(inSum + zins) << 5 |
          (int)(inSum + yins) << 7 |
          (int)(inSum + xins) << 9;

      auto c = lookup3D[hash];

      auto value = 0.0;
      while (c != nullptr)
      {
          auto dx = dx0 + c->dx;
          auto dy = dy0 + c->dy;
          auto dz = dz0 + c->dz;
          auto attn = 2 - dx * dx - dy * dy - dz * dz;

          if (attn > 0)
          {
              auto px = xsb + c->xsb;
              auto py = ysb + c->ysb;
              auto pz = zsb + c->zsb;

              auto i = perm3D[(perm[(perm[px & 0xFF] + py) & 0xFF] + pz) & 0xFF];
              auto valuePart = gradients3D[i] * dx + gradients3D[i + 1] * dy + gradients3D[i + 2] * dz;

              attn *= attn;
              value += attn * attn * valuePart;
          }

          c = c->Next;
      }
      return value * NORM_3D;
  }

  simplex_t::simplex_t(const simplex_t& cp)
  :perm(new uint8_t[256]),
   perm3D(new uint8_t[256])
  {
      memcpy(this->perm.get(), cp.perm.get(), sizeof(uint8_t)*256);
      memcpy(this->perm3D.get(), cp.perm3D.get(), sizeof(uint8_t)*256);
  }

  simplex_t::simplex_t(simplex_t&& mv)
  :perm(std::move(mv.perm)),
   perm3D(std::move(mv.perm3D))
  {
    ;
  }

  simplex_t& simplex_t::operator=(const simplex_t& cp)
  {
      assert((this->perm) && (this->perm3D));
      if (this != &cp)
      {   // old pointer exists
          memcpy(this->perm.get(), cp.perm.get(), sizeof(uint8_t)*256);
          memcpy(this->perm3D.get(), cp.perm3D.get(), sizeof(uint8_t)*256);
      }
      return *this;
  }

  simplex_t& simplex_t::operator=(simplex_t&& mv)
  {
      if (this != &mv)
      {
          this->perm = std::move(mv.perm);
          this->perm3D = std::move(mv.perm3D);
      }
      return *this;
  }

}