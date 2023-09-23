#include "Shader.h"

#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <sstream>



Shader::Shader(const std::string& filepath) : m_FilePath(filepath), m_RendererID(0) {

    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(m_RendererID));
}

Shader::~Shader() {
    GLCall(glDeleteProgram(m_RendererID));
}

enum class ShaderType {
    NONE = -1, VERTEX = 0, FRAGMENT = 1
};

struct ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

    struct ShaderProgramSource sps = { ss[0].str(), ss[1].str() };
    return sps;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // Error checking
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    std::cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader compile status: " << result << std::endl;

    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout 
            << "Failed to compile " 
            << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
            << " shader!" 
            << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));

    GLint program_linked;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &program_linked));
    std::cout << "Program link status: " << program_linked << std::endl;

    if (program_linked != GL_TRUE) {
        GLsizei log_length = 0;
        GLchar message[1024];
        GLCall(glGetProgramInfoLog(program, 1024, &log_length, message));
        std::cout << "Failed to link program" << std::endl;
        std::cout << message << std::endl;
    }

    GLCall(glValidateProgram(program));

    // Not needed anymore, need to be deleted
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

void Shader::Bind() const {
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const {
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1f(const std::string& name, float value) {
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    
    m_UniformLocationCache[name] = location;
    return location;
}
