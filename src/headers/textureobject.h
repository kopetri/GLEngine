#ifndef TEXTUREOBJECT_H
#define TEXTUREOBJECT_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>



class TextureObject
{
    public:
        GLuint texID, texWidth, texHeight, texComponents;
        GLenum texFormat;

        TextureObject(const char* texPath)
        {
            string tempPath = string(texPath);
            stbi_set_flip_vertically_on_load(true);

            glGenTextures(1, &texID);
            glBindTexture(GL_TEXTURE_2D, texID);

            int width, height, numComponents;
            unsigned char* texData = stbi_load(tempPath.c_str(), &width, &height, &numComponents, 0);
            this->texWidth = width;
            this->texHeight = height;
            this->texComponents = numComponents;

            if (texData)
            {
                if (numComponents == 1)
                    this->texFormat = GL_RED;
                else if (numComponents == 3)
                    this->texFormat = GL_RGB;
                else if (numComponents == 4)
                    this->texFormat = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, this->texFormat, this->texWidth, this->texHeight, 0, this->texFormat, GL_UNSIGNED_BYTE, texData);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            else
            {
                std::cerr << "TEXTURE FAILED LOADING : " << texPath << std::endl;
            }

            stbi_image_free(texData);
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        ~TextureObject()
        {
            glDeleteTextures(1, &texID);
        }


        void Bind()
        {
            glBindTexture(GL_TEXTURE_2D, texID);
        }
};

#endif
