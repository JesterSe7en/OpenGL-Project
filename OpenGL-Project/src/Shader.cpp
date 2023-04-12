#include "Shader.h"
#include "Renderer.h"
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const std::string &filePath)
    : m_filePath(filePath), m_RendererID(0) {
  ShaderProgramSource src = ParseShader(filePath);
  m_RendererID = CreateShader(src.VertexSource, src.FragmentSource);
}

Shader::~Shader() { GLCall(glDeleteProgram(m_RendererID)); }

void Shader::Bind() const { GLCall(glUseProgram(m_RendererID)); }

void Shader::Unbind() const { GLCall(glUseProgram(0)); }

void Shader::SetUniform1i(const std::string& name, int value) {
  GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2,
                          float v3) {
  GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
  GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string &name) {
  // In order to set the uniform, a program (aka a shader) must be bound
  // i.e. glUniform must be called after glUseProgram
  // u_Color is defined in our basic.shader.  This variable name (including
  // case) must match Uniform allows us to define values in C++ and pass it to
  // our shader program Uniforms are used as a per frame thing

  if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
    return m_UniformLocationCache[name];
  }

  GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
  if (location == -1) {
    std::cout << "Warning: uniform '" << name << "' does not exist!.\n";
  }
  m_UniformLocationCache[name] = location;
  return location;
}

unsigned int Shader::CompileShader(unsigned int type,
                                   const std::string &source) {
  unsigned int id = glCreateShader(type);

  const char *src = source.c_str();
  GLCall(glShaderSource(id, 1, &src, nullptr));
  GLCall(glCompileShader(id));

  int result;
  GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

  // error handling
  if (result == GL_FALSE) {
    int length;
    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

    // _malloca still allocated on stack but more secure
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/malloca?view=msvc-170
    char *message = (char *)_malloca(length * sizeof(char));
    GLCall(glGetShaderInfoLog(id, length, &length, message));

    std::cout << "Failed to compile "
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
              << std::endl;
    std::cout << message << std::endl;

    GLCall(glDeleteShader(id));
    return 0;
  }

  return id;
}

ShaderProgramSource Shader::ParseShader(const std::string &filePath) {
  // we need to parse and find our special syntax e.g. (#shader vertex)

  std::ifstream stream(filePath);

  enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;
  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        // set mode to vertex
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") != std::string::npos) {
        // set mode to fragment
        type = ShaderType::FRAGMENT;
      }
    } else {
      ss[(int)type] << line << '\n';
    }
  }

  return {ss[0].str(), ss[1].str()};
}

// Shaders are just strings of code, we are passing it to OpenGL to compile and
// create a program object
unsigned int Shader::CreateShader(const std::string &vertexShader,
                                  const std::string &fragmentShader) {

  // We are using this "program" to combine these two shaders
  unsigned int program = glCreateProgram();
  // returns a pointer to the blank program object
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  // The follow operates similar to how C++ compiles/links code
  // You first attach shaders to the program objecct
  // Link the program and validate the code is correct
  // Now you have "intermediate" shaders, so lets clean up by deleting them
  GLCall(glAttachShader(program, vs));
  GLCall(glAttachShader(program, fs));
  GLCall(glLinkProgram(program));
  GLCall(glValidateProgram(program));
  GLCall(glDeleteShader(vs));
  GLCall(glDeleteShader(fs));

  return program;
}
