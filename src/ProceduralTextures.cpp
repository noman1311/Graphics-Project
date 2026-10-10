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
    int ix = (int)x;
    int iy = (int)y;
    if (x < 0.0f && (float)ix != x) ix--;
    if (y < 0.0f && (float)iy != y) iy--;
    return Hash21Int(ix, iy);
}
static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float Smooth(float t) { return t * t * (3.0f - 2.0f * t); }
static inline float ValueNoise2D(float x, float y)
{
    int xi = (int)x;
    int yi = (int)y;
    if (x < 0.0f && (float)xi != x) xi--;
    if (y < 0.0f && (float)yi != y) yi--;
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

            #pragma omp parallel for schedule(static)
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
            #pragma omp parallel for schedule(static)
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
            #pragma omp parallel for schedule(static)
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
            #pragma omp parallel for schedule(static)
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
            #pragma omp parallel for schedule(static)
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
            #pragma omp parallel for schedule(static)
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
                if (l2 < 1e-7f) {
                    float qx = px - ax, qy = py - ay;
                    return std::sqrt(qx * qx + qy * qy);
                }
                float t = std::clamp(((px - ax) * dx + (py - ay) * dy) / l2, 0.0f, 1.0f);
                float qx = px - (ax + t * dx);
                float qy = py - (ay + t * dy);
                return std::sqrt(qx * qx + qy * qy);
            };

            auto drawStroke = [&](glm::vec3& dst, float dist, float radius, const glm::vec3& col) {
                float cov = 1.0f - glm::smoothstep(radius - aa, radius + aa, dist);
                if (cov > 0.0f)
                    dst = glm::mix(dst, col, cov);
            };

            #pragma omp parallel for schedule(static)
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
                    float grain = FBM(u * 50.0f, v * 5.0f, 3, 2.0f, 0.5f);
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

    Texture ChessBoardSurface(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "chess_surface", true,
            [](unsigned char* data, int W, int H)
        {
            const float grid_min = 0.075f;
            const float grid_max = 0.925f;
            const float grid_size = grid_max - grid_min;
            const float sq_size = grid_size / 8.0f;

            const glm::vec3 c_border1(0.33f, 0.16f, 0.09f);
            const glm::vec3 c_border2(0.22f, 0.10f, 0.05f);
            const glm::vec3 c_gold   (0.85f, 0.69f, 0.30f);
            const glm::vec3 c_inlayDark(0.14f, 0.07f, 0.04f);

            const glm::vec3 c_maple1 (0.93f, 0.86f, 0.72f);
            const glm::vec3 c_maple2 (0.88f, 0.80f, 0.65f);

            const glm::vec3 c_walnut1(0.35f, 0.19f, 0.11f);
            const glm::vec3 c_walnut2(0.23f, 0.12f, 0.06f);

            const float aa = 1.0f / (float)W;

            #pragma omp parallel for schedule(static)
            for (int y = 0; y < H; ++y)
            {
                float v = (float)y / (float)H;
                for (int x = 0; x < W; ++x)
                {
                    float u = (float)x / (float)W;
                    glm::vec3 col;

                    if (u >= grid_min && u <= grid_max && v >= grid_min && v <= grid_max)
                    {
                        int file_idx = std::clamp((int)((u - grid_min) / sq_size), 0, 7);
                        int rank_idx = std::clamp((int)((v - grid_min) / sq_size), 0, 7);
                        bool is_light = ((file_idx + rank_idx) % 2 == 1);

                        float sq_u = (u - (grid_min + file_idx * sq_size)) / sq_size;
                        float sq_v = (v - (grid_min + rank_idx * sq_size)) / sq_size;
                        float dist_edge = std::min({sq_u, 1.0f - sq_u, sq_v, 1.0f - sq_v}) * sq_size;

                        if (is_light)
                        {
                            // Fine curly maple grain
                            float g1 = FBM(u * 120.0f, v * 15.0f, 3, 2.0f, 0.5f);
                            float g2 = 0.5f + 0.5f * std::sin(u * 280.0f + v * 20.0f);
                            float t = std::clamp(g1 * 0.40f + g2 * 0.15f, 0.0f, 1.0f);
                            col = glm::mix(c_maple1, c_maple2, t);
                        }
                        else
                        {
                            // Deep rich walnut grain
                            float g1 = FBM(u * 12.0f, v * 160.0f, 4, 2.0f, 0.5f);
                            float g2 = 0.5f + 0.5f * std::sin(v * 320.0f + u * 25.0f);
                            float t = std::clamp(g1 * 0.45f + g2 * 0.18f, 0.0f, 1.0f);
                            col = glm::mix(c_walnut1, c_walnut2, t);
                        }

                        // Subtle veneer seams between adjacent squares
                        if (dist_edge < 0.0015f)
                        {
                            float seam = dist_edge / 0.0015f;
                            col *= (0.65f + 0.35f * seam);
                        }
                    }
                    else
                    {
                        // Rich polished rosewood border with grain
                        float g_border = FBM(u * 35.0f, v * 35.0f, 3, 2.0f, 0.5f);
                        col = glm::mix(c_border1, c_border2, g_border);

                        float d_u = std::max({grid_min - u, u - grid_max, 0.0f});
                        float d_v = std::max({grid_min - v, v - grid_max, 0.0f});
                        float d_to_grid = std::max(d_u, d_v);

                        // Brass / Boxwood Inlay stringing 1 (at 0.008 from grid)
                        float dLine1 = std::abs(d_to_grid - 0.008f);
                        if (dLine1 < 0.0026f)
                        {
                            float covGold = 1.0f - glm::smoothstep(0.0014f - aa, 0.0014f + aa, dLine1);
                            col = glm::mix(col, c_inlayDark, 1.0f);
                            col = glm::mix(col, c_gold, covGold);
                        }

                        // Brass / Boxwood Inlay stringing 2 (at 0.016 from grid)
                        float dLine2 = std::abs(d_to_grid - 0.016f);
                        if (dLine2 < 0.0022f)
                        {
                            float covGold = 1.0f - glm::smoothstep(0.0012f - aa, 0.0012f + aa, dLine2);
                            col = glm::mix(col, c_inlayDark, 1.0f);
                            col = glm::mix(col, c_gold, covGold);
                        }

                        // Outer border chamfer shadow
                        float edge_dist = std::min({u, 1.0f - u, v, 1.0f - v});
                        if (edge_dist < 0.025f)
                        {
                            float shadow = edge_dist / 0.025f;
                            col *= (0.50f + 0.50f * shadow);
                        }
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

    Texture ClockDial(int w, int h)
    {
        return Texture::CreateProcedural(w, h, "clock_dial", true,
            [](unsigned char* data, int W, int H)
        {
            const glm::vec3 c_dialCenter(0.985f, 0.980f, 0.965f);
            const glm::vec3 c_dialEdge  (0.935f, 0.920f, 0.885f);
            const glm::vec3 c_black     (0.06f, 0.06f, 0.08f);
            const glm::vec3 c_brass     (0.82f, 0.65f, 0.28f);

            const float aa = 1.0f / (float)W;

            // Stroke definitions for digits 0-9 in normalized box [-0.5..0.5, -0.5..0.5]
            struct Stroke2D { float x1, y1, x2, y2; };
            auto addDigitStrokes = [](char d, float bx, float by, float cw, float ch, std::vector<Stroke2D>& out) {
                auto addS = [&](float x1, float y1, float x2, float y2) {
                    out.push_back({bx + x1 * cw, by + y1 * ch, bx + x2 * cw, by + y2 * ch});
                };
                switch (d) {
                case '0':
                    addS(-0.35f, -0.50f,  0.35f, -0.50f);
                    addS( 0.35f, -0.50f,  0.35f,  0.50f);
                    addS( 0.35f,  0.50f, -0.35f,  0.50f);
                    addS(-0.35f,  0.50f, -0.35f, -0.50f);
                    break;
                case '1':
                    addS( 0.00f, -0.50f,  0.00f,  0.50f);
                    addS(-0.20f, -0.30f,  0.00f, -0.50f);
                    addS(-0.25f,  0.50f,  0.25f,  0.50f);
                    break;
                case '2':
                    addS(-0.35f, -0.50f,  0.35f, -0.50f);
                    addS( 0.35f, -0.50f,  0.35f,  0.00f);
                    addS( 0.35f,  0.00f, -0.35f,  0.00f);
                    addS(-0.35f,  0.00f, -0.35f,  0.50f);
                    addS(-0.35f,  0.50f,  0.35f,  0.50f);
                    break;
                case '3':
                    addS(-0.35f, -0.50f,  0.35f, -0.50f);
                    addS( 0.35f, -0.50f,  0.35f,  0.50f);
                    addS( 0.35f,  0.50f, -0.35f,  0.50f);
                    addS(-0.15f,  0.00f,  0.35f,  0.00f);
                    break;
                case '4':
                    addS(-0.35f, -0.50f, -0.35f,  0.00f);
                    addS(-0.35f,  0.00f,  0.35f,  0.00f);
                    addS( 0.20f, -0.50f,  0.20f,  0.50f);
                    break;
                case '5':
                    addS( 0.35f, -0.50f, -0.35f, -0.50f);
                    addS(-0.35f, -0.50f, -0.35f,  0.00f);
                    addS(-0.35f,  0.00f,  0.35f,  0.00f);
                    addS( 0.35f,  0.00f,  0.35f,  0.50f);
                    addS( 0.35f,  0.50f, -0.35f,  0.50f);
                    break;
                case '6':
                    addS( 0.35f, -0.50f, -0.35f, -0.50f);
                    addS(-0.35f, -0.50f, -0.35f,  0.50f);
                    addS(-0.35f,  0.50f,  0.35f,  0.50f);
                    addS( 0.35f,  0.50f,  0.35f,  0.00f);
                    addS( 0.35f,  0.00f, -0.35f,  0.00f);
                    break;
                case '7':
                    addS(-0.35f, -0.50f,  0.35f, -0.50f);
                    addS( 0.35f, -0.50f, -0.05f,  0.50f);
                    break;
                case '8':
                    addS(-0.35f, -0.50f,  0.35f, -0.50f);
                    addS( 0.35f, -0.50f,  0.35f,  0.50f);
                    addS( 0.35f,  0.50f, -0.35f,  0.50f);
                    addS(-0.35f,  0.50f, -0.35f, -0.50f);
                    addS(-0.35f,  0.00f,  0.35f,  0.00f);
                    break;
                case '9':
                    addS( 0.35f,  0.00f, -0.35f,  0.00f);
                    addS(-0.35f,  0.00f, -0.35f, -0.50f);
                    addS(-0.35f, -0.50f,  0.35f, -0.50f);
                    addS( 0.35f, -0.50f,  0.35f,  0.50f);
                    addS( 0.35f,  0.50f, -0.35f,  0.50f);
                    break;
                }
            };

            // Precompute strokes for hours 1 through 12 grouped into glyphs with bounding boxes
            const float r_num = 0.355f;
            const float char_w = 0.040f;
            const float char_h = 0.058f;
            const float spacing = 0.046f;

            struct NumeralGlyph {
                float minX, maxX, minY, maxY;
                std::vector<Stroke2D> strokes;
            };
            std::vector<NumeralGlyph> numeralGlyphs;

            for (int h = 1; h <= 12; ++h)
            {
                float ang = (float)h * (3.14159265f / 6.0f) - 3.14159265f * 0.5f;
                float cx = r_num * std::cos(ang);
                float cy = r_num * std::sin(ang);

                std::string s = std::to_string(h);
                float total_w = (float)s.length() * spacing;
                float start_x = cx - total_w * 0.5f + spacing * 0.5f;

                NumeralGlyph g;
                g.minX = 999.0f; g.maxX = -999.0f;
                g.minY = 999.0f; g.maxY = -999.0f;

                for (size_t i = 0; i < s.length(); ++i)
                {
                    float bx = start_x + (float)i * spacing;
                    float by = cy;
                    addDigitStrokes(s[i], bx, by, char_w, char_h, g.strokes);
                }

                for (const auto& st : g.strokes)
                {
                    g.minX = std::min({g.minX, st.x1, st.x2});
                    g.maxX = std::max({g.maxX, st.x1, st.x2});
                    g.minY = std::min({g.minY, st.y1, st.y2});
                    g.maxY = std::max({g.maxY, st.y1, st.y2});
                }
                g.minX -= 0.008f;
                g.maxX += 0.008f;
                g.minY -= 0.008f;
                g.maxY += 0.008f;
                numeralGlyphs.push_back(g);
            }

            auto distSeg = [](float px, float py, float ax, float ay, float bx, float by) -> float {
                float dx = bx - ax;
                float dy = by - ay;
                float l2 = dx * dx + dy * dy;
                if (l2 < 1e-7f) {
                    float qx = px - ax, qy = py - ay;
                    return std::sqrt(qx * qx + qy * qy);
                }
                float t = std::clamp(((px - ax) * dx + (py - ay) * dy) / l2, 0.0f, 1.0f);
                float qx = px - (ax + t * dx);
                float qy = py - (ay + t * dy);
                return std::sqrt(qx * qx + qy * qy);
            };

            #pragma omp parallel for schedule(static)
            for (int y = 0; y < H; ++y)
            {
                float v = (float)y / (float)H;
                float y_rel = v - 0.5f;

                for (int x = 0; x < W; ++x)
                {
                    float u = (float)x / (float)W;
                    float x_rel = u - 0.5f;

                    float r = std::hypot(x_rel, y_rel);
                    float theta = std::atan2(y_rel, x_rel);

                    float t_r = std::clamp(r / 0.48f, 0.0f, 1.0f);
                    glm::vec3 col = glm::mix(c_dialCenter, c_dialEdge, t_r);

                    // Outer perimeter shadow
                    if (r > 0.470f)
                    {
                        float shadow = std::min(1.0f, (r - 0.470f) / 0.025f);
                        col *= (1.0f - 0.45f * shadow);
                    }

                    // Outer track ring
                    float dOuter = std::abs(r - 0.468f);
                    if (dOuter < 0.0022f + aa)
                    {
                        float cov = 1.0f - glm::smoothstep(0.0016f - aa, 0.0016f + aa, dOuter);
                        col = glm::mix(col, c_black, cov);
                    }

                    // Inner railway track ring
                    float dTrack = std::abs(r - 0.442f);
                    if (dTrack < 0.0020f + aa)
                    {
                        float cov = 1.0f - glm::smoothstep(0.0014f - aa, 0.0014f + aa, dTrack);
                        col = glm::mix(col, c_black, cov);
                    }

                    // 60-Minute tick marks between 0.442 and 0.468
                    if (r >= 0.442f - aa && r <= 0.468f + aa)
                    {
                        const float step_min = 3.14159265f / 30.0f; // 6 degrees
                        float phi = std::abs(std::fmod(theta + 3.14159265f + step_min * 0.5f, step_min) - step_min * 0.5f);
                        float d_tick = phi * r;
                        if (d_tick < 0.0016f + aa)
                        {
                            float cov = 1.0f - glm::smoothstep(0.0011f - aa, 0.0011f + aa, d_tick);
                            col = glm::mix(col, c_black, cov);
                        }
                    }

                    // 12 Hour Batons outside the numbers (from 0.418 to 0.442)
                    const float step_hour = 3.14159265f / 6.0f; // 30 degrees
                    float phi_h = std::abs(std::fmod(theta + 3.14159265f + step_hour * 0.5f, step_hour) - step_hour * 0.5f);
                    if (r >= 0.418f - aa && r <= 0.442f + aa)
                    {
                        float d_hour = phi_h * r;
                        if (d_hour < 0.0036f + aa)
                        {
                            float cov = 1.0f - glm::smoothstep(0.0028f - aa, 0.0028f + aa, d_hour);
                            col = glm::mix(col, c_black, cov);
                        }
                    }

                    // Vector Hour Numerals (1 through 12) between r = 0.27 and 0.42
                    if (r >= 0.27f && r <= 0.42f)
                    {
                        float min_d = 999.0f;
                        for (const auto& g : numeralGlyphs)
                        {
                            if (x_rel < g.minX || x_rel > g.maxX || y_rel < g.minY || y_rel > g.maxY)
                                continue;
                            for (const auto& s : g.strokes)
                            {
                                float d = distSeg(x_rel, y_rel, s.x1, s.y1, s.x2, s.y2);
                                if (d < min_d) min_d = d;
                            }
                        }
                        if (min_d < 0.0036f + aa)
                        {
                            float covNum = 1.0f - glm::smoothstep(0.0030f - aa, 0.0030f + aa, min_d);
                            col = glm::mix(col, c_black, covNum);
                        }
                    }

                    // Inner chapter ring at r = 0.240
                    float dInner = std::abs(r - 0.240f);
                    if (dInner < 0.0018f + aa)
                    {
                        float cov = 1.0f - glm::smoothstep(0.0012f - aa, 0.0012f + aa, dInner);
                        col = glm::mix(col, c_black, cov);
                    }

                    // Center brass accent ring at r = 0.035
                    float dCenter = std::abs(r - 0.035f);
                    if (dCenter < 0.0020f + aa)
                    {
                        float cov = 1.0f - glm::smoothstep(0.0014f - aa, 0.0014f + aa, dCenter);
                        col = glm::mix(col, c_brass, cov);
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
            #pragma omp parallel for schedule(static)
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
            #pragma omp parallel for schedule(static)
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
            #pragma omp parallel for schedule(static)
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
