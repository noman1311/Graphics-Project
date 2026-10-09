#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

// Lightweight texture wrapper for the indoor-rooms project.
// All textures are generated procedurally in code so that the project has no
// external asset / network dependency. The texture stores its data in sRGB
// space (GL_SRGB8_ALPHA8) so colors are converted to linear in the shader.
class Texture {
public:
    unsigned int id = 0;
    int width = 0;
    int height = 0;
    std::string debugName;

    Texture() = default;
    Texture(const std::string& debugName_) : debugName(debugName_) {}

    // Create an OpenGL texture from RGBA8 pixel data. Generates mipmaps.
    // sRGB = true will use GL_SRGB8_ALPHA8 so the shader's automatic linearization works.
    static Texture CreateFromRGBA(const unsigned char* rgba, int w, int h,
                                  bool sRGB = true, const std::string& debugName = "tex");

    // Create a texture procedurally. The pixelData callback is invoked with a
    // pointer to a (w*h*4) byte buffer; the callback must fill it.
    static Texture CreateProcedural(int w, int h,
                                    const std::string& debugName,
                                    bool sRGB,
                                    const std::function<void(unsigned char*, int, int)>& fill);

    void Bind(int unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void Destroy()
    {
        if (id) { glDeleteTextures(1, &id); id = 0; }
    }
};

#endif
