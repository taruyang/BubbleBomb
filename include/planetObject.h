#ifndef PLANETOBJECT_H_INCLUDED
#define PLANETOBJECT_H_INCLUDED


#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "IGraphicObject.h"
#include "shader.h"
#include "meshObject.h"

namespace gl
{

using namespace std;

/**
 * @brief   Class to manage and draw a mesh object.
 *          This class is the base class of all mesh graphic object.
 */
class PlanetObject : public MeshObject
{

public:
    /**
     * @brief   Constructor of Rock object
     *
     * @param pShader       Shader Object to be used
     * @param vertices      All vertices for this mesh object
     * @param indices       All indices for this mesh object
     * @param textures      All texture information for this mesh object
     */
    PlanetObject(Shader* pShader, vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

    /**
     * @brief   Destructor of Mesh object
     */
    virtual ~PlanetObject();

    /**
     * @brief   Initialize all processes before draw an object
     */
    virtual bool Initialize();

    /**
     * @brief   Check this object is intersected with the ray
     * @param rayOrg    Ray origin in world space
     * @param rayDir    Ray direction in world space
     * @return  distance    distance from ray when the object is intersected
     * @return  result of method
     */
    virtual bool IsIntersected(glm::vec3 rayOrg, glm::vec3 rayDir, float *distance){return false;};

    /**
     * @brief   Get current information
     * @return objInfo    Current information
     */
    void GetCurObjectInfo(struct ObjectInfo& objInfo);

    /**
     * @brief   Draw a frame into a back buffer to display at next frame.
     *
     * @param time      Current time
     * @param PV        Project/View matrix
     * @return  result of method
     */
    virtual bool DrawNextFrame(const double time, glm::mat4 PV, StudioEnv& studioEnv);

protected:
    GLuint          _amount = 150;
    glm::mat4*      _modelMatrices;

    void            DrawContainer();
    bool            generateModelMatrix();
    bool            generateInstanceAttribute();

};

}
#endif // PLANETOBJECT_H_INCLUDED
