#include "shader.h"

#include "PVRUtils/OpenGLES/BindingsGles.h"

#include <iostream>

Shader &Shader::Use()
{
    gl::UseProgram(this->ID);
    return *this;
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    unsigned int sVertex, sFragment, gShader = 0;
    // vertex Shader
    sVertex = gl::CreateShader(GL_VERTEX_SHADER);
    gl::ShaderSource(sVertex, 1, &vertexSource, NULL);
    gl::CompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    // fragment Shader
    sFragment = gl::CreateShader(GL_FRAGMENT_SHADER);
    gl::ShaderSource(sFragment, 1, &fragmentSource, NULL);
    gl::CompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    // if geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr)
    {
        gShader = gl::CreateShader(GL_GEOMETRY_SHADER);
        gl::ShaderSource(gShader, 1, &geometrySource, NULL);
        gl::CompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY");
    }
    // shader program
    this->ID = gl::CreateProgram();
    gl::AttachShader(this->ID, sVertex);
    gl::AttachShader(this->ID, sFragment);
    if (geometrySource != nullptr)
        gl::AttachShader(this->ID, gShader);
    gl::LinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    gl::DeleteShader(sVertex);
    gl::DeleteShader(sFragment);
    if (geometrySource != nullptr)
        gl::DeleteShader(gShader);
}

void Shader::SetFloat(const char *name, float value, bool useShader)
{
    if (useShader)
        this->Use();
    gl::Uniform1f(gl::GetUniformLocation(this->ID, name), value);
}
void Shader::SetInteger(const char *name, int value, bool useShader)
{
    if (useShader)
        this->Use();
    gl::Uniform1i(gl::GetUniformLocation(this->ID, name), value);
}
void Shader::SetVector2f(const char *name, float x, float y, bool useShader)
{
    if (useShader)
        this->Use();
    gl::Uniform2f(gl::GetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVector2f(const char *name, const glm::vec2 &value, bool useShader)
{
    if (useShader)
        this->Use();
    gl::Uniform2f(gl::GetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVector3f(const char *name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->Use();
    gl::Uniform3f(gl::GetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVector3f(const char *name, const glm::vec3 &value, bool useShader)
{
    if (useShader)
        this->Use();
    gl::Uniform3f(gl::GetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const char *name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->Use();
    gl::Uniform4f(gl::GetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVector4f(const char *name, const glm::vec4 &value, bool useShader)
{
    if (useShader)
        this->Use();
    gl::Uniform4f(gl::GetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix, bool useShader)
{
    if (useShader)
        this->Use();
    gl::UniformMatrix4fv(gl::GetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}


void Shader::checkCompileErrors(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        gl::GetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            gl::GetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        gl::GetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            gl::GetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}