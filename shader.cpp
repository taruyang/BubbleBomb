#include <stdio.h>
#include <stdlib.h>
#include "shader.h"
#include "logging.h"

namespace gl
{

using namespace std;

/**
 * @brief   Check the validity of a program object
 *          Validates a program object and return the result. If it is not valid,
 *          error information will be written in the log file.
 *
 * @param programObject     The program object to validate
 * @return                  The validation result of the program object
 */
static bool IsValidShaderProgram( GLuint programObject )
{
	int params = -1;

	/// Validates a program object
	glValidateProgram( programObject );
	/// Returns a parameter from a program object
	glGetProgramiv( programObject, GL_VALIDATE_STATUS, &params );
	Log( "program %i GL_VALIDATE_STATUS = %i\n", programObject, params );
	if ( GL_TRUE != params ) {
        LogShaderProgramInfo( programObject );
		return false;
	}

	return true;
}

/**
 * @brief   Read Shader code file
 *          Read a program file named fileName and put the read data into the
 *          string passed as an input parameter.
 *
 * @param fileName          The shadow program file name
 * @param shaderCode        The string buffer to contain the content in the file (Return)
 * @return                  The result of the program file reading.
 */
static bool ReadShaderCodeFile( const char *fileName, string &shaderCode)
{
	FILE *file = fopen( fileName, "r" );
	if ( !file ) {
		LogError( "opening file for reading: %s\n", fileName );
		return false;
	}

	/// get file size
	fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

    char* programBuffer = (char*)malloc(sizeof(char) * (fileSize + 1));
    if(programBuffer == NULL) {
		LogError( "Memory allocation fail to read file(%s).\n", fileName );
        return false;
    }

	size_t readCount = fread( programBuffer, 1, fileSize, file );
	if ( (int)readCount != fileSize ) {
		LogError( "Reading file(%s).\n", fileName );
		free(programBuffer);
		return false;
	}

	/// append \0 to end of file string
	programBuffer[fileSize] = 0;

	shaderCode.append(programBuffer);
	fclose( file );
	free(programBuffer);

	return true;
}

/**
 * @brief   Compile shader object
 *          If there is any error while compiling,
 *          the error information will be written in the log file.
 *
 * @param shaderObject      Shader object
 * @param shaderType        Shader type
 * @param shaderPath        Shader file path
 * @return                  The result of the compilation
 */
static bool CompileShaderObject(GLuint& shaderObject, GLenum shaderType, const GLchar* shaderPath)
{
    string  shaderCode;      /// Shader buffer containing all code

    if(!ReadShaderCodeFile(shaderPath, shaderCode))
        return false;

    /** Create fragment shader object and compile */
    shaderObject = glCreateShader( shaderType );

    GLint params;
    const GLchar* t = (const GLchar *)shaderCode.c_str();
    /// Replaces the source code in a shader object
	glShaderSource( shaderObject, 1, &t, NULL );
	/// compiles a shader object
	glCompileShader( shaderObject );

	/** check for shader compile errors */
	glGetShaderiv( shaderObject, GL_COMPILE_STATUS, &params );
	if ( GL_TRUE != params ) {
		LogError(" GL shader object %i compile error. \n", shaderObject);
		LogShaderErrorInfo(shaderObject);
		return false; /** or exit or something */
	}

    return true;
}

/**
 * @brief   Link shader programs
 *          If there is any error while linking,
 *          the error information will be written in the log file.
 *
 * @param program           Program object
 * @param fragmentShader    Fragment Shader object to be linked
 * @param vertexShader      Vertex Shader object to be linked
 * @param tcsShader         Tessellation Control Shader object to be linked
 * @param tesShader         Tessellation Evaluation Shader object to be linked
 * @param gsShader          Geometry Shader object to be linked
 * @return                  The result of link
 */
static bool LinkShaderPrograms(GLuint program, GLuint fragmentShader, GLuint vertexShader,
                               GLuint tcsShader, GLuint tesShader, GLuint gsShader)
{
    GLint params;

   	if(vertexShader)    glAttachShader( program, vertexShader );
	if(tcsShader)       glAttachShader( program, tcsShader );
	if(tesShader)       glAttachShader( program, tesShader );
	if(gsShader)        glAttachShader( program, gsShader );
	if(fragmentShader)  glAttachShader( program, fragmentShader );

    /// Links a program object
	glLinkProgram(program);

	/** check for shader linking errors */
	glGetProgramiv( program, GL_LINK_STATUS, &params );
	if ( GL_TRUE != params ) {
		LogError( "could not link shader program object %i\n", program);
		LogShaderProgramInfo(program);
		return false;
	}

    return true;
}

bool Shader::Initialize()
{
    GLuint  vertexShader = 0;       /// Vertex shader object
    GLuint  fragmentShader = 0;     /// Fragment shader object
    GLuint  tcsShader = 0;          /// Tessellation Control shader object
    GLuint  tesShader = 0;          /// Tessellation Evaluation shader object
    GLuint  gsShader = 0;           /// Geometry shader object

    /** Create vertex shader object and compile */
    if(!CompileShaderObject(vertexShader, GL_VERTEX_SHADER, _vertexPath))
    {
        LogError("Compile a vertex shader object fail \n");
        return false;
    }

    /** Create fragment shader object and compile */
    if(!CompileShaderObject(fragmentShader, GL_FRAGMENT_SHADER, _fragmentPath))
    {
        LogError("Compile a fragment shader object fail \n");
        return false;
    }

    /** Create Tessellation Control shader object and compile */
    if(_tcsPath != nullptr) /// optional shader
    if(!CompileShaderObject(tcsShader, GL_TESS_CONTROL_SHADER, _tcsPath))
    {
        LogError("Compile a Tessellation Control shader object fail \n");
        return false;
    }

    /** Create Tessellation Evaluation shader object and compile */
    if(_tesPath != nullptr) /// optional shader
    if(!CompileShaderObject(tesShader, GL_TESS_EVALUATION_SHADER, _tesPath))
    {
        LogError("Compile a Tessellation Evaluation shader object fail \n");
        return false;
    }

    /** Create Geometry shader object and compile */
    if(_gsPath != nullptr) /// optional shader
    if(!CompileShaderObject(gsShader, GL_GEOMETRY_SHADER, _gsPath))
    {
        LogError("Compile a Geometry shader object fail \n");
        return false;
    }

    _program = glCreateProgram();
	if(!LinkShaderPrograms(_program, fragmentShader, vertexShader, tcsShader, tesShader, gsShader))
    {
        LogError("Linking error ! \n");
        return false;
    }

    if(!IsValidShaderProgram(_program))
    {
        LogError("This is not valid program \n");
        return false;
    }
    /// Delete the shaders as they're linked into our program now and no longer necessery
    if(vertexShader) glDeleteShader(vertexShader);
    if(fragmentShader) glDeleteShader(fragmentShader);
    if(tcsShader) glDeleteShader(tcsShader);
    if(tesShader) glDeleteShader(tesShader);
    if(gsShader) glDeleteShader(gsShader);

    return true;
}


} /// namespace gl
