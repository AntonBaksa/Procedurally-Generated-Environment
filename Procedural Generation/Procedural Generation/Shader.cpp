#include "Shader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace
{
    std::string readTextFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file)
        {
            throw std::runtime_error("Could not open file: " + path);
        }

        std::ostringstream contents;
        contents << file.rdbuf();
        return contents.str();
    }

    GLuint compileShader(GLenum type, const std::string& source, const std::string& label)
    {
        const GLuint shader = glCreateShader(type);
        const char* sourcePtr = source.c_str();

        glShaderSource(shader, 1, &sourcePtr, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            std::string log(static_cast<std::size_t>(logLength), '\0');
            // &log[0] is non-const since C++11, unlike log.data() which is
            // only non-const from C++17 onward - this works either way.
            glGetShaderInfoLog(shader, logLength, nullptr, &log[0]);

            glDeleteShader(shader);
            throw std::runtime_error("Shader compilation failed (" + label + "):\n" + log);
        }

        return shader;
    }
} // namespace

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
{
    const std::string vertexSource = readTextFile(vertexPath);
    const std::string fragmentSource = readTextFile(fragmentPath);

    const GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
    const GLuint fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, &log[0]);

        glDeleteProgram(program);
        throw std::runtime_error("Shader program link failed:\n" + log);
    }

    return program;
}