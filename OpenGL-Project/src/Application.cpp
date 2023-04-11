#include <GLFW/glfw3.h>
#include <fstream>
#include <gl/glew.h>
#include <iostream>
#include <sstream>
#include <string>

#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

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
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
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
    shader.Bind();

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
      /* Render here */
      glClear(GL_COLOR_BUFFER_BIT);

      shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

      // this function call knows what buffer to use because on line 39, we
      // bound the buffer we could clear the buffer by calling
      // glBindBuffer(GL_ARRAY_BUFFER, 0) last param is the number of verticies
      // to draw
      // glDrawArrays(GL_TRIANGLES, 0, 6);

      va.Bind();
      ib.Bind();

      GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
      // we are using nullptr since above we already Bound
      // GL_ELEMENT_ARRAY_BUFFER to ibo

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
