#include <utility>

#include <camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace bb
{

  camera_t::camera_t(camera_t&& mv) noexcept
  : data(mv.data),
    ubo(std::move(mv.ubo))
  {
    ;
  }

  camera_t& camera_t::operator =(camera_t&& mv)  noexcept
  {
    if (this == &mv)
    {
      return *this;
    }
    this->ubo = std::move(mv.ubo);
    this->data = mv.data;
    return *this;
  }

  camera_t::camera_t(const camera_t& cpy)
  : data(cpy.data)
  {
    ;
  }

  camera_t& camera_t::operator =(const camera_t& cpy)
  {
    if (this == &cpy)
    {
      return *this;
    }

    this->data = cpy.data;
    return *this;
  }

  camera_t::camera_t(const mat4_t& proj, const mat4_t& view)
  {
    this->data.proj = proj;
    this->data.view = view;
  }

  camera_t::camera_t()
  {
    this->data.proj = glm::mat4(1.0);
    this->data.view = glm::mat4(1.0);
  }

  camera_t::~camera_t()
  {
    ;
  }

  void camera_t::Update() const
  {
    this->ubo.UpdateData(&this->data, 0, sizeof(camera_t::data_t));
  }

  camera_t camera_t::Perspective(float fov, float aspect, float nearZ, float farZ)
  {
    camera_t result(
      glm::perspective(fov, aspect, nearZ, farZ),
      glm::mat4(1.0f)
    );
    return result;
  }

  camera_t camera_t::Orthogonal(float left, float right, float bottom, float top)
  {
    camera_t result(
      glm::ortho(left, right, bottom, top, -1000.0f, 1000.0f),
      glm::mat4(1.0f)
    );
    return result;
  }

} // namespace bb
