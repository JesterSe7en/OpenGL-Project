#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"


struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath) {
    // we need to parse and find our special syntax e.g. (#shader vertex) 
    
    std::ifstream stream(filePath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                // set mode to vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                // set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

    return {
        ss[0].str(), ss[1].str()
    };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);

    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    // error handling
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

        // _malloca still allocated on stack but more secure - https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/malloca?view=msvc-170
        char* message = (char*)_malloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;

        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}


// Shaders are just strings of code, we are passing it to OpenGL to compile and create a program object
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    
    // We are using this "program" to combine these two shaders
    unsigned int program = glCreateProgram();       // returns a pointer to the blank program object
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

int main(void)
{
    // use docs.gl for documentation
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // by default this is running in compatibility mode
    // to demonstrate VAO (vertex array objects), we need to set this to core mode
    // VAO - https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
    // glfwWindowHint - https://www.glfw.org/docs/3.3/group__window.html#ga7d9c8c62384b1e2821c4dc48952d2033
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // turn on vsync
    glfwSwapInterval(1);

    // From docs: "you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points"
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
        -0.5f, -0.5f,   //0
        0.5f, -0.5f,    //1
        0.5f, 0.5f,     //2
        -0.5f, 0.5f,    //3
        };

        unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0
        };


        // Vertex Array is the encapulation of the vertex buffer and a layout (or attribute array)
        VertexArray va;
        VertexBuffer vb(position, 4 * 2 * sizeof(float));
        IndexBuffer ib(indicies, 6);


        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);


        ShaderProgramSource src = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(src.VertexSource, src.FragmentSource);
        GLCall(glUseProgram(shader));


        // In order to set the uniform, a program (aka a shader) must be bound
        // i.e. glUniform must be called after glUseProgram
        // u_Color is defined in our basic.shader.  This variable name (including case) must match
        // Uniform allows us to define values in C++ and pass it to our shader program
        // Uniforms are used as a per frame thing
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1);

        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

            // this function call knows what buffer to use because on line 39, we bound the buffer
            // we could clear the buffer by calling glBindBuffer(GL_ARRAY_BUFFER, 0)
            // last param is the number of verticies to draw
            //glDrawArrays(GL_TRIANGLES, 0, 6);
            va.Bind();
            ib.Bind();
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));  // we are using nullptr since above we already Bound GL_ELEMENT_ARRAY_BUFFER to ibo

            if (r > 1.0f) {
                increment = -0.05f;
            }
            else if (r < 0.0f) {
                increment = 0.05f;
            }

            r += increment;

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }

        GLCall(glDeleteProgram(shader));
    }

    glfwTerminate();
    return 0;
}