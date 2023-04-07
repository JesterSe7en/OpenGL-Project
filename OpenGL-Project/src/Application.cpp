#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

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
    glfwMakeContextCurrent(window);


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


    // This is to enable the attribute in the array.  Otherwise the attribute will do nothing
    glEnableVertexAttribArray(0);

    // Vertex is a blob of data
    // Each vertex can be comprised of multiple attributes e.g. position, texture coordinates, color, etc.
    // We are using VertextAttribPointer to describe our bound buffer attributes via an indexing system
    // 5th param = stride = the number of bytes the pointer needs to go to find the next VERTEX aka next blob (NOT next attribute)
    // 6th param = the pointer to the next attribute (in this case we only have one attribute so it defaults to zero
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);



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