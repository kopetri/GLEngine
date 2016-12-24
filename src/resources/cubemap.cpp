#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "cubemap.h"


CubeMap::CubeMap()
{

}


CubeMap::~CubeMap()
{

}


void CubeMap::setCubeMap(const char* texPath)
{
    this->shapeCubemap.setShape("cube", glm::vec3(0.0f, 0.0f, 0.0f));
    this->texCubemap.setTextureHDR(texPath, "cubemapHDR", true);
}


void CubeMap::renderToShader(Shader& shaderCubemap, glm::mat4& projection, Camera& camera)
{
    glDepthFunc(GL_LEQUAL);

    glActiveTexture(GL_TEXTURE0);
    this->texCubemap.useTexture();

    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    this->shapeCubemap.drawShape(shaderCubemap, view, projection, camera);
    glUniform1f(glGetUniformLocation(shaderCubemap.Program, "cameraAperture"), this->cameraAperture);
    glUniform1f(glGetUniformLocation(shaderCubemap.Program, "cameraShutterSpeed"), this->cameraShutterSpeed);
    glUniform1f(glGetUniformLocation(shaderCubemap.Program, "cameraISO"), this->cameraISO);

    glDepthFunc(GL_LESS);
}


void CubeMap::setExposure(GLfloat aperture, GLfloat shutterSpeed, GLfloat iso)
{
    this->cameraAperture = aperture;
    this->cameraShutterSpeed = shutterSpeed;
    this->cameraISO = iso;
}
