#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#define GLEW_NO_GLU
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gl
{

/**
 * @brief Class for a shader object.
 *        An object contains a program object.
 *        The program object is constructed at "Initialize" method.
 *        So, before using the program by calling "Use" method, "Initialize" should be called.
 *
 */
class Shader
{
    GLuint        _program;
    const GLchar* _vertexPath;
    const GLchar* _fragmentPath;
    const GLchar* _tcsPath;         /// Tessellation Control Shader path
    const GLchar* _tesPath;         /// Tessellation Evaluation Shader path
    const GLchar* _gsPath;          /// Geometry Shader path

public:
    /**
     * @brief   Constructor of Shader object
     *
     * @param vertexPath     Vertex shader source code file path
     * @param fragmentPath   Fragment shader source code file path
     */
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* tcsPath = nullptr
           , const GLchar* tesPath = nullptr, const GLchar* gsPath = nullptr)
            : _vertexPath(vertexPath), _fragmentPath(fragmentPath),
                _tcsPath(tcsPath), _tesPath(tesPath), _gsPath(gsPath) {};

    /**
     * @brief   Destructor of Shader object
     *
     */
    ~Shader()
    {
        glDeleteProgram(_program);
    }

    /**
     * @brief   Construct a program object.
     *          Build and compile the vertex shader and the fragment shader and makes program object.
     *
     * @return  result of method
     */
    bool Initialize();

    /**
     * @brief   Get Shader program object
     *
     * @return  Shader program object
     */
    const GLuint GetProgram() { return _program; };

    /**
     * @brief   Uses the current shader
     */
    void Use()
    {
        glUseProgram(this->_program);
    }
};


}
#endif // SHADER_H_INCLUDED
