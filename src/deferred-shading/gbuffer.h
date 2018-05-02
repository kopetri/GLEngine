#pragma once

#include <glad/glad.h>
#include "camera.h"
#include <memory>

class Shader;
class Texture;
class Model;
class BoundingBox;

class GBuffer
{
public:
    enum TextureUsage : int
    {
        Normal = 0,
        Albedo = 1,
        Roughness = 2,
        AmbientOcclusion = 3,
        Metalness = 4
    };

    GBuffer(const GLuint width, const GLuint height);
    ~GBuffer();

    void setup();
    void draw(Camera camera);

    // buffers
    GLuint gBuffer;
    GLuint gPosition;
    GLuint gAlbedo;
    GLuint gEffects;
    GLuint gNormal;
    GLuint gBoundingBox;
    GLuint zBuffer;

    GLfloat modelRotationSpeed;
    GLfloat materialRoughness;
    GLfloat materialMetallicity;

    bool useRoughnessTexture;
    bool useAlbedoTexture;
    bool useMetalnessTexture;
    bool useNormalTexture;
    bool negativeNormals;
    bool enableBoundingBox;

    glm::vec3 modelRotationAxis;
    glm::vec3 modelPosition;
    glm::vec3 modelScale;
    glm::vec3 albedoColor;

    void loadModel(const std::string path, const glm::vec3 scale);
    void setTexture(TextureUsage texUse, const char* path, std::string name, bool flip);
private:
    GLuint width;
    GLuint height;

    glm::mat4 projViewModel;
    glm::mat4 prevProjViewModel;

    // shader
    std::shared_ptr<Shader> shader;

    // textures
    std::shared_ptr<Texture> objectAlbedo;
    std::shared_ptr<Texture> objectNormal;
    std::shared_ptr<Texture> objectRoughness;
    std::shared_ptr<Texture> objectMetalness;
    std::shared_ptr<Texture> objectAO;

    // model
    std::shared_ptr<Model> objectModel;
    std::shared_ptr<BoundingBox> boundingBox;
};
