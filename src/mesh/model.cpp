#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"
#include "mesh.h"


Model::Model()
{

}


Model::~Model()
{

}


void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(MODEL_PATH + path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs|
        aiProcess_FixInfacingNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_TransformUVCoords |
        aiProcess_GenSmoothNormals |
        aiProcess_GenNormals |
        aiProcess_GenUVCoords);

    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    this->processNode(scene->mRootNode, scene);
}

void Model::Draw()
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i].Draw();
}

glm::vec3 Model::centroid()
{
    auto centroid = glm::vec3(0.f);
    for(auto m : this->meshes)
    {
        centroid += m.centroid;
    }
    centroid /= this->meshes.size();
    return centroid;
}


void Model::processNode(aiNode* node, const aiScene* scene)
{
    for(GLuint i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }

    for(GLuint i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    glm::vec3 centroid;

    auto min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX;
    auto max_x = -FLT_MIN, max_y = -FLT_MIN, max_z = -FLT_MIN;

    for(GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        min_x = glm::min(min_x, vector.x);
        min_y = glm::min(min_y, vector.y);
        min_z = glm::min(min_z, vector.z);

        max_x = glm::max(max_x, vector.x);
        max_y = glm::max(max_y, vector.y);
        max_z = glm::max(max_z, vector.z);

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            //std::cout << vec.x << std::endl;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    centroid.x = min_x + (max_x - min_x) * 0.5f;
    centroid.y = min_y + (max_y - min_y) * 0.5f;
    centroid.z = min_z + (max_z - min_z) * 0.5f;

    for(GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for(GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return Mesh(vertices, indices, centroid);
}
