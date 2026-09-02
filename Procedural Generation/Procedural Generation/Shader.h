#pragma once

#include <glad/gl.h>
#include <string>

// Reads a vertex and fragment shader from disk, compiles both, links them
// into a program, and returns the program's ID. Throws std::runtime_error
// with a descriptive message if the file can't be read, either shader fails
// to compile, or the program fails to link.
GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
