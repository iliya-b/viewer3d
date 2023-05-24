#ifndef SHADER_H_
#define SHADER_H_

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;

    Shader(const char *vertexPath, const char *fragmentPath);
    void use() const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    bool inited = false;

private:
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif