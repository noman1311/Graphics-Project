#ifndef HUD_RENDERER_H
#define HUD_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

// A minimal bitmap-font HUD renderer for OpenGL 3.3 Core Profile.
// Generates a tiny procedural 8x8 ASCII glyph atlas at init time.
// All rendering is done in screen-space (orthographic projection).
class HudRenderer {
public:
    HudRenderer() = default;

    void Init(int screenW, int screenH);
    void Resize(int screenW, int screenH);

    // Draw text at pixel coordinates (x, y=top-left corner).
    // scale: pixel multiplier (1.0 = 8px glyphs, 2.0 = 16px, etc.)
    void RenderText(const std::string& text, float x, float y,
                    float scale, glm::vec3 color);

    // Draw centered text at vertical position y.
    void RenderTextCentered(const std::string& text, float y,
                            float scale, glm::vec3 color);

    // Must be called before any RenderText calls each frame.
    void BeginFrame();
    // Must be called after all RenderText calls each frame.
    void EndFrame();

private:
    unsigned int shaderID = 0;
    unsigned int VAO = 0, VBO = 0;
    unsigned int fontTexture = 0;
    int screenWidth = 800, screenHeight = 600;
    glm::mat4 ortho;

    void buildFontAtlas();
    void buildShader();
};

#endif
