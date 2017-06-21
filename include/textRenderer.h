#ifndef TEXTRENDERER_H_INCLUDED
#define TEXTRENDERER_H_INCLUDED

#include <string>
#include <map>
#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "studioEnv.h"

namespace gl{

using namespace std;

struct CharInfo {
    GLuint      texObj;     /// Texture object for a character
    glm::ivec2  Size;       /// The size of a character
    glm::ivec2  Bearing;
    long int    Advance;
};

/**
 * @brief   Class to manage and render text on screen.
 */
class TextRenderer {
    string      _fontPath;
    map<GLchar, CharInfo>   _charMap;

    GLuint      _vbo;               /// vertex buffer object
    GLuint      _ebo;               /// element array buffer object
    GLuint      _vao;               /// vertex array object

    Shader*     _pShader;           /// Shader object

    /**
     * @brief   Generates texture objects for characters
     *          and store them into internal container with
     *          related font information for future usage.
     */
    bool generateFontTexures();
public :
    /**
     * @brief   Constructor of Mesh object
     *
     * @param pShader       Shader Object to be used
     * @param fontPath      Font file path
     */
    TextRenderer(Shader* pShader, const char* fontPath);

    /**
     * @brief   Destructor of Triangle object
     */
    ~TextRenderer();

    /**
     * @brief   Print 2D text on screen
     *
     * @param str          String to be displayed
     * @param pos           Starting position of text
     * @param scale         Scale
     * @param fontColor     Text Color
     * @param studioEnv     Studio environment
     */
    void Print(const char* str, glm::vec2 pos, GLfloat scale, glm::vec3 fontColor, StudioEnv& studioEnv);

    /**
     * @brief   Initialize all processes before draw an object
     */
    bool Initialize();

};
}

#endif // TEXTRENDERER_H_INCLUDED
