#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "meshObject.h"
#include "shader.h"

namespace gl {

using namespace std;

class Model
{
public:
    /**
     * @brief   Constructor of model
     *
     * @param filePath      model file path
     */
    Model(const char* filePath);

    /**
     * @brief   Parses all model data from Assimp library
     *          and puts them into internal container
     * @return  result of method
     */
    bool Parse();

    /**
     * @brief   Return all mesh data
     * @return  mesh data container
     */
    vector<MeshData>    GetMeshData();

private:
    string              _modelPath;
    vector<MeshData>    _meshes;

    string              _directory;
    vector<Texture>     _texturesCache;

    /**
     * @brief   Processes mesh data at the node and its all children nodes.
     *          After processing mesh, this creates MeshData structures from the mesh data
     *          and stores it into a mesh objects container
     *
     * @param node          Assimp Node struct
     * @param scene         Assimp Scene struct
     * @return  result of method
     */
    bool parseNodeData(aiNode* node, const aiScene* scene);

    /**
     * @brief   Processes Assimp mesh struct and generates a MeshData structure containing all information
     *
     * @param mesh          Assimp Mesh struct
     * @param scene         Assimp Scene struct
     * @return  Mesh object containing all mesh information
     */
    MeshData parseMeshData(aiMesh* mesh, const aiScene* scene);

    /**
     * @brief   Parse Assimp material struct and generates Texture maps
     *
     * @param mat           Assimp Material struct
     * @param type          Assimp Texture Type
     * @param textures      Texture map
     * @return  Result of methods
     */
    bool parseTextures(aiMaterial* mat, aiTextureType type, vector<Texture>& textures);

    /**
     * @brief   Create a texture object to handle the input texture file
     *
     * @param path          Texture file path
     * @param directory     Directory the texture file is
     * @return  Texture object
     */
    GLint getTextureObjFromFile(const char* path, string directory);
};

}
#endif // MODEL_H_INCLUDED
