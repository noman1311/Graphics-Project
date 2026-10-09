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

    Texture CarromBoardSurface(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "carrom_surface", true,
            [](unsigned char* data, int W, int H)
        {
            const float D_base = 0.3167f;
            const float D_inner = 0.3047f;
            const float D_outer = 0.3287f;
            const float R_baseCircle = 0.0150f;
            const float R_queen = 0.0160f;
            const float R_innerCircle = 0.0567f;
            const float R_outerCircle1 = 0.1080f;
            const float R_outerCircle2 = 0.1140f;

            const glm::vec3 c_wood1(0.95f, 0.88f, 0.74f);
            const glm::vec3 c_wood2(0.89f, 0.79f, 0.63f);
            const glm::vec3 c_black(0.08f, 0.08f, 0.10f);
            const glm::vec3 c_red  (0.85f, 0.13f, 0.13f);

            const float aa = 1.0f / (float)W;
            const float halfThick = 0.0016f;

            auto distSeg = [](float px, float py, float ax, float ay, float bx, float by) -> float {
                float dx = bx - ax;
                float dy = by - ay;
                float l2 = dx * dx + dy * dy;
                if (l2 < 1e-7f) return std::hypot(px - ax, py - ay);
                float t = std::clamp(((px - ax) * dx + (py - ay) * dy) / l2, 0.0f, 1.0f);
                return std::hypot(px - (ax + t * dx), py - (ay + t * dy));
            };

            auto drawStroke = [&](glm::vec3& dst, float dist, float radius, const glm::vec3& col) {
                float cov = 1.0f - glm::smoothstep(radius - aa, radius + aa, dist);
                if (cov > 0.0f)
                    dst = glm::mix(dst, col, cov);
            };

            for (int y = 0; y < H; ++y)
            {
                float v = (float)y / (float)H;
                float ay_rel = std::abs(v - 0.5f);

                for (int x = 0; x < W; ++x)
                {
                    float u = (float)x / (float)W;
                    float ax_rel = std::abs(u - 0.5f);
                    float r = std::hypot(u - 0.5f, v - 0.5f);
                    float theta = std::atan2(v - 0.5f, u - 0.5f);

                    // 1. Natural blonde birch wood grain
                    float n1 = FBM(u * 50.0f, v * 5.0f, 4, 2.0f, 0.5f);
                    float n2 = FBM(u * 120.0f, v * 12.0f, 3, 2.0f, 0.5f);
                    float grain = n1 * 0.75f + n2 * 0.25f;
                    float rings = 0.5f + 0.5f * std::sin(grain * 14.0f + u * 4.0f);
                    float t_wood = std::clamp(rings * 0.22f + grain * 0.35f, 0.0f, 1.0f);
                    glm::vec3 col = glm::mix(c_wood1, c_wood2, t_wood);

                    // Frame edge contact shadow
                    float b_dist = std::max(ax_rel, ay_rel);
                    if (b_dist > 0.415f)
                    {
                        float shadow = std::min(1.0f, (b_dist - 0.415f) / 0.025f);
                        col *= (1.0f - 0.20f * shadow);
                    }

                    // 2. Outer boundary square (framed border)
                    if (ax_rel <= 0.415f + aa && ay_rel <= 0.415f + aa)
                    {
                        float dSqX = (ay_rel <= 0.415f) ? std::abs(ax_rel - 0.415f) : 1.0f;
                        float dSqY = (ax_rel <= 0.415f) ? std::abs(ay_rel - 0.415f) : 1.0f;
                        drawStroke(col, std::min(dSqX, dSqY), halfThick, c_black);
                    }

                    // 3. Pocket boundary arcs (pockets at +-1.2m, i.e. 0.40, 0.40 in UV)
                    if (ax_rel <= 0.415f && ay_rel <= 0.415f)
                    {
                        float dPock = std::hypot(ax_rel - 0.400f, ay_rel - 0.400f);
                        drawStroke(col, std::abs(dPock - 0.060f), halfThick, c_black);
                    }

                    // 4. Baselines (Striker lines on all 4 sides)
                    // Horizontal baselines (South & North)
                    if (ax_rel <= 0.250f + aa)
                    {
                        drawStroke(col, std::abs(ay_rel - D_inner), halfThick, c_black);
                        drawStroke(col, std::abs(ay_rel - D_outer), halfThick, c_black);
                    }
                    // Vertical baselines (West & East)
                    if (ay_rel <= 0.250f + aa)
                    {
                        drawStroke(col, std::abs(ax_rel - D_inner), halfThick, c_black);
                        drawStroke(col, std::abs(ax_rel - D_outer), halfThick, c_black);
                    }

                    // 5. Diagonal Foul Lines & Corner Arrows pointing toward pockets
                    float dPerp = std::abs(ax_rel - ay_rel) * 0.70710678f;
                    float sDiag = (ax_rel + ay_rel) * 0.70710678f;
                    if (sDiag >= 0.260f && sDiag <= 0.480f)
                    {
                        drawStroke(col, dPerp, halfThick, c_black);
                    }

                    // Arrowhead barbs and shaft tip pointing into pocket
                    const float tipX = 0.340f, tipY = 0.340f;
                    const float headX = 0.362f, headY = 0.362f;
                    float dBarb1 = distSeg(ax_rel, ay_rel, headX, headY, 0.345f, 0.365f);
                    float dBarb2 = distSeg(ax_rel, ay_rel, headX, headY, 0.365f, 0.345f);
                    float dShaftExt = distSeg(ax_rel, ay_rel, tipX, tipY, headX, headY);
                    drawStroke(col, std::min({dBarb1, dBarb2, dShaftExt}), halfThick, c_black);

                    // Arrow circle at base of head
                    float dTip = std::hypot(ax_rel - tipX, ay_rel - tipY);
                    if (dTip <= 0.0090f + aa)
                    {
                        float tipBorderCov = 1.0f - glm::smoothstep(0.0085f - aa, 0.0085f + aa, dTip);
                        float tipRedCov    = 1.0f - glm::smoothstep(0.0065f - aa, 0.0065f + aa, dTip);
                        col = glm::mix(col, c_black, tipBorderCov);
                        col = glm::mix(col, c_red, tipRedCov);
                    }

                    // 6. Base circles (8 red striker circles with black borders and dots)
                    float dC1 = std::hypot(ax_rel - 0.250f, ay_rel - D_base);
                    float dC2 = std::hypot(ax_rel - D_base, ay_rel - 0.250f);
                    float dBc = std::min(dC1, dC2);
                    if (dBc <= R_baseCircle + aa)
                    {
                        float bcBorderCov = 1.0f - glm::smoothstep(R_baseCircle - aa, R_baseCircle + aa, dBc);
                        float bcRedCov    = 1.0f - glm::smoothstep(R_baseCircle - 0.0020f - aa, R_baseCircle - 0.0020f + aa, dBc);
                        float bcDotCov    = 1.0f - glm::smoothstep(0.0035f - aa, 0.0035f + aa, dBc);
                        col = glm::mix(col, c_black, bcBorderCov);
                        col = glm::mix(col, c_red, bcRedCov);
                        col = glm::mix(col, c_black, bcDotCov);
                    }

                    // 7. Center Circles and Rosette
                    // Outer break circles (double ring)
                    drawStroke(col, std::abs(r - R_outerCircle1), halfThick, c_black);
                    drawStroke(col, std::abs(r - R_outerCircle2), halfThick, c_black);

                    // Outer circle tick marks (every 15 deg)
                    if (r > R_outerCircle1 - aa && r < R_outerCircle2 + aa)
                    {
                        const float step = 3.14159265f / 12.0f;
                        float phi = std::abs(std::fmod(theta + 3.14159265f + step * 0.5f, step) - step * 0.5f);
                        drawStroke(col, phi * r, halfThick * 0.85f, c_black);
                    }

                    // Inner circle
                    drawStroke(col, std::abs(r - R_innerCircle), halfThick, c_black);

                    // 6-petals rosette
                    const float petalDelta = 3.14159265f / 3.0f;
                    float secF = std::round(theta / petalDelta);
                    int sec = (int)secF;
                    float deltaTh = theta - secF * petalDelta;
                    float px = r * std::cos(deltaTh) - 0.0360f;
                    float py = r * std::sin(deltaTh);
                    float dPet = std::hypot(px, py);
                    drawStroke(col, std::abs(dPet - 0.0135f), halfThick, c_black);
                    if (dPet < 0.0135f + aa)
                    {
                        drawStroke(col, std::abs(dPet - 0.0080f), halfThick, c_black);
                        float innerPetCov = 1.0f - glm::smoothstep(0.0080f - aa, 0.0080f + aa, dPet);
                        glm::vec3 petalCol = (((sec % 2 + 2) % 2) == 0) ? c_red : c_black;
                        col = glm::mix(col, petalCol, innerPetCov);
                    }

                    // Queen circle (center solid red circle with black border and dot)
                    if (r <= R_queen + aa)
                    {
                        float qBorderCov = 1.0f - glm::smoothstep(R_queen - aa, R_queen + aa, r);
                        float qRedCov    = 1.0f - glm::smoothstep(R_queen - 0.0020f - aa, R_queen - 0.0020f + aa, r);
                        float qDotCov    = 1.0f - glm::smoothstep(0.0030f - aa, 0.0030f + aa, r);
                        col = glm::mix(col, c_black, qBorderCov);
                        col = glm::mix(col, c_red, qRedCov);
                        col = glm::mix(col, c_black, qDotCov);
                    }

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
