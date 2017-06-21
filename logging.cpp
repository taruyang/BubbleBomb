#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "logging.h"

#define LOG_FILE "gl.log"
#define ERR "[GL ERROR]"
#define LOG "[GL LOG]"

namespace gl
{

void RestartLog()
{
    FILE* file = fopen(LOG_FILE, "w");
    if (!file) {
        fprintf(stderr, "%s Could not open GL_LOG_FILE (%s) for writing \n", ERR, LOG_FILE);
        return;
    }

    time_t now = time(NULL);
    char* date = ctime(&now);

    fprintf(file, "\n%s local time %s \n",LOG, date);
    fclose(file);
}

void Log(const char* message, ...)
{
    va_list argList;
    FILE* file = fopen(LOG_FILE, "a");
    if (!file) {
        fprintf(file, "%s Could not open GL_LOG_FILE (%s) for appending \n", ERR, LOG_FILE);
        return;
    }

    va_start(argList, message);
    fprintf(file, "%s", LOG);
    vfprintf(file, message, argList);
    va_end(argList);

    fclose(file);
}

void LogError(const char* message, ...)
{
    va_list argList;
    FILE* file = fopen(LOG_FILE, "a");
    if (!file) {
        fprintf(file, "%s Could not open GL_LOG_FILE (%s) for appending \n", ERR, LOG_FILE);
        return;
    }

    va_start(argList, message);
    fprintf(file, "%s", LOG);
    vfprintf(file, message, argList);
    va_end(argList);

    va_start(argList, message);
    fprintf(stderr, "%s", LOG);
    vfprintf(stderr, message, argList);
    va_end(argList);

    fclose(file);
}

void LogGlVersionInfo()
{
    const GLubyte *renderer;
    const GLubyte *version;

    /** get version info */
    renderer = glGetString( GL_RENDERER );  /** get renderer string */
    version = glGetString( GL_VERSION );    /** version as a string */
    Log("Renderer: %s\n", renderer );
    Log("OpenGL version supported %s\n", version );
    Log("renderer: %s\nversion: %s\n", renderer, version );
}

void LogShaderErrorInfo( GLuint shaderObject )
{
	int maxLength = 2048;
	int actualLength = 0;
	char buffer[2048];
    /// Returns the information log for a shader object
	glGetShaderInfoLog(shaderObject, maxLength, &actualLength, buffer );
	LogError("[Shader] Info log for Shader Index %i:\n%s\n", shaderObject, buffer);
}

void LogShaderProgramInfo( GLuint programObject )
{
	int maxLength = 2048;
	int actualLength = 0;
	char buffer[2048];

	glGetProgramInfoLog( programObject, maxLength, &actualLength, buffer );
	Log("program info log for program object %i:\n%s", programObject, buffer);
}

} /// namespace gl
