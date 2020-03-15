#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include <common.hpp>
#include <shader.hpp>

namespace
{

  GLuint LoadShader(GLenum shaderType, const char* shaderText) {
    auto result = glCreateShader(shaderType);
    glShaderSource(result, 1, &shaderText, nullptr);
    glCompileShader(result);

    GLint compileResult = GL_FALSE;
    GLint compileInfoLen = 0;

    glGetShaderiv(result, GL_COMPILE_STATUS, &compileResult);
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &compileInfoLen);
    if ( compileInfoLen > 1 ){
      compileInfoLen += 1;
      std::string text(static_cast<size_t>(compileInfoLen), '\0');
      glGetShaderInfoLog(result, compileInfoLen, nullptr, &text[0]);
      throw std::runtime_error(std::string("Shader compilation failed: type ") + std::to_string(shaderType) + ": " + text);
    }
    return result;
  }

  GLuint CreateProgram(const char* vshader, const char* fshader) {
    GLuint VertexShader = LoadShader(GL_VERTEX_SHADER, vshader);
    GLuint FragmentShader = LoadShader(GL_FRAGMENT_SHADER, fshader);

    auto result = glCreateProgram();
    glAttachShader(result, VertexShader);
    glAttachShader(result, FragmentShader);
    glLinkProgram(result);

    GLint linkResult = GL_FALSE;
    GLint linkInfoLen = 0;

    glGetProgramiv(result, GL_LINK_STATUS, &linkResult);
    glGetProgramiv(result, GL_INFO_LOG_LENGTH, &linkInfoLen);
    if ( linkInfoLen > 1 ){
      linkInfoLen += 1;
      std::string text(static_cast<size_t>(linkInfoLen), '\0');
      glGetProgramInfoLog(result, linkInfoLen, nullptr, &text[0]);
      throw std::runtime_error(std::string("Program link failed: ") + text);
    }
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    return result;
  }

} // namespace

namespace bb
{

  shader_t::shader_t(shader_t&& move)
  :handle(move.handle)
  {
    move.handle = 0;
  } 

  shader_t& shader_t::operator=(shader_t&& move)
  {
    if (this == &move)
    {
      return *this;
    }

    if (this->handle != 0)
    {
      glDeleteProgram(this->handle);
    }
    this->handle = move.handle;
    move.handle = 0;

    return *this;
  }

  shader_t::shader_t()
  :handle(0)
  {

  }

  shader_t::shader_t(const char* vpSource, const char* fpSource)
  :handle(0)
  {
    this->handle = CreateProgram(vpSource, fpSource);
  }

  shader_t::~shader_t()
  {
    if (this->handle != 0)
    {
      glDeleteProgram(this->handle);
    }
  }

  GLint shader_t::UniformLocation(const char* name) const
  {
    return glGetUniformLocation(this->handle, name);
  }

  GLuint shader_t::UniformBlockIndex(const char* name) const
  {
    return glGetUniformBlockIndex(this->handle, name);
  }

  void shader_t::SetBlock(GLuint blockIndex, const uniformBlock_t& block)
  {
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, block.self);
  }

  void shader_t::SetFloat(GLint loc, float value) const
  {
    glUniform1f(loc, value);
  }

  void shader_t::SetVector2f(GLint loc, GLsizei count, const float* values) const
  {
    glUniform2fv(loc, count, values);
  }

  void shader_t::SetVector3f(GLint loc, GLsizei count, const float* values) const
  {
    glUniform3fv(loc, count, values);
  }
  void shader_t::SetVector4f(GLint loc, GLsizei count, const float* values) const
  {
    glUniform4fv(loc, count, values);
  }

  void shader_t::SetMatrix(GLint loc, const float* matrix) const
  {
    glUniformMatrix4fv(loc, 1, GL_FALSE, matrix);
  }

  void shader_t::SetTexture(GLint loc, int texUnit) const
  {
    glUniform1i(loc, texUnit);
  }

  void shader_t::SetVector2f(GLint loc, const glm::vec2& value) const
  {
    glUniform2f(loc, value.x, value.y);
  }

  void shader_t::SetVector3f(GLint loc, const glm::vec3& value) const
  {
    glUniform3f(loc, value.x, value.y, value.z);
  }

  void shader_t::SetVector4f(GLint loc, const glm::vec4& value) const
  {
    glUniform4f(loc, value.x, value.y, value.z, value.w);
  }

  void shader_t::Bind(const shader_t& shader)
  {
    glUseProgram(shader.handle);
  }

  shader_t shader_t::LoadProgramFromFiles(const char* vpFilename, const char* fpFilename)
  {
    std::ifstream vpFile(vpFilename, std::ios::in);
    std::ifstream fpFile(fpFilename, std::ios::in);

    if (vpFile && fpFile)
    {
      std::stringstream vpText;
      std::stringstream fpText;

      vpText << vpFile.rdbuf();
      fpText << fpFile.rdbuf();

      return shader_t(vpText.str().c_str(), fpText.str().c_str());
    }
    throw std::runtime_error("Can't open shader files");
  }

  
} // namespace bb
