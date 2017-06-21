#include <glm/gtc/matrix_transform.hpp>
#include "textRenderer.h"
#include "logging.h"

namespace gl{

using namespace std;

TextRenderer::TextRenderer(Shader* pShader, const char* fontPath) : _fontPath(fontPath)
{
    _pShader = pShader;
}

TextRenderer::~TextRenderer()
{

}

bool TextRenderer::generateFontTexures()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        Log("[TextRenderer] Fail to init FreeType lib \n");
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, _fontPath.c_str(), 0, &face)) {
        Log("[TextRenderer] Fail to load font file %s \n", _fontPath.c_str());
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, 20);

    /// OpenGL requires that textures all have a 4-byte alignment
    /// By setting its unpack alignment equal to 1, there would be no alignment issues
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte c = 0; c < 128; c++)
    {
        /// Load a glyph of char
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            Log("[TextRenderer] Fail to load a glyph for %c \n", c);
            continue;
        }

        /// Generates texture images
        GLuint texObj;
        glGenTextures(1, &texObj);
        glBindTexture(GL_TEXTURE_2D, texObj);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        /// Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        CharInfo charInfo = {texObj,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        _charMap.insert(std::pair<GLchar, CharInfo>(c, charInfo));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

bool TextRenderer::Initialize()
{
    if(!generateFontTexures())
        return false;

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    /// reserve enough memory and update later when rendering characters.
    /// So, GL_DYNAMIC_DRAW is chosen.
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void TextRenderer::Print(const char* str, glm::vec2 pos, GLfloat scale, glm::vec3 fontColor, StudioEnv& studioEnv)
{
    _pShader->Use();

    float near = 0.1f, far = 100.0f;
    float sw = studioEnv.ScreenSize.x;
    float sh = studioEnv.ScreenSize.y;

    glm::mat4 proj(2/sw, 0, 0, 0,
                   0, 2/sh, 0, 0,
                   0, 0, -2/(far - near), 0,
                   0, 0, (far + near)/(near - far), 1);

    glUniformMatrix4fv(glGetUniformLocation(_pShader->GetProgram(), "proj"), 1, GL_FALSE, &proj[0][0]);
    glUniform3f(glGetUniformLocation(_pShader->GetProgram(), "fontColor"), fontColor.x, fontColor.y, fontColor.z);
    glUniform1i(glGetUniformLocation(_pShader->GetProgram(), "texImg"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_vao);

    /// render all characters in the input string
    for (int i = 0; str[i] != '\0'; i++)
    {
        char c = str[i];
        CharInfo ch = _charMap[c];

        GLfloat xPos = pos.x + ch.Bearing.x * scale;
        GLfloat yPos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        /// Update VBO for each character
        GLfloat vertices[6][4] = {
            { xPos,     yPos + h,   0.0, 0.0 },
            { xPos,     yPos,       0.0, 1.0 },
            { xPos + w, yPos,       1.0, 1.0 },

            { xPos,     yPos + h,   0.0, 0.0 },
            { xPos + w, yPos,       1.0, 1.0 },
            { xPos + w, yPos + h,   1.0, 0.0 }
        };
        /// Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texObj);
        /// Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        /// advance is number of 1/64 pixels
        /// so can be shifted by multiplying 64 ( 2 >> 6 ).
        pos.x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


}
