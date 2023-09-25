#include "TestTexture2D.h"
#include <Renderer.h>
#include "imgui/imgui.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace test {
	TestTexture2D::TestTexture2D() :
        m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)), 
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
        m_TranslationA(200, 200, 0),
        m_TranslationB(400, 200, 0),
        m_CubeColorA(0.8f, 0.3f, 0.8f, 1.0f),
        m_CubeColorB(0.8f, 0.3f, 0.8f, 1.0f),
        m_UseTextureA(false),
        m_UseTextureB(true) {
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

        m_VAO = std::make_unique<VertexArray>();
        m_VB = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2); // Square object
        layout.Push<float>(2); // Square Texture
        m_IBO = std::make_unique<IndexBuffer>(indices, 6);
        m_VAO->AddBuffer(*m_VB, layout);

        m_Shader = std::make_unique<Shader>("res/shaders/basic.shader");
        m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));

        //Texture texture("res/textures/googleAssistant.png");
        m_Texture = std::make_unique<Texture>("res/textures/testPic.jpg");
        m_Shader->SetUniform1i("u_Texture", 0);
	}
	TestTexture2D::~TestTexture2D() { }

	void TestTexture2D::OnUpdate(float deltaTime) { }

	void TestTexture2D::OnRender() {
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;
        m_Texture->Bind();


        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            m_Shader->SetUniform4f("u_Color", m_CubeColorA);
            m_Shader->SetUniform1b("u_UseTexture", m_UseTextureA);
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            m_Shader->SetUniform4f("u_Color", m_CubeColorB);
            m_Shader->SetUniform1b("u_UseTexture", m_UseTextureB);
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }
	}

	void TestTexture2D::OnImGuiRender() {
        ImGui::Checkbox("Use Texture A", &m_UseTextureA);
        ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 960.0f);
        ImGui::ColorEdit4("change color A", (float*)&m_CubeColorA);
        ImGui::Checkbox("Use Texture B", &m_UseTextureB);
        ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 960.0f);
        ImGui::ColorEdit4("change color B", (float*)&m_CubeColorB);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}