#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

int main(void)
{
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
    glfwMakeContextCurrent(window);


    // From docs: "you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points"
    if (glewInit() != GLEW_OK) {
        std::cout << "Error!\n";
    }

    // just sample code to ensure glew is linked
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    
    // this is a basic vertex buffer (1 vertex length = 4 bytes?)
    unsigned int buffer;
    glGenBuffers(1, &buffer);

    // this is like "selecting the buffer" in opengl this is called binding
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    // specify the data of our buffer
    float position[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f,
    };
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //glBegin(GL_TRIANGLES);
        //glVertex2f(-0.5f, -0.5f);
        //glVertex2f(0.0f, 0.5f);
        //glVertex2f(0.5f, -0.5f);
        //glEnd();

        // this function call knows what buffer to use because on line 39, we bound the buffer
        // we could clear the buffer by calling glBindBuffer(GL_ARRAY_BUFFER, 0)
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}