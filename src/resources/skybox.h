#ifndef SKYBOX_H
#define SKYBOX_H

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


class Skybox
{
    public:
        GLfloat cameraAperture, cameraShutterSpeed, cameraISO;
        Texture texSkybox;

        Skybox();
        ~Skybox();
        void setup();
        void setSkyboxTexture(const char* texPath);
        void renderToShader(Shader& shaderSkybox, glm::mat4& projection, glm::mat4& view);
        void setExposure(GLfloat aperture, GLfloat shutterSpeed, GLfloat iso);
        void setTextureHDR(const char* texPath, std::string texName, bool texFlip);
        void iblSetup(GLuint WIDTH, GLuint HEIGHT);

        void bindEnvMapHDRTexture();
        void bindEnvMapIrradianceTexture();
        void bindEnvMapPrefilterTexture();
        void bindEnvMapLUTTexture();
private:
    // buffer
    GLuint envToCubeFBO;
    GLuint envToCubeRBO;

    GLuint irradianceFBO;
    GLuint irradianceRBO;

    GLuint prefilterFBO;
    GLuint prefilterRBO;

    GLuint brdfLUTFBO;
    GLuint brdfLUTRBO;

    // renderer
    Shape quadRender;
    Shape envCubeRender;

    // shader
    Shader latlongToCubeShader;
    Shader irradianceIBLShader;
    Shader prefilterIBLShader;
    Shader integrateIBLShader;

    // textures
    Texture envMapCube;
    Texture envMapHDR;
    Texture envMapIrradiance;
    Texture envMapPrefilter;
    Texture envMapLUT;

    glm::mat4 envMapProjection;
    std::vector<glm::mat4> envMapView;
};


#endif
