#ifndef LIGHTOBJECT_H
#define LIGHTOBJECT_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



GLfloat lightMeshVertices[] =
{
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};



class LightObject
{
    public:
        static GLuint lightCount;
        static std::vector<LightObject> lightList;

        GLuint lightID, lightVAO, lightVBO;

        string lightType;

        bool lightToMesh;

        glm::vec3 lightPosition;
        glm::vec4 lightColor;


        LightObject(string type, glm::vec3 position, glm::vec4 color, bool isMesh)
        {
            this->lightType = type;
            this->lightPosition = position;
            this->lightColor = color;
            this->lightID = lightCount;
            this->lightToMesh = isMesh;

            if(this->lightToMesh)
            {
                glGenVertexArrays(1, &lightVAO);
                glGenBuffers(1, &lightVBO);
                glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

                glBufferData(GL_ARRAY_BUFFER, sizeof(lightMeshVertices), lightMeshVertices, GL_STATIC_DRAW);
                glBindVertexArray(lightVAO);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
                glBindVertexArray(0);
            }

            lightCount = ++lightCount;
            lightList.push_back(*this);
        }


        void renderToShader(Shader& shader, Camera& camera)
        {
            shader.Use();

            glm::vec3 lightPositionViewSpace = glm::vec3(camera.GetViewMatrix() * glm::vec4(this->lightPosition, 1.0));

            glUniform3f(glGetUniformLocation(shader.Program, ("lightArray["+ to_string(this->lightID) +"].position").c_str()), lightPositionViewSpace.x, lightPositionViewSpace.y, lightPositionViewSpace.z);
            glUniform4f(glGetUniformLocation(shader.Program, ("lightArray["+ to_string(this->lightID) +"].color").c_str()), this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
        }


        void drawLightMesh(Shader& shader, glm::mat4& view, glm::mat4& projection, Camera& camera)
        {
            shader.Use();

            GLint modelLoc = glGetUniformLocation(shader.Program, "model");
            GLint viewLoc = glGetUniformLocation(shader.Program, "view");
            GLint projLoc = glGetUniformLocation(shader.Program, "projection");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.cameraPosition.x, camera.cameraPosition.y, camera.cameraPosition.z);

            glm::mat4 model;
            model = glm::translate(model, this->lightPosition);
            model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }


        string getLightType()
        {
            return lightType;
        }


        glm::vec3 getLightPosition()
        {
            return lightPosition;
        }


        glm::vec4 getLightColor()
        {
            return lightColor;
        }


        GLuint getLightID()
        {
            return lightID;
        }

        bool isMesh()
        {
            return lightToMesh;
        }


        void setLightPosition(glm::vec3 position)
        {
            this->lightPosition = position;
        }


        void setLightColor(glm::vec4 color)
        {
            if (color != this->lightColor)
            {
                this->lightColor = color;
                lightList[this->lightID].lightColor = color;
            }
        }
};


GLuint LightObject::lightCount = 0;
std::vector<LightObject> LightObject::lightList;


#endif
