#include "lighting.h"
#include "shader.h"
#include "light.h"
#include "shape.h"
#include "texture.h"

Lighting::Lighting(GLuint width, GLuint height) :
      simpleShader(std::make_shared<Shader>())
    , lightingBRDFShader(std::make_shared<Shader>())
    , lightPoint1(std::make_shared<Light>())
    , lightPoint2(std::make_shared<Light>())
    , lightPoint3(std::make_shared<Light>())
    , lightDirectional1(std::make_shared<Light>())
    , lightPointPosition1(glm::vec3(1.5f, 0.75f, 1.0f))
    , lightPointPosition2(glm::vec3(-1.5f, 1.0f, 1.0f))
    , lightPointPosition3(glm::vec3(0.0f, 0.75f, -1.2f))
    , lightPointColor1(glm::vec3(1.0f))
    , lightPointColor2(glm::vec3(1.0f))
    , lightPointColor3(glm::vec3(1.0f))
    , lightDirectionalDirection1(glm::vec3(-0.2f, -1.0f, -0.3f))
    , lightDirectionalColor1(glm::vec3(1.0f))
    , lightPointRadius1(3.0f)
    , lightPointRadius2(3.0f)
    , lightPointRadius3(3.0f)
    , backgroundColor(0.f)
    , foregroundColor(1.f)
    , quadRender(std::make_shared<Shape>())
    , backgroundTexture(std::make_shared<Texture>())
    , materialF0(glm::vec3(0.04f))
    , ambientIntensity(0.005f)
    , attenuationMode(2)
    , gBufferView(1)
    , pointMode(false)
    , directionalMode(true)
    , iblMode(true)
    , enableEnvMap(true)
    , enableBackground(false)
    , width(width)
    , height(height)
{
}

Lighting::~Lighting()
{
}

void Lighting::setup()
{
    // framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // init renderer
    quadRender->setShape("quad", glm::vec3(0.0f));

    simpleShader->setShader("lighting/simple.vert", "lighting/simple.frag");
    lightingBRDFShader->setShader("lighting/lightingBRDF.vert", "lighting/lightingBRDF.frag");

    backgroundTexture->setTexture("hdr/ulm.jpg", "ulm", true);

    lightingBRDFShader->useShader();
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "gAlbedo"), 1);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "gNormal"), 2);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "gEffects"), 3);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "sao"), 4);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "envMap"), 5);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "envMapIrradiance"), 6);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "envMapPrefilter"), 7);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "envMapLUT"), 8);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "backgroundTexture"), 9);

    //----------------
    // Light source(s)
    //----------------
    lightPoint1->setLight(lightPointPosition1, glm::vec4(lightPointColor1, 1.0f), lightPointRadius1, true);
    lightPoint2->setLight(lightPointPosition2, glm::vec4(lightPointColor2, 1.0f), lightPointRadius2, true);
    lightPoint3->setLight(lightPointPosition3, glm::vec4(lightPointColor3, 1.0f), lightPointRadius3, true);

    lightDirectional1->setLight(lightDirectionalDirection1, glm::vec4(lightDirectionalColor1, 1.0f));
}

