#ifndef STAGEOBJECT_H_INCLUDED
#define STAGEOBJECT_H_INCLUDED

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "IGraphicObject.h"
#include "triangleObject.h"
#include "shader.h"

namespace gl
{
using namespace std;

/**
 * @brief   Class to manage and draw a stage object.
 *          This class inherits the Triangle class because a flag is a composition of multiple triangles.
 *          A Stage object consist of 4 vertices on xz plane which y is -1.
 */
class StageObject : public TriangleObject
{

public:
    /**
     * @brief   Constructor of Stage object
     *
     * @param pShader       Shader Object to be used
     * @param texturePath   Texture file path to wrap this object
     */
    StageObject(Shader* pShader, const char* texturePath);

    /**
     * @brief   Destructor of Stage object
     */
    virtual ~StageObject();

    /**
     * @brief   Draw a frame into a back buffer to display at next frame.
     *
     * @param time      Current time
     * @param PV        Project/View matrix
     * @param studioEnv Studio environment
     * @return  result of method
     */
    virtual bool DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv);

};
}  /// namespace gl


#endif // STAGEOBJECT_H_INCLUDED
