#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

float backgroundColour[] = {
    0.2f, 0.3f, 0.3f, 1.0f
};

GLFWwindow* InitWindow() {
    /* Initialize the library */
    if (!glfwInit())
        return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);

    if (window == NULL) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    // Has to be after glfwMakeContextCurrent, glewInit() requires a valid context
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW!" << std::endl;
    }

    // For debugging purposes, will output the OpenGL version as well as GPU driver version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    return window;
}


int main(void) {
    GLFWwindow* window = InitWindow();
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(1);

    {
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f,
             50.0f, -50.0f, 1.0f, 0.0f,
             50.0f,  50.0f, 1.0f, 1.0f,
            -50.0f,  50.0f, 0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Enable blending and display transparent pixels correctly
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));
        IndexBuffer ib(indices, 6);

        VertexBufferLayout layout;
        layout.Push<float>(2); // Square object
        layout.Push<float>(2); // Square Texture
        va.AddBuffer(vb, layout);

        // Orthographic projection
        // Give values that equate to the 4:3 aspect ration that the window is on
        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

        // Camera movement
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        Shader shader("res/shaders/basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));

        //Texture texture("res/textures/googleAssistant.png");
        Texture texture("res/textures/testPic.jpg");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);
        glm::vec4 cubeColorA(0.8f, 0.3f, 0.8f, 1.0f);
        glm::vec4 cubeColorB(0.8f, 0.3f, 0.8f, 1.0f);
        bool useTextureA = false;
        bool useTextureB = true;

        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            // Input
            processInput(window);
            
            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                shader.SetUniform4f("u_Color", cubeColorA);
                shader.SetUniform1b("u_UseTexture", useTextureA);
                renderer.Draw(va, ib, shader);
            }
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                shader.SetUniform4f("u_Color", cubeColorB);
                shader.SetUniform1b("u_UseTexture", useTextureB);
                renderer.Draw(va, ib, shader);
            }

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            { // ImGui Settings
                ImGui::Checkbox("Use Texture A", &useTextureA);
                ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
                ImGui::ColorEdit4("change color A", (float*)&cubeColorA);
                ImGui::Checkbox("Use Texture B", &useTextureB);
                ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);
                ImGui::ColorEdit4("change color B", (float*)&cubeColorB);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
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

    GLCall(glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]));
}
