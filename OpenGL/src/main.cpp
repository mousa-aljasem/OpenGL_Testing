#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

// Simple vertex shader written in GLSL, stored in C string
const char *vertexShaderSource = "";

void render();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


float backgroundColour[] = {
    0.2f, 0.3f, 0.3f, 1.0f
};

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

    // Has to be after glfwMakeContextCurrent, glewInit() requires a valid context
    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR!" << std::endl;
    }

    // For debugging purposes, will output the OpenGL version as well as GPU driver version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Sets the selected function to be called when the window size is changed
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Rendering

        render();

        glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);
        glClear(GL_COLOR_BUFFER_BIT);



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void render() {

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        backgroundColour[0] = 1.0f;
        backgroundColour[1] = 0.0f;
        backgroundColour[2] = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        backgroundColour[0] = 0.0f;
        backgroundColour[1] = 1.0f;
        backgroundColour[2] = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        backgroundColour[0] = 0.0f;
        backgroundColour[1] = 0.0f;
        backgroundColour[2] = 1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        backgroundColour[0] = 1.0f;
        backgroundColour[1] = 1.0f;
        backgroundColour[2] = 0.0f;
    }
    else {
        backgroundColour[0] = 0.2f;
        backgroundColour[1] = 0.3f;
        backgroundColour[2] = 0.3f;
    }
}