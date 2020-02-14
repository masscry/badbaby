/**
 * @file meshDesc.hpp
 * 
 * Mesh description.
 * 
 */

#pragma once
#ifndef __BB_CORE_UTIL_SHAPES_MESH_DESCRIPTOR_HEADER__
#define __BB_CORE_UTIL_SHAPES_MESH_DESCRIPTOR_HEADER__

#include <common.hpp>
#include <camera.hpp>

#include <deque>
#include <vector>
#include <memory>
#include <cassert>

namespace bb
{

  class basicVertexBuffer_t
  {
  public:
    virtual int Append(const basicVertexBuffer_t& src);
    virtual size_t Size() const = 0;
    virtual GLint Dimensions() const = 0;
    virtual GLenum Type() const = 0;
    virtual GLboolean Normalized() const = 0;
    virtual void* Data() const;
    virtual ~basicVertexBuffer_t() = 0;
  };

  template<typename data_t>
  class vertexBuffer_t final: public basicVertexBuffer_t
  {
    using array_t = std::vector<data_t>;

    array_t self;
    GLboolean isNormalized;

  public:

    vertexBuffer_t(const vertexBuffer_t<data_t>&) = default;
    vertexBuffer_t& operator=(const vertexBuffer_t<data_t>&) = default;

    vertexBuffer_t(vertexBuffer_t<data_t>&&) = default;
    vertexBuffer_t& operator=(vertexBuffer_t<data_t>&&) = default;

    vertexBuffer_t();
    ~vertexBuffer_t() override;

    array_t& Self();
    const array_t& Self() const;

    void SetNormalized(GLboolean value);

    int Append(const basicVertexBuffer_t& src) override;
    size_t Size() const override;
    GLint Dimensions() const override;
    GLenum Type() const override;
    GLboolean Normalized() const override;
    void* Data() const override;

  };

  template<typename data_t>
  inline vertexBuffer_t<data_t>::vertexBuffer_t()
  : isNormalized(GL_FALSE)
  {
    ;
  }

  template<typename data_t>
  inline vertexBuffer_t<data_t>::~vertexBuffer_t()
  {
    ;
  }

  template<typename data_t>
  inline typename vertexBuffer_t<data_t>::array_t& vertexBuffer_t<data_t>::Self()
  {
    return this->self;
  }

  template<typename data_t>
  inline const typename vertexBuffer_t<data_t>::array_t& vertexBuffer_t<data_t>::Self() const
  {
    return this->self;
  }

  template<typename data_t>
  size_t vertexBuffer_t<data_t>::Size() const
  {
    return this->self.size();
  }

  template<typename data_t>
  inline GLint vertexBuffer_t<data_t>::Dimensions() const
  {
    static_assert(std::is_same<typename data_t::value_type, float>::value, "data_t must contain floats");
    return sizeof(data_t)/sizeof(float);
  }

  template<typename data_t>
  inline GLenum vertexBuffer_t<data_t>::Type() const
  {
    return GL_FLOAT;
  }

  template<typename data_t>
  inline GLboolean vertexBuffer_t<data_t>::Normalized() const
  {
    return this->isNormalized;
  }

  template<typename data_t>
  inline void vertexBuffer_t<data_t>::SetNormalized(GLboolean value)
  {
    this->isNormalized = value;
  }

  template<typename data_t>
  inline void* vertexBuffer_t<data_t>::Data() const
  {
    return &this->self.data();
  }

  template<typename data_t>
  int vertexBuffer_t<data_t>::Append(const basicVertexBuffer_t& src)
  {
    if (this == &src)
    {
      // Programmer's error!
      bb::Error("Can't append buffer to itself");
      assert(0);
      return -1;
    }

    if (
         (src.Dimensions() != this->Dimensions())
      || (src.Type() != this->Type())
      || (src.Normalized() != this->Normalized())
    )
    {
      // Programmer's error!
      bb::Error("Trying to append vertex buffer of different kind");
      assert(0);
      return -1;
    }

    auto itemsToAppend = reinterpret_cast<data_t*>(src.Data());
    size_t itemsSize = src.Size();

    this->self.insert(this->self.end(), itemsToAppend, itemsToAppend + itemsSize);
    return 0;
  }

  using arrayOfIndecies_t = std::vector<uint16_t>;
  using arrayOfVertexBuffers_t = std::deque<std::unique_ptr<basicVertexBuffer_t>>;
  using linePoints_t = std::deque<glm::vec2>;

  class meshDesc_t final
  {
    arrayOfVertexBuffers_t buffers;
    arrayOfIndecies_t indecies;
    GLenum drawMode;

    meshDesc_t(const meshDesc_t&) = delete;
    meshDesc_t& operator=(const meshDesc_t&) = delete;

  public:

    arrayOfVertexBuffers_t& Buffers();
   
    arrayOfIndecies_t& Indecies();

    const arrayOfVertexBuffers_t& Buffers() const;
   
    const arrayOfIndecies_t& Indecies() const;

    GLenum DrawMode() const;

    void SetDrawMode(GLenum drawMode);

    uint16_t MaxIndex() const;

    bool IsGood() const;

    meshDesc_t();
    ~meshDesc_t();

    meshDesc_t(meshDesc_t&&) = default;
    meshDesc_t& operator=(meshDesc_t&&) = default;

    int Append(const meshDesc_t& mesh);

  };

  inline arrayOfVertexBuffers_t& meshDesc_t::Buffers()
  {
    return this->buffers;
  }
   
  inline const arrayOfVertexBuffers_t& meshDesc_t::Buffers() const
  {
    return this->buffers;
  }
   
  inline arrayOfIndecies_t& meshDesc_t::Indecies()
  {
    return this->indecies;
  }

  inline const arrayOfIndecies_t& meshDesc_t::Indecies() const
  {
    return this->indecies;
  }

  inline GLenum meshDesc_t::DrawMode() const
  {
    return this->drawMode;
  }

  inline void meshDesc_t::SetDrawMode(GLenum drawMode)
  {
    this->drawMode = drawMode;
  }

  meshDesc_t DefineCircle(glm::vec3 center, uint32_t sides, float radius, float width);
  meshDesc_t DefineLine(glm::vec3 offset, float width, const linePoints_t& linePoints);

} // namespace bb

#endif /* __BB_CORE_UTIL_SHAPES_MESH_DESCRIPTOR_HEADER__ */
