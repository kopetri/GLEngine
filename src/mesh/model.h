#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

class Shader;

class Model 
{
    public:
        Model();
        ~Model();
        void loadModel(std::string path);
        void Draw();

        glm::vec3 getSize() { return size; }
        glm::vec3 getCenter() { return center; }
        std::vector<glm::vec3> boundingPoints();

    private:
        std::vector<Mesh> meshes;

        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        glm::vec3 size;
        glm::vec3 center;
};