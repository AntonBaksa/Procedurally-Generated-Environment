#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


namespace
{
    constexpr int WindowWidth = 900;
    constexpr int WindowHeight = 600;

    void glfwErrorCallback(int error, const char* description)
    {
        std::cerr << "GLFW error (" << error << "): " << description << '\n';
    }

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
            //glGetShaderInfoLog(shader, logLength, nullptr, log.data());

            glDeleteShader(shader);
            throw std::runtime_error("Shader compilation failed (" + label + "):\n" + log);
        }

        return shader;
    }

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
            //glGetProgramInfoLog(program, logLength, nullptr, log.data());

            glDeleteProgram(program);
            throw std::runtime_error("Shader program link failed:\n" + log);
        }

        return program;
    }

    void framebufferSizeCallback(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
} // namespace

int main()
{
    glfwSetErrorCallback(glfwErrorCallback);
    // Need to initialize GLFW before calling any GLFW functions
    if (glfwInit() != GLFW_TRUE)
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    GLFWwindow* window =
        glfwCreateWindow(WindowWidth, WindowHeight, "3D and Shader Programming", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cerr << "Failed to create a GLFW window.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSwapInterval(1);

    const int loadedVersion = gladLoadGL(glfwGetProcAddress);
    if (loadedVersion == 0)
    {
        std::cerr << "Failed to load OpenGL functions with GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';

    glEnable(GL_DEPTH_TEST);

    GLuint vao = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    constexpr GLsizei stride = 6 * sizeof(float);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    GLuint shaderProgram = 0;

    try
    {
        shaderProgram =
            createShaderProgram("shaders/basic.vert", "shaders/basic.frag");
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << '\n';
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // Uniform locations identify the three matrix inputs in the vertex shader.
    // We ask for them once after linking, then use the locations when sending
    // matrix values from the CPU to the GPU before drawing.
    const GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
    const GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
    const GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    const GLint normalMatrixLocation = glGetUniformLocation(shaderProgram, "normalMatrix");
    const GLint lightDirectionLocation = glGetUniformLocation(shaderProgram, "lightDirection");
    const GLint lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor");
    const GLint viewPositionLocation = glGetUniformLocation(shaderProgram, "viewPosition");
    const GLint baseColorLocation = glGetUniformLocation(shaderProgram, "baseColor");
    const GLint ambientStrengthLocation = glGetUniformLocation(shaderProgram, "ambientStrength");
    const GLint specularStrengthLocation = glGetUniformLocation(shaderProgram, "specularStrength");
    const GLint shininessLocation = glGetUniformLocation(shaderProgram, "shininess");

    if (modelLocation == -1 ||
        viewLocation == -1 ||
        projectionLocation == -1)
    {
        std::cerr
            << "Note: one or more matrix uniforms are inactive. "
            << "This is expected if the current shader experiment does not use them.\n";
    }

    glm::mat4 model(1.0f);

    // Positions and normals transform differently. The inverse-transpose keeps
    // normals perpendicular to their surfaces, including under non-uniform scale.
    const glm::mat3 normalMatrix =
        glm::transpose(glm::inverse(glm::mat3(model)));

    // The view matrix converts world-space positions into view space. Moving the
    // world by the negative viewer position places the cube in front of the
    // viewer without introducing a camera class or camera controls.
    const glm::vec3 viewPosition(0.0f, 0.0f, 3.0f);
    const glm::mat4 view =
        glm::translate(glm::mat4(1.0f), -viewPosition);

    // This direction points from the surface toward the light. It is not axis-
    // aligned, so more than one visible face receives diffuse illumination.
    const glm::vec3 lightDirection =
        glm::normalize(glm::vec3(0.6f, 1.0f, 0.8f));
    const glm::vec3 lightColor(1.0f, 0.96f, 0.90f);
    const glm::vec3 baseColor(0.18f, 0.48f, 0.82f);
    const float ambientStrength = 0.12f;
    const float specularStrength = 0.28f;
    const float shininess = 32.0f;

    // These values define the perspective viewing volume. Keeping them named and
    // visible makes it easy to ask: what changes when the field of view narrows,
    // or when the near and far clipping planes move?
    const float fieldOfView = glm::radians(45.0f);
    const float nearPlane = 0.1f;
    const float farPlane = 100.0f;

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        processInput(window);

        // Framebuffer dimensions can differ from window dimensions on high-DPI
        // displays. Reading the current framebuffer size keeps projected shapes
        // in the correct proportions after a resize. A minimized window may have
        // no drawable area, so wait for events instead of dividing by zero.
        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        if (framebufferWidth == 0 || framebufferHeight == 0)
        {
            glfwPollEvents();
            continue;
        }

        const float aspectRatio =
            static_cast<float>(framebufferWidth) /
            static_cast<float>(framebufferHeight);
        const glm::mat4 projection =
            glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);

        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // glm::value_ptr exposes each GLM matrix as contiguous float data.
        // GL_FALSE means OpenGL should use the conventional GLM/OpenGL matrix
        // layout directly, without transposing it during the upload.
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(
            projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix3fv(
            normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3fv(lightDirectionLocation, 1, glm::value_ptr(lightDirection));
        glUniform3fv(lightColorLocation, 1, glm::value_ptr(lightColor));
        glUniform3fv(viewPositionLocation, 1, glm::value_ptr(viewPosition));
        glUniform3fv(baseColorLocation, 1, glm::value_ptr(baseColor));
        glUniform1f(ambientStrengthLocation, ambientStrength);
        glUniform1f(specularStrengthLocation, specularStrength);
        glUniform1f(shininessLocation, shininess);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}