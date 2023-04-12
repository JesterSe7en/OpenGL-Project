#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#define WIN32

#ifdef WIN32
#include <windows.h>
extern "C"
{
  __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif //def WIN32

int main(void) {
  // use docs.gl for documentation
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  // by default this is running in compatibility mode
  // to demonstrate VAO (vertex array objects), we need to set this to core mode
  // VAO -
  // https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
  // glfwWindowHint -
  // https://www.glfw.org/docs/3.3/group__window.html#ga7d9c8c62384b1e2821c4dc48952d2033
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "OpenGL - Project", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // turn on vsync
  glfwSwapInterval(1);

  // From docs: "you need to create a valid OpenGL rendering context and call
  // glewInit() to initialize the extension entry points"
  if (glewInit() != GLEW_OK) {
    std::cout << "Error!\n";
  }

  // just sample code to ensure glew is linked
  std::cout << glGetString(GL_VERSION) << std::endl;

  {
    // VAO - vertex array object
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    float position[] = {
        -0.5f, -0.5f, // 0
        0.5f,  -0.5f, // 1
        0.5f,  0.5f,  // 2
        -0.5f, 0.5f,  // 3
    };

    unsigned int indicies[] = {0, 1, 2, 2, 3, 0};

    // Vertex Array is the encapulation of the vertex buffer and a layout (or
    // attribute array)
    VertexArray va;
    VertexBuffer vb(position, 4 * 2 * sizeof(float));
    IndexBuffer ib(indicies, 6);

    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    Shader shader("res/shaders/Basic.shader");
    Renderer renderer;

    float r = 0.0f;
    float increment = 0.05f;

    while (!glfwWindowShouldClose(window)) {

      renderer.Clear();
      
      shader.Bind();
      shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

      renderer.Draw(va, ib, shader);

      if (r > 1.0f) {
        increment = -0.05f;
      } else if (r < 0.0f) {
        increment = 0.05f;
      }

      r += increment;

      /* Swap front and back buffers */
      GLCall(glfwSwapBuffers(window));

      /* Poll for and process events */
      GLCall(glfwPollEvents());
    }
  }

  glfwTerminate();
  return 0;
}
