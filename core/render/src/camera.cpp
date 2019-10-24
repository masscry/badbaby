#include <utility>

#include <camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace bb
{

  void Update();

  camera_t::camera_t(camera_t&& mv)
  :ubo(std::move(mv.ubo))
  {
    ;
  }

  camera_t& camera_t::operator =(camera_t&& mv)
  {
    if (this == &mv)
    {
      return *this;
    }
    this->ubo = std::move(mv.ubo);
    this->data = std::move(mv.data);
    return *this;
  }

  camera_t::camera_t(mat4_t&& proj, mat4_t&& view, uniformBlock_t&& ubo)
  :ubo(std::move(ubo))
  {
    this->data.proj = std::move(proj);
    this->data.view = std::move(view);
  }

  camera_t::~camera_t()
  {
    ;
  }

  void camera_t::Update()
  {
    this->ubo.UpdateData(&this->data, 0, sizeof(data_t));
  }

  camera_t camera_t::Perspective(float fov, float aspect, float nearZ, float farZ)
  {
    camera_t result(
      glm::perspective(fov, aspect, nearZ, farZ),
      glm::mat4(1.0f),
      uniformBlock_t::CreateUniformBlock(sizeof(data_t))
    );
    result.Update();
    return result;
  }


} // namespace bb
