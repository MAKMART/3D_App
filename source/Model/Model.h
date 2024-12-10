#pragma once

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Mesh.h"

class Model {
public:
    std::vector<Texture> loaded_textures;
    std::vector<Mesh> meshes;
    std::string directory;

    Model(const std::string& path);
    void Draw(Shader& shader);

private:
    bool skinned;
    bool isClockwiseOrder(const std::vector<Vertex>& vertices, aiFace& face);
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    unsigned int TextureFromFile(const char* path, const std::string& directory);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
};