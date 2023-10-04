#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>

class Shader
{
    public:
        const char* vertexShaderSource;
        const char* fragmentShaderSource;
        unsigned int fragmentShader,vertexShader;
        std::string fragmentShader_,vertexShader_;
        unsigned int shaderProgram;

        Shader(const std::string arg1, const std::string arg2);
        void use();
        void parser(const std::string& location);
        void shaderCompileResult(unsigned int);
        //set uniform function
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
};

#endif /* SHADER_H */
