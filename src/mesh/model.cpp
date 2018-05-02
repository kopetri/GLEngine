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
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"
#include "mesh.h"
#include "shader.h"


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
        aiProcess_FlipUVs |
        aiProcess_FixInfacingNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_TransformUVCoords |
        aiProcess_GenSmoothNormals |
        aiProcess_GenNormals |
        aiProcess_GenUVCoords);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    this->processNode(scene->mRootNode, scene);
}

void Model::Draw()
{
    for (GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i].Draw();
}

void Model::processNode(aiNode *node, const aiScene* scene)
{
    for (GLuint i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }

    for (GLuint i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }

    GLfloat min_x, min_y, min_z, max_x, max_y, max_z;
    min_x = max_x = meshes[0].vertices[0].Position.x;
    min_y = max_y = meshes[0].vertices[0].Position.y;
    min_z = max_z = meshes[0].vertices[0].Position.z;

    for (auto mesh : meshes) {
        for (int i = 0; i < mesh.vertices.size(); i++) {
            if (mesh.vertices[i].Position.x < min_x) min_x = mesh.vertices[i].Position.x;
            if (mesh.vertices[i].Position.x > max_x) max_x = mesh.vertices[i].Position.x;
            if (mesh.vertices[i].Position.y < min_y) min_y = mesh.vertices[i].Position.y;
            if (mesh.vertices[i].Position.y > max_y) max_y = mesh.vertices[i].Position.y;
            if (mesh.vertices[i].Position.z < min_z) min_z = mesh.vertices[i].Position.z;
            if (mesh.vertices[i].Position.z > max_z) max_z = mesh.vertices[i].Position.z;
        }
    }

    size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for (GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return Mesh(vertices, indices);
}