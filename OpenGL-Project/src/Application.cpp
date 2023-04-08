#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <sstream>

// this define only works on MSVC compiler
#define ASSERT(x) if (!(x)) __debugbreak();

#define DEBUG = 1

#ifdef DEBUG
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall())
#else
#define GLCall(x) x
#endif

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")\n";
        return false;
    }
    return true;
}

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

        char* message = (char*)alloca(length * sizeof(char));
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    GLCall(glfwMakeContextCurrent(window));


    // From docs: "you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points"
    if (glewInit() != GLEW_OK) {
        std::cout << "Error!\n";
    }

    // just sample code to ensure glew is linked
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    
    // this is a basic vertex buffer
    // param #1 = number of buffer objects names to generate
    // param #2  = specifies the array in which the generated object names are stored
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));

    // this is like "selecting the buffer" in opengl this is called binding
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));

    // specify the data of our buffer
    float position[] = {
        -0.5f, -0.5f,   //0
        0.5f, -0.5f,    //1
        0.5f, 0.5f,     //2
        -0.5f, 0.5f,    //3
    };

    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), position, GL_STATIC_DRAW));


    // This is to enable the attribute in the array.  Otherwise the attribute will do nothing
    GLCall(glEnableVertexAttribArray(0));

    // Vertex is a blob of data
    // Each vertex can be comprised of multiple attributes e.g. position, texture coordinates, color, etc.
    // We are using VertextAttribPointer to describe our bound buffer attributes via an indexing system
    // 5th param = stride = the number of bytes the pointer needs to go to find the next VERTEX aka next blob (NOT next attribute)
    // 6th param = the pointer to the next attribute (in this case we only have one attribute so it defaults to zero
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    // Index Buffer - this MUST be unsigned int
	unsigned int indicies[] = {
	0, 1, 2,
	2, 3, 0
	};

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW));


    ShaderProgramSource src = ParseShader("res/shaders/Basic.shader");
    //std::cout << "Vertex\n";
    //std::cout << src.VertexSource << std::endl;
    //std::cout << "Fragment\n";
    //std::cout << src.FragmentSource << std::endl;

    unsigned int shader = CreateShader(src.VertexSource, src.FragmentSource);
    GLCall(glUseProgram(shader));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        // this function call knows what buffer to use because on line 39, we bound the buffer
        // we could clear the buffer by calling glBindBuffer(GL_ARRAY_BUFFER, 0)
        // last param is the number of verticies to draw
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));  // we are using nullptr since above we already Bound GL_ELEMENT_ARRAY_BUFFER to ibo

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}