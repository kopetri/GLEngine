#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "boundingbox.h"
#include "shader.h"



BoundingBox::BoundingBox() : center(glm::vec3(0)), size(glm::vec3(-1))
{
   this->setup();
}


BoundingBox::~BoundingBox()
{
    if (ibo_elements)
        glDeleteBuffers(1, &ibo_elements);
}

void BoundingBox::setup()
{
    // init shader
    shader = std::make_shared<Shader>();
    shader->setShader("boundingBox.vert", "boundingBox.frag");

    GLfloat boundingBoxVertices[] = {
        -0.5, -0.5, -0.5, 1.0,
        0.5, -0.5, -0.5, 1.0,
        0.5,  0.5, -0.5, 1.0,
        -0.5,  0.5, -0.5, 1.0,
        -0.5, -0.5,  0.5, 1.0,
        0.5, -0.5,  0.5, 1.0,
        0.5,  0.5,  0.5, 1.0,
        -0.5,  0.5,  0.5, 1.0,
    };

    GLushort elements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
  

    glGenVertexArrays(1, &shapeVAO);
    glGenBuffers(1, &shapeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->shapeVBO);

  
    glBufferData(GL_ARRAY_BUFFER, sizeof(boundingBoxVertices), boundingBoxVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glBindVertexArray(this->shapeVAO);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
}

void BoundingBox::setDimensions(glm::vec3& center, glm::vec3&size) {
    transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
}


void BoundingBox::Draw(glm::mat4 &mvp)
{
    // Model(s) rendering
    shader->useShader();
    GLint mvpLoc = glGetUniformLocation(shader->Program, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp * transform));

    glBindVertexArray(this->shapeVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glLineWidth(1);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
    glBindVertexArray(0);
}