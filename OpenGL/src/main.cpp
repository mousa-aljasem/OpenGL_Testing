#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};



static ShaderProgramSource parseShader(const std::string& filepath);
static unsigned int CompileShader(unsigned int type, const std::string& source);
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
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

    glfwSwapInterval(1);

    // Has to be after glfwMakeContextCurrent, glewInit() requires a valid context
    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR!" << std::endl;
    }

    // For debugging purposes, will output the OpenGL version as well as GPU driver version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    {
        float positions[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f,
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Required for modern OpenGL
        unsigned int VAO; // Vertex Array Object
        GLCall(glGenVertexArrays(1, &VAO));
        GLCall(glBindVertexArray(VAO));

        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

        IndexBuffer ib(indices, 6);

        ShaderProgramSource source = parseShader("res/shaders/basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        // Looks for the location of the variable "u_Color" stored inside the shader
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1); // Make sure location is actually found

        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            // Input
            processInput(window);

            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.0f, 0.0f, 1.0f));

            GLCall(glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]));
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            GLCall(glBindVertexArray(VAO));
            ib.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f) {
                increment = 0.05f;
            }
            r += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        GLCall(glDeleteProgram(shader));
    }

    glfwTerminate();
    return 0;
}

static ShaderProgramSource parseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        // First get the length of the error message
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // Not needed anymore, need to be deleted
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void render() {
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    GLCall(glViewport(0, 0, width, height));
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