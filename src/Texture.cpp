#include "Texture.h"
#include <vector>
#include <functional>
#include <cstring>

Texture Texture::CreateFromRGBA(const unsigned char* rgba, int w, int h,
                                bool sRGB, const std::string& debugName)
{
    Texture t(debugName);
    t.width = w; t.height = h;
    glGenTextures(1, &t.id);
    glBindTexture(GL_TEXTURE_2D, t.id);
    GLenum internal = sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    glTexImage2D(GL_TEXTURE_2D, 0, internal, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return t;
}

Texture Texture::CreateProcedural(int w, int h,
                                  const std::string& debugName,
                                  bool sRGB,
                                  const std::function<void(unsigned char*, int, int)>& fill)
{
    std::vector<unsigned char> data(w * h * 4, 0);
    fill(data.data(), w, h);
    return CreateFromRGBA(data.data(), w, h, sRGB, debugName);
}
