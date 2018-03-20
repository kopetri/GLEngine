#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

class Shape;
class Texture;
class Shader;


class Skybox
{
    public:
        GLfloat cameraAperture, cameraShutterSpeed, cameraISO;

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
    std::shared_ptr<Shape> quadRender;
    std::shared_ptr<Shape> envCubeRender;

    // shader
    std::shared_ptr<Shader> latlongToCubeShader;
    std::shared_ptr<Shader> irradianceIBLShader;
    std::shared_ptr<Shader> prefilterIBLShader;
    std::shared_ptr<Shader> integrateIBLShader;

    // textures
    std::shared_ptr<Texture> envMapCube;
    std::shared_ptr<Texture> envMapHDR;
    std::shared_ptr<Texture> envMapIrradiance;
    std::shared_ptr<Texture> envMapPrefilter;
    std::shared_ptr<Texture> envMapLUT;
    std::shared_ptr<Texture> texSkybox;

    glm::mat4 envMapProjection;
    std::vector<glm::mat4> envMapView;
};


#endif
