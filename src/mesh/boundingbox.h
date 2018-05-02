#pragma once

#include <string>
#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Shader;

class BoundingBox 
{
    public:
        BoundingBox();
        ~BoundingBox();
        void setDimensions(glm::vec3& center, glm::vec3&size);
        void Draw(glm::mat4 &mvp);

    private:
        void setup();
        GLuint shapeVAO, shapeVBO, ibo_elements;

        glm::vec3 size;
        glm::vec3 center;
        glm::mat4 transform;

        // shader
        std::shared_ptr<Shader> shader;
};