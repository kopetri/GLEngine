#ifndef LIGHTOBJECT_H
#define LIGHTOBJECT_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class LightObject
{
    public:
        static GLuint lightCount;
        GLuint lightID;

        string lightType;

        glm::vec3 lightPosition;
        glm::vec4 lightColor;


        LightObject(string type, glm::vec3 position, glm::vec4 color)
        {
            this->lightType = type;
            this->lightPosition = position;
            this->lightColor = color;
            this->lightID = lightCount;

            lightCount = ++lightCount;
        }


        void renderToShader(Shader& shader)
        {
            shader.Use();

            glUniform3f(glGetUniformLocation(shader.Program, ("lightArray["+ to_string(this->lightID) +"].position").c_str()), this->lightPosition.x, this->lightPosition.y, this->lightPosition.z);
            glUniform4f(glGetUniformLocation(shader.Program, ("lightArray["+ to_string(this->lightID) +"].color").c_str()), this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
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



        void setLightPosition(glm::vec3 position)
        {
            this->lightPosition = position;
        }


        void setLightColor(glm::vec4 color)
        {
            this->lightColor = color;
        }
};


GLuint LightObject::lightCount = 0;


#endif
