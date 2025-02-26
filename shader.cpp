#include "shader.h"


/*
* @brief Sets the current shader program
*
*/
void Shader::use()
{   
    glUseProgram(shaderProgram);
}

/*
*   @brief parses and Compiles the vertex and fragment shaders. 
*
*   @param arg1 location of the vertex shader
*   @param arg2 location of the second shader
*/
Shader::Shader(const std::string arg1, const std::string arg2)
{
    //arg1, arg2 -> Shader Source Code 
    parser(arg1);
    parser(arg2);
    // Vertex Shader and fragment shader Instantiation -- 
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();
    // -- GLSL source code --
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    shaderCompileResult(fragmentShader);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    shaderCompileResult(vertexShader);
    //Attaching and linking the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Checking of the GLSL Shader source code compiled properly
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
        infoLog << std::endl;
    }
    glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
		printf("Error validating program: '%s'\n", infoLog);
	}
   
    //Next delete the shader item  objects since we do not need them anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

/*
* @brief The result of the shader compilation. Generates an info log if something goes wrong. 
*
*   @param theShader ID of the shader program
*
*/
void Shader::shaderCompileResult(unsigned int theShader)
{
    GLint result = 0;
	GLchar eLog[1024] = {0};

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, 1024, NULL, eLog);
		fprintf(stderr, "Error compiling the shader: '%s'\n", eLog);
		return;
	}
}

/*
*   @brief Parses/Serialises a shader program(vertex and fragment shaders)
*
*   @param location location of the shaders, relative to the directory where the program is being executed.
*/
void Shader::parser(const std::string& location)
{
    
    std::ifstream input  (location, std::ifstream::in);

    if(!(input.is_open()))
    {
        std::cout << location << " -- Didn't open the input file properly, exiting now\n";
        return;
    }

    std::string line, program; 
    while(input) // parsing vector shader into a single single
    {
        line = "";
        getline(input, line);
        program.append(line+"\n");
    }
    program.append("\0");
    
    input.close();

    if( location == "vertexShader.glsl")
    {
        vertexShader_ = program; //copy data into seperate buffer because the file stream data only exits while the file is open and gets deleted when the file closes so that char* pointer points to nothing
        vertexShaderSource = &vertexShader_[0];
    }
    else 
    {
        fragmentShader_  = program;
        fragmentShaderSource = &fragmentShader_[0];
    }
}

/**
*   @brief The follow are setters for the uniforms in the shader program.
*
*
*/
void Shader::setBool(const std::string &name, bool value) const{
    glUniform1i( glGetUniformLocation(shaderProgram, name.c_str()),(int)value);
}
void Shader::setInt(const std::string &name, int value) const{
    glUniform1i( glGetUniformLocation(shaderProgram, name.c_str()),value);
}
void Shader::setFloat(const std::string &name, float value) const{
    glUniform1i( glGetUniformLocation(shaderProgram, name.c_str()),(int)value);
}
