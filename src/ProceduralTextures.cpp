#include "ProceduralTextures.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <cmath>
#include <cstring>
#include <algorithm>

// ---------- fast bitwise hash for procedural noise ----------
static inline float Hash11(float p)
{
    unsigned int n = (unsigned int)((int)p * 73856093 + 19349663);
    n = (n ^ (n >> 13)) * 1274126177u;
    return (float)(n & 0x00FFFFFF) / 16777215.0f;
}
static inline float Hash21Int(int ix, int iy)
{
    unsigned int n = (unsigned int)(ix * 374761393 + iy * 668265263);
    n = (n ^ (n >> 13)) * 1274126177u;
    return (float)(n & 0x00FFFFFF) / 16777215.0f;
}
static inline float Hash21(float x, float y)
{
    return Hash21Int((int)floorf(x), (int)floorf(y));
}
static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float Smooth(float t) { return t * t * (3.0f - 2.0f * t); }
static float ValueNoise2D(float x, float y)
{
    int xi = (int)floorf(x);
    int yi = (int)floorf(y);
    float xf = x - (float)xi;
    float yf = y - (float)yi;
    float v00 = Hash21Int(xi,     yi);
    float v10 = Hash21Int(xi + 1, yi);
    float v01 = Hash21Int(xi,     yi + 1);
    float v11 = Hash21Int(xi + 1, yi + 1);
    float u = Smooth(xf);
    float v = Smooth(yf);
    return Lerp(Lerp(v00, v10, u), Lerp(v01, v11, u), v);
}
static float FBM(float x, float y, int octaves = 5, float lac = 2.02f, float gain = 0.5f)
{
    float sum = 0.0f, amp = 1.0f, freq = 1.0f, norm = 0.0f;
    for (int i = 0; i < octaves; ++i)
    {
        sum += ValueNoise2D(x * freq, y * freq) * amp;
        norm += amp;
        amp *= gain;
        freq *= lac;
    }
    return sum / norm;
}

static inline unsigned char ClampByte(float v)
{
    int iv = (int)roundf(v);
    if (iv < 0) iv = 0;
    if (iv > 255) iv = 255;
    return (unsigned char)iv;
}

