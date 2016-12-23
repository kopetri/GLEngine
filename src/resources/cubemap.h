#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "shape.h"


class CubeMap
{
    public:
        Texture texCubemap;
        Shape shapeCubemap;

        CubeMap();
        ~CubeMap();
        void setCubeMap(std::vector<const char*>& faces);
        void renderToShader(Shader& shaderCubemap, Shader& shaderLighting, glm::mat4& projection, Camera& camera);
};


#endif