void Lighting::draw(Camera &camera, GBuffer &gBuffer, SSAO &ssao, Skybox &skybox, bool segmentation)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingBRDFShader->useShader();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gPosition);
    if (!segmentation) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedo);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gEffects);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, ssao.saoBlurBuffer);
        glActiveTexture(GL_TEXTURE5);
        skybox.bindEnvMapHDRTexture();
        glActiveTexture(GL_TEXTURE6);
        skybox.bindEnvMapIrradianceTexture();
        glActiveTexture(GL_TEXTURE7);
        skybox.bindEnvMapPrefilterTexture();
        glActiveTexture(GL_TEXTURE8);
        skybox.bindEnvMapLUTTexture();
    }
    glActiveTexture(GL_TEXTURE9);
    backgroundTexture->useTexture();

    if (!segmentation) {
        lightPoint1->setLightPosition(lightPointPosition1);
        lightPoint2->setLightPosition(lightPointPosition2);
        lightPoint3->setLightPosition(lightPointPosition3);
        lightPoint1->setLightColor(glm::vec4(lightPointColor1, 1.0f));
        lightPoint2->setLightColor(glm::vec4(lightPointColor2, 1.0f));
        lightPoint3->setLightColor(glm::vec4(lightPointColor3, 1.0f));
        lightPoint1->setLightRadius(lightPointRadius1);
        lightPoint2->setLightRadius(lightPointRadius2);
        lightPoint3->setLightRadius(lightPointRadius3);

        for (int i = 0; i < Light::lightPointList.size(); i++)
        {
            Light::lightPointList[i].renderToShader(*lightingBRDFShader.get(), camera);
        }

        lightDirectional1->setLightDirection(lightDirectionalDirection1);
        lightDirectional1->setLightColor(glm::vec4(lightDirectionalColor1, 1.0f));

        for (int i = 0; i < Light::lightDirectionalList.size(); i++)
        {
            Light::lightDirectionalList[i].renderToShader(*lightingBRDFShader.get(), camera);
        }
    }

    glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader->Program, "inverseView"), 1, GL_FALSE, glm::value_ptr(glm::transpose(camera.GetViewMatrix())));
    glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader->Program, "inverseProj"), 1, GL_FALSE, glm::value_ptr(glm::inverse(camera.GetProjectionMatrix())));
    glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
    if (!segmentation) {
        glUniform1f(glGetUniformLocation(lightingBRDFShader->Program, "materialRoughness"), gBuffer.materialRoughness);
        glUniform1f(glGetUniformLocation(lightingBRDFShader->Program, "materialMetallicity"), gBuffer.materialMetallicity);
        glUniform3f(glGetUniformLocation(lightingBRDFShader->Program, "materialF0"), materialF0.r, materialF0.g, materialF0.b);
        glUniform1f(glGetUniformLocation(lightingBRDFShader->Program, "ambientIntensity"), ambientIntensity);
        glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "gBufferView"), gBufferView);
        glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "pointMode"), pointMode);
        glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "directionalMode"), directionalMode);
        glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "iblMode"), iblMode);
        glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "attenuationMode"), attenuationMode);
        glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "enableEnvMap"), enableEnvMap);
    }
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "enableBackground"), enableBackground);
    glUniform1i(glGetUniformLocation(lightingBRDFShader->Program, "segmentationEnabled"), segmentation);
    glUniform3fv(glGetUniformLocation(lightingBRDFShader->Program, "backgroundColor"), 1, glm::value_ptr(backgroundColor));
    glUniform3fv(glGetUniformLocation(lightingBRDFShader->Program, "foregroundColor"), 1, glm::value_ptr(foregroundColor));

    quadRender->drawShape();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Lighting::forwardPass(Camera &camera, GBuffer &gBuffer)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


    // Copy the depth informations from the Geometry Pass into the default framebuffer
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    // Shape(s) rendering
    if (pointMode)
    {
        simpleShader->useShader();
        glUniformMatrix4fv(glGetUniformLocation(simpleShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(simpleShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

        for (int i = 0; i < Light::lightPointList.size(); i++)
        {
            glUniform4f(glGetUniformLocation(simpleShader->Program, "lightColor"), Light::lightPointList[i].getLightColor().r, Light::lightPointList[i].getLightColor().g, Light::lightPointList[i].getLightColor().b, Light::lightPointList[i].getLightColor().a);

            if (Light::lightPointList[i].isMesh())
                Light::lightPointList[i].lightMesh.drawShape(*simpleShader.get(), camera.GetViewMatrix(), camera.GetProjectionMatrix(), camera);
        }
    }
}

void Lighting::setRender(const Shape &render)
{
    quadRender = std::make_shared<Shape>(render);
}