namespace ProceduralTextures
{
    // ---- Wood planks (warm brown, with grain and per-plank variation) ----
    Texture WoodPlanks(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "wood_planks", true,
            [](unsigned char* data, int W, int H)
        {
            int plankCountX = 6;
            float plankW = (float)W / plankCountX;

            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    int pIdx = (int)floorf(x / plankW);
                    float localX = (x - pIdx * plankW) / plankW;
                    float plankSeed = (float)pIdx * 1.37f + 0.5f;

                    float toneVar = Hash11(plankSeed) * 0.3f - 0.15f;
                    glm::vec3 base(0.36f + toneVar, 0.22f + toneVar * 0.9f, 0.12f + toneVar * 0.6f);

                    float grain = FBM(x * 0.02f, y * 0.18f + plankSeed, 5, 2.0f, 0.55f);
                    float rings = 0.5f + 0.5f * sinf(grain * 18.0f + plankSeed * 3.1f);

                    float edgeX = std::min(localX, 1.0f - localX);
                    float edge = 1.0f - std::min(1.0f, edgeX * 50.0f);

                    float g = rings * 0.35f + 0.65f;
                    glm::vec3 col = base * g;
                    col *= 1.0f - 0.55f * edge;

                    float sp = Hash21((float)x, (float)y) * 0.04f;
                    col += glm::vec3(sp);

                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(col.r * 255.0f);
                    data[idx + 1] = ClampByte(col.g * 255.0f);
                    data[idx + 2] = ClampByte(col.b * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }

    Texture DarkWood(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "dark_wood", true,
            [](unsigned char* data, int W, int H)
        {
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    float n = FBM(x * 0.05f, y * 0.5f, 4, 2.0f, 0.5f);
                    float r = 0.28f + n * 0.10f;
                    float g = 0.17f + n * 0.08f;
                    float b = 0.09f + n * 0.05f;
                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(r * 255.0f);
                    data[idx + 1] = ClampByte(g * 255.0f);
                    data[idx + 2] = ClampByte(b * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }

    Texture LightWood(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "light_wood", true,
            [](unsigned char* data, int W, int H)
        {
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    float n = FBM(x * 0.03f, y * 0.45f, 5, 2.0f, 0.5f);
                    float r = 0.78f + n * 0.10f;
                    float g = 0.60f + n * 0.09f;
                    float b = 0.40f + n * 0.07f;
                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(r * 255.0f);
                    data[idx + 1] = ClampByte(g * 255.0f);
                    data[idx + 2] = ClampByte(b * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }

    Texture Plaster(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "plaster", true,
            [](unsigned char* data, int W, int H)
        {
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    float n = FBM(x * 0.025f, y * 0.025f, 4, 2.0f, 0.55f);
                    float v = 0.88f + (n - 0.5f) * 0.06f;
                    float r = v * 1.00f;
                    float g = v * 0.97f;
                    float b = v * 0.92f;
                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(r * 255.0f);
                    data[idx + 1] = ClampByte(g * 255.0f);
                    data[idx + 2] = ClampByte(b * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }

    Texture CeilingMatte(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "ceiling_matte", true,
            [](unsigned char* data, int W, int H)
        {
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    float n = FBM(x * 0.04f, y * 0.04f, 3, 2.0f, 0.5f);
                    float v = 0.94f + (n - 0.5f) * 0.04f;
                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(v * 0.98f * 255.0f);
                    data[idx + 1] = ClampByte(v * 0.98f * 255.0f);
                    data[idx + 2] = ClampByte(v * 0.96f * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }

    Texture TableTennisSurface(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "tt_surface", true,
            [](unsigned char* data, int W, int H)
        {
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    float u = (float)x / W;
                    float v = (float)y / H;
                    glm::vec3 deep(0.06f, 0.16f, 0.44f);
                    glm::vec3 blue(0.12f, 0.32f, 0.72f);
                    glm::vec3 col = glm::mix(deep, blue, FBM(x * 0.02f, y * 0.02f, 3));

                    float ex = std::min(u, 1.0f - u);
                    float ey = std::min(v, 1.0f - v);
                    float edge = std::min(ex, ey);
                    if (edge < 0.01f) col *= 0.55f;

                    bool isEdgeU = (u > 0.04f && u < 0.045f) || (u > 0.955f && u < 0.96f);
                    bool isEdgeV = (v > 0.04f && v < 0.045f) || (v > 0.955f && v < 0.96f);
                    if ((isEdgeU || isEdgeV))
                        col = glm::vec3(0.95f);

                    if ((u > 0.245f && u < 0.255f) || (u > 0.745f && u < 0.755f))
                        col = glm::vec3(0.95f);

                    if (u > 0.495f && u < 0.505f)
                        col = glm::vec3(0.95f);

                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(col.r * 255.0f);
                    data[idx + 1] = ClampByte(col.g * 255.0f);
                    data[idx + 2] = ClampByte(col.b * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }

    Texture Sky(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "sky", true,
            [](unsigned char* data, int W, int H)
        {
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    float v = (float)y / H;
                    glm::vec3 horizon(0.78f, 0.84f, 0.86f);
                    glm::vec3 zenith(0.42f, 0.60f, 0.78f);
                    float t = v * 1.4f;
                    if (t < 0.0f) t = 0.0f;
                    if (t > 1.0f) t = 1.0f;
                    glm::vec3 col = glm::mix(horizon, zenith, t);
                    float clouds = FBM(x * 0.01f, y * 0.005f, 4, 2.0f, 0.5f);
                    col = glm::mix(col, glm::vec3(0.95f), std::max(0.0f, clouds - 0.55f) * 1.2f);

                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(col.r * 255.0f);
                    data[idx + 1] = ClampByte(col.g * 255.0f);
                    data[idx + 2] = ClampByte(col.b * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }

    Texture Grass(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "grass", true,
            [](unsigned char* data, int W, int H)
        {
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    float n = FBM(x * 0.05f, y * 0.05f, 4, 2.0f, 0.5f);
                    float v = 0.32f + n * 0.20f;
                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(v * 0.65f * 255.0f);
                    data[idx + 1] = ClampByte(v * 1.00f * 255.0f);
                    data[idx + 2] = ClampByte(v * 0.55f * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }

    Texture Fabric(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "fabric", true,
            [](unsigned char* data, int W, int H)
        {
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    float n = FBM(x * 0.12f, y * 0.12f, 3, 2.0f, 0.5f);
                    glm::vec3 base(0.55f, 0.30f, 0.20f);
                    glm::vec3 col = base * (0.7f + n * 0.4f);
                    int idx = (y * W + x) * 4;
                    data[idx + 0] = ClampByte(col.r * 255.0f);
                    data[idx + 1] = ClampByte(col.g * 255.0f);
                    data[idx + 2] = ClampByte(col.b * 255.0f);
                    data[idx + 3] = 255;
                }
            }
        });
    }
}
