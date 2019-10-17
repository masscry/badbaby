#include <string>
#include <stdexcept>

#include <shader.hpp>

namespace 
{

  GLuint LoadShader(GLenum shaderType, const char* shaderText) {
    GLuint result = glCreateShader(shaderType);
    glShaderSource(result, 1, &shaderText, nullptr);
    glCompileShader(result);

    GLint compileResult = GL_FALSE;
    GLint compileInfoLen = 0;

    glGetShaderiv(result, GL_COMPILE_STATUS, &compileResult);
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &compileInfoLen);
    if ( compileInfoLen > 1 ){
      compileInfoLen += 1;
      std::string text(compileInfoLen, '\0');
      glGetShaderInfoLog(result, compileInfoLen, nullptr, &text[0]);
      throw std::runtime_error(std::string("Shader compilation failed: type ") + std::to_string(shaderType) + ": " + text);
    }
    return result;
  }

  GLuint CreateProgram(const char* vshader, const char* fshader) {
    GLuint VertexShader = LoadShader(GL_VERTEX_SHADER, vshader);
    GLuint FragmentShader = LoadShader(GL_FRAGMENT_SHADER, fshader);

    int result = glCreateProgram();
    glAttachShader(result, VertexShader);
    glAttachShader(result, FragmentShader);
    glLinkProgram(result);

    GLint linkResult = GL_FALSE;
    GLint linkInfoLen = 0;

    glGetProgramiv(result, GL_LINK_STATUS, &linkResult);
    glGetProgramiv(result, GL_INFO_LOG_LENGTH, &linkInfoLen);
    if ( linkInfoLen > 1 ){
      linkInfoLen += 1;
      std::string text(linkInfoLen, '\0');
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

  void shader_t::Bind(const shader_t& shader)
  {
    glUseProgram(shader.handle);
  }

  
} // namespace bb
