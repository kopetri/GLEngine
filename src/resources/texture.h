#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>


class Texture
{
    public:
        GLuint texID, texWidth, texHeight, texComponents;
        GLfloat anisoFilterLevel;
        GLenum texType, texInternalFormat, texFormat;
        std::string texName;

        Texture();
        ~Texture();
        void setTexture(const char* texPath, std::string texName, bool texFlip);
        void setTextureHDR(const char* texPath, std::string texName, bool texFlip);
        void setTextureCube(std::vector<const char*>& faces, bool texFlip);
        GLuint getTexWidth();
        GLuint getTexHeight();
        std::string getTexName();
        void useTexture();
};

#endif
