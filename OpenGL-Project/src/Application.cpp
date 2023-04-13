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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tests/TestClearColor.h"

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
  const char* glsl_version = "#version 130";

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(960, 540, "OpenGL - Project", NULL, NULL);
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


    GLCall(glEnable(GL_BLEND));
    // source is whatever is in our fragment shader (in the case of the texture example - it is the png itself)
    // the destination is the is what is currently painted in the frame buffer - i.e. what is painted on the screen now
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    Renderer renderer;

    test::TestClearColor test;


    while (!glfwWindowShouldClose(window)) {

      renderer.Clear();

      test.OnUpdate(0.0f);
      test.OnRender();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      
      test.OnImGuiRender();

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


      /* Swap front and back buffers */
      GLCall(glfwSwapBuffers(window));

      /* Poll for and process events */
      GLCall(glfwPollEvents());
    }
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
