#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Model::Model(const std::string& path) {
    loadModel(path);
}

void Model::Draw(Shader& shader) {
    for (auto& mesh : meshes) {
        mesh.Draw(shader);
    }
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error("Assimp Error: " + std::string(importer.GetErrorString()));
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

//Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
//    std::vector<Vertex> vertices;
//    std::vector<unsigned int> indices;
//    std::vector<Texture> textures;
//
//    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
//        Vertex vertex;
//        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
//        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
//        //vertex.Normal = glm::vec3(-mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z);
//        if (mesh->mTextureCoords[0])
//            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
//            //vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, -mesh->mTextureCoords[0][i].y);
//        else
//            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//        vertices.push_back(vertex);
//    }
//
//    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//        aiFace face = mesh->mFaces[i];
//        for (unsigned int j = 0; j < face.mNumIndices; j++)
//            indices.push_back(face.mIndices[j]);
//    }
//
//    if (mesh->mMaterialIndex >= 0) {
//        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//
//        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//
//        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//    }
//
//    return Mesh(vertices, indices, textures);
//}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Step 1: Collect vertex data
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // Step 2: Process faces (indices)
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        // Check if the face has three indices (triangular face)
        if (face.mNumIndices == 3) {
            // Flip the winding order if the face's indices are in counter-clockwise order
            if (!isClockwiseOrder(vertices, face)) {
                std::swap(face.mIndices[0], face.mIndices[1]);
                std::swap(face.mIndices[1], face.mIndices[2]);
            }
        }

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Step 3: Load textures if available
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

// Helper function to check if the winding order is clockwise
bool Model::isClockwiseOrder(const std::vector<Vertex>& vertices, aiFace& face) {
    // Get the vertices of the face
    glm::vec3 v0 = vertices[face.mIndices[0]].Position;
    glm::vec3 v1 = vertices[face.mIndices[1]].Position;
    glm::vec3 v2 = vertices[face.mIndices[2]].Position;

    // Calculate the cross product to determine winding order
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 cross = glm::cross(edge1, edge2);

    // If the cross product's Z value is positive, it's counter-clockwise, otherwise it's clockwise
    return cross.z < 0;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::string filename = str.C_Str();

        // Check if the path is already absolute or relative to the directory
        if (filename.find(directory) == std::string::npos) {
            filename = directory + '/' + filename;
        }
        std::cout << "Fixed texture path: " << filename << std::endl;
        Texture texture = { TextureFromFile(filename.c_str(), directory), typeName, filename };
        textures.emplace_back(texture);
    }
    return textures;
}
unsigned int Model::TextureFromFile(const char* path, const std::string& directory) {
    // Use path directly instead of appending directory again
    std::string filename(path);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = (nrComponents == 1) ? GL_RED : (nrComponents == 3) ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
