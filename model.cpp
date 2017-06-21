#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <SOIL.h>
#include "model.h"
#include "logging.h"

namespace gl
{

using namespace std;

Model::Model(const char* path)
{
    _modelPath = string(path);
    _meshes.clear();
    _texturesCache.clear();
}

bool Model::Parse()
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LogError("[Model][Parse] ASSIMP error. Fail to read scene from file. %s \n", importer.GetErrorString());
        return false;
    }

    /// store the directory of the file
    _directory = _modelPath.substr(0, _modelPath.find_last_of('/'));
    Log("[Model][Parse] the directory path of model file (%s) \n", _directory.c_str());

    /// Parses all nodes in the scene
    parseNodeData(scene->mRootNode, scene);

    Log("[DEBUG] end of parseNodeData \n");

    return true;
}

bool Model::parseNodeData(aiNode* node, const aiScene* scene)
{
    /// Extracts all mesh object in this node.
    Log("[DEBUG] mNumMeshes %d \n", node->mNumMeshes);

    for(GLuint i = 0; i < node->mNumMeshes; i++)
    {
        /// The node object only contains index to the actual objects in the scene.
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        MeshData data = parseMeshData(mesh, scene);
        _meshes.push_back(data);
    }

    Log("[DEBUG] mNumChildren %d \n", node->mNumChildren);

    /// Parses all children nodes
    for(GLuint i = 0; i < node->mNumChildren; i++)
        parseNodeData(node->mChildren[i], scene);

    return true;
}

MeshData Model::parseMeshData(aiMesh* mesh, const aiScene* scene)
{
    Log("[DEBUG] start parseMeshData %d vertices \n", mesh->mNumVertices);

    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    /// Extracts vertex information
    for(GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vert;

        vert.Position.x = mesh->mVertices[i].x;
        vert.Position.y = mesh->mVertices[i].y;
        vert.Position.z = mesh->mVertices[i].z;

        if(mesh->mNormals) {
            vert.Normal.x = mesh->mNormals[i].x;
            vert.Normal.y = mesh->mNormals[i].y;
            vert.Normal.z = mesh->mNormals[i].z;
        }
        else
            vert.Normal = vert.Position;

        /// check if the mesh includes texture coordinates
        if(mesh->mTextureCoords[0])
        {
            vert.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vert.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else
            vert.TexCoords = glm::vec2(0.0f);

        vertices.push_back(vert);
    }

    Log("[DEBUG] mNumFaces %d \n", mesh->mNumFaces);
    /// Extracts index information
    for(GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Log("[DEBUG] mMaterialIndex %d \n", mesh->mMaterialIndex);
    /// Extracts material information
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        printf("[DEBUG] parseTextures aiTextureType_DIFFUSE \n");
        parseTextures(material, aiTextureType_DIFFUSE, textures);
        printf("[DEBUG] parseTextures aiTextureType_SPECULAR \n");
        parseTextures(material, aiTextureType_SPECULAR, textures);
    }

    return MeshData(vertices, indices, textures);
}

bool Model::parseTextures(aiMaterial* mat, aiTextureType type, vector<Texture>& textures)
{
    Log("[DEBUG] parseTextures type %d count %d \n", type, mat->GetTextureCount(type));

    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        /// Check if there is same texture in the cache
        GLboolean isThere = false;
        for(GLuint j = 0; j < _texturesCache.size(); j++)
        {
            if(strcmp(_texturesCache[j].path.c_str(), str.C_Str()) == 0)
            {
                textures.push_back(_texturesCache[j]);
                isThere = true;
                break;
            }
        }

        /// In case there is no loaded texture, loads texture from file.
        if(!isThere)
        {
            Texture texture;
            texture.object = getTextureObjFromFile(str.C_Str(), _directory);
            texture.type = (type == aiTextureType_DIFFUSE) ? Texture_Diffuse :Texture_Specular;
            texture.path = string(str.C_Str());
            textures.push_back(texture);
            _texturesCache.push_back(texture);
        }
    }

    return true;
}

GLint Model::getTextureObjFromFile(const char* path, string directory)
{
    /// Make Texture file path
    string texturePath;

    string str;

    for(int i = 0; path[i] != '\0'; i++)
    {
        if(path[i] == '\\') str += '/';
        else if(path[0] == '/') continue;
        else
            str += path[i];
    }

    texturePath = directory + '/' + str;

    GLuint textureObj;
    glGenTextures(1, &textureObj);

    Log("[DEBUG] getTextureObjFromFile %s \n", texturePath.c_str());

    /// All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    glBindTexture(GL_TEXTURE_2D, textureObj);

    /// Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    /// Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /// Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image(texturePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    if(image == NULL)
    {
        LogError("SOIL_load_image fail \n");
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    /// Unbind texture when done, so we won't accidentally mess up our texture.
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureObj;
}

vector<MeshData> Model::GetMeshData()
{
    return _meshes;
}

}
