#include "CarromGame.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

// ============================================================
//  Construction
// ============================================================
CarromGame::CarromGame()
{
}

// ============================================================
//  Proximity detection
// ============================================================
bool CarromGame::IsNearBoard(const glm::vec3& camPos, const glm::vec3& center)
{
    // The player approaches the board from the south (positive Z side)
    glm::vec3 interactPt(center.x, center.y, center.z + 1.8f);
    float dist = glm::length(glm::vec2(camPos.x - interactPt.x, camPos.z - interactPt.z));
    return dist < 3.5f;
}

// ============================================================
//  Camera configuration
// ============================================================
glm::vec3 CarromGame::GetGameplayCameraPos() const
{
    // Positioned above and behind the player baseline (south), looking down at the board
    return glm::vec3(boardCenter.x, surfaceY + 2.85f, boardCenter.z + 2.95f);
}

glm::mat4 CarromGame::GetGameplayViewMatrix() const
{
    glm::vec3 eye = GetGameplayCameraPos();
    glm::vec3 target = glm::vec3(boardCenter.x, surfaceY, boardCenter.z);
    return glm::lookAt(eye, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

// ============================================================
//  Lifecycle
// ============================================================
void CarromGame::Start(int boardId, glm::vec3 center)
{
    activeBoardId = boardId;
    boardCenter = center;
    surfaceY = center.y + 0.05f; // Top of the 0.1-thick board base

    // Inner playing boundaries (accounting for border thickness)
    minX = center.x - 1.28f;
    maxX = center.x + 1.28f;
    minZ = center.z - 1.28f;
    maxZ = center.z + 1.28f;

    // Corner pockets at (+-1.2, +-1.2) from board center
    pockets.clear();
    pockets.push_back(glm::vec2(center.x - 1.20f, center.z - 1.20f)); // Top-Left
    pockets.push_back(glm::vec2(center.x + 1.20f, center.z - 1.20f)); // Top-Right
    pockets.push_back(glm::vec2(center.x - 1.20f, center.z + 1.20f)); // Bottom-Left
    pockets.push_back(glm::vec2(center.x + 1.20f, center.z + 1.20f)); // Bottom-Right

    // Baseline constraints along South side
    baselineZ = center.z + 0.95f;
    strikerMinX = center.x - 0.75f;
    strikerMaxX = center.x + 0.75f;

    aimDir = glm::vec2(0.0f, -1.0f); // Default forward aim (north toward center)
    aimTarget = glm::vec2(center.x, center.z);
    isCharging = false;
    chargeTime = 0.0f;
    feedbackMsg.clear();
    feedbackTimer = 0.0f;
    physicsAccumulator = 0.0f;

    resetBoard();
    state = CarromState::AIMING;
}

void CarromGame::Stop()
{
    state = CarromState::INACTIVE;
    activeBoardId = 0;
    isCharging = false;
    chargeTime = 0.0f;
    feedbackMsg.clear();
    discs.clear();
}

// ============================================================
//  Board initialization (19-coin classic break formation)
// ============================================================
void CarromGame::resetBoard()
{
    discs.clear();

    // Striker (index 0)
    CarromDisc striker;
    striker.pos = glm::vec2(boardCenter.x, baselineZ);
    striker.vel = glm::vec2(0.0f);
    striker.radius = 0.11f; // Visibly larger than 0.08 coins
    striker.mass = 2.5f;
    striker.type = DiscType::STRIKER;
    striker.isActive = true;
    striker.isPocketed = false;
    striker.sinkAnim = 0.0f;
    striker.sinkPocketPos = glm::vec2(0.0f);
    discs.push_back(striker);

    const float coinR = 0.08f;
    const float coinMass = 1.0f;

    // Center: Red Queen
    CarromDisc queen;
    queen.pos = glm::vec2(boardCenter.x, boardCenter.z);
    queen.vel = glm::vec2(0.0f);
    queen.radius = coinR;
    queen.mass = coinMass;
    queen.type = DiscType::QUEEN;
    queen.isActive = true;
    queen.isPocketed = false;
    queen.sinkAnim = 0.0f;
    queen.sinkPocketPos = glm::vec2(0.0f);
    discs.push_back(queen);

    // Inner ring (6 coins, alternating White / Black)
    const float r1 = 0.163f; // 2 * coinR + 3mm safety margin
    for (int i = 0; i < 6; ++i)
    {
        float angle = glm::radians(i * 60.0f);
        CarromDisc c;
        c.pos = glm::vec2(boardCenter.x + std::cos(angle) * r1,
                          boardCenter.z + std::sin(angle) * r1);
        c.vel = glm::vec2(0.0f);
        c.radius = coinR;
        c.mass = coinMass;
        c.type = (i % 2 == 0) ? DiscType::WHITE : DiscType::BLACK;
        c.isActive = true;
        c.isPocketed = false;
        c.sinkAnim = 0.0f;
        c.sinkPocketPos = glm::vec2(0.0f);
        discs.push_back(c);
    }

    // Outer ring (12 coins, alternating Black / White)
    const float r2Vertex = 0.326f;
    const float r2Edge   = 0.2823f;
    for (int i = 0; i < 12; ++i)
    {
        float angle = glm::radians(i * 30.0f);
        float dist = (i % 2 == 0) ? r2Vertex : r2Edge;
        CarromDisc c;
        c.pos = glm::vec2(boardCenter.x + std::cos(angle) * dist,
                          boardCenter.z + std::sin(angle) * dist);
        c.vel = glm::vec2(0.0f);
        c.radius = coinR;
        c.mass = coinMass;
        c.type = (i % 2 == 0) ? DiscType::BLACK : DiscType::WHITE;
        c.isActive = true;
        c.isPocketed = false;
        c.sinkAnim = 0.0f;
        c.sinkPocketPos = glm::vec2(0.0f);
        discs.push_back(c);
    }
}

// ============================================================
//  Reset striker back to baseline after a shot
// ============================================================
void CarromGame::resetStriker()
{
    if (discs.empty()) return;

    discs[0].pos = glm::vec2(boardCenter.x, baselineZ);
    discs[0].vel = glm::vec2(0.0f);
    discs[0].isActive = true;
    discs[0].isPocketed = false;
    discs[0].sinkAnim = 0.0f;

    // Check if striker overlaps any coin resting on the baseline, and nudge if needed
    for (size_t i = 1; i < discs.size(); ++i)
    {
        if (!discs[i].isActive || discs[i].isPocketed) continue;
        float minDist = discs[0].radius + discs[i].radius + 0.02f;
        while (glm::length(discs[0].pos - discs[i].pos) < minDist && discs[0].pos.x < strikerMaxX)
        {
            discs[0].pos.x += 0.05f;
        }
    }
    discs[0].pos.x = std::clamp(discs[0].pos.x, strikerMinX, strikerMaxX);
}

// ============================================================
//  Controls
// ============================================================
void CarromGame::MoveStriker(float dir, float dt)
{
    if (state != CarromState::AIMING || discs.empty()) return;

    discs[0].pos.x += dir * 2.2f * dt;
    discs[0].pos.x = std::clamp(discs[0].pos.x, strikerMinX, strikerMaxX);

    // Update aim direction relative to new striker position
    glm::vec2 delta = aimTarget - discs[0].pos;
    if (glm::length(delta) > 0.05f)
    {
        aimDir = glm::normalize(delta);
    }
}

void CarromGame::HandleMouseMove(double xpos, double ypos, int scrW, int scrH,
                                const glm::mat4& view, const glm::mat4& proj)
{
    if (state != CarromState::AIMING || discs.empty()) return;

    // Unproject mouse cursor into world space ray
    float xndc = (2.0f * (float)xpos) / (float)scrW - 1.0f;
    float yndc = 1.0f - (2.0f * (float)ypos) / (float)scrH;

    glm::mat4 invVP = glm::inverse(proj * view);
    glm::vec4 pNear = invVP * glm::vec4(xndc, yndc, -1.0f, 1.0f);
    pNear /= pNear.w;
    glm::vec4 pFar  = invVP * glm::vec4(xndc, yndc,  1.0f, 1.0f);
    pFar /= pFar.w;

    glm::vec3 r0 = glm::vec3(pNear);
    glm::vec3 rd = glm::normalize(glm::vec3(pFar - pNear));

    // Intersect ray with horizontal playing surface Y = surfaceY
    if (std::abs(rd.y) > 1e-5f)
    {
        float t = (surfaceY - r0.y) / rd.y;
        if (t > 0.0f)
        {
            glm::vec3 hit = r0 + t * rd;
            aimTarget = glm::vec2(hit.x, hit.z);

            glm::vec2 delta = aimTarget - discs[0].pos;
            if (glm::length(delta) > 0.05f)
            {
                aimDir = glm::normalize(delta);
            }
        }
    }
}

void CarromGame::HandleMouseButton(int button, int action)
{
    if (state != CarromState::AIMING || discs.empty()) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            isCharging = true;
            chargeTime = 0.0f;
        }
        else if (action == GLFW_RELEASE && isCharging)
        {
            isCharging = false;
            float fraction = std::clamp(chargeTime / CHARGE_DURATION, 0.0f, 1.0f);
            float power = MIN_POWER + (MAX_POWER - MIN_POWER) * fraction;

            // Launch striker
            discs[0].vel = aimDir * power;
            state = CarromState::SHOT_IN_PROGRESS;
            feedbackMsg = "Shot in progress...";
            feedbackTimer = 0.0f;
        }
    }
}

// ============================================================
//  Simulation Update
// ============================================================
void CarromGame::Update(float dt)
{
    if (state == CarromState::INACTIVE) return;

    dt = std::min(dt, 0.05f);

    if (feedbackTimer > 0.0f)
    {
        feedbackTimer -= dt;
        if (feedbackTimer <= 0.0f) feedbackMsg.clear();
    }

    if (state == CarromState::AIMING && isCharging)
    {
        chargeTime += dt;
        if (chargeTime > CHARGE_DURATION) chargeTime = CHARGE_DURATION;
    }

    if (state == CarromState::RESETTING)
    {
        resetTimer -= dt;
        if (resetTimer <= 0.0f)
        {
            resetBoard();
            state = CarromState::AIMING;
            feedbackMsg.clear();
        }
    }

    // Fixed timestep physics update (120 Hz)
    physicsAccumulator += dt;
    const float FIXED_DT = 1.0f / 120.0f;
    int maxSteps = 4;
    while (physicsAccumulator >= FIXED_DT && maxSteps-- > 0)
    {
        physicsSubstep(FIXED_DT);
        physicsAccumulator -= FIXED_DT;
    }

    // Check shot resolution
    if (state == CarromState::SHOT_IN_PROGRESS)
    {
        if (isBoardStationary())
        {
            // Count remaining active coins (excluding striker)
            int activeCoins = 0;
            for (size_t i = 1; i < discs.size(); ++i)
            {
                if (discs[i].isActive && !discs[i].isPocketed)
                    activeCoins++;
            }

            if (activeCoins == 0)
            {
                feedbackMsg = "Board cleared!";
                feedbackTimer = 2.5f;
                state = CarromState::RESETTING;
                resetTimer = 2.5f;
            }
            else
            {
                if (discs[0].isPocketed)
                {
                    feedbackMsg = "Striker pocketed - resetting";
                    feedbackTimer = 2.0f;
                }
                else
                {
                    feedbackMsg.clear();
                }

                resetStriker();
                state = CarromState::AIMING;
            }
        }
    }
}

void CarromGame::physicsSubstep(float dt)
{
    // 1. Move discs and apply Coulomb sliding friction
    for (auto& d : discs)
    {
        if (!d.isActive) continue;

        if (d.isPocketed)
        {
            // Sinking animation: translate toward pocket center and sink downward
            d.sinkAnim += dt / 0.25f;
            if (d.sinkAnim >= 1.0f)
            {
                d.sinkAnim = 1.0f;
                if (d.type != DiscType::STRIKER)
                {
                    d.isActive = false; // Remove pocketed coin from active physics
                }
            }
            continue;
        }

        d.pos += d.vel * dt;

        // Sliding friction
        float speed = glm::length(d.vel);
        if (speed > 0.0f)
        {
            float decel = 1.9f * dt;
            if (speed <= decel || speed < 0.03f)
            {
                d.vel = glm::vec2(0.0f);
            }
            else
            {
                d.vel = glm::normalize(d.vel) * (speed - decel);
                d.vel *= std::pow(0.992f, dt * 60.0f); // slight viscous damping
            }
        }
    }

    // 2. Corner pocket capture check
    const float captureRadius = 0.165f;
    for (auto& d : discs)
    {
        if (!d.isActive || d.isPocketed) continue;

        for (const auto& pocket : pockets)
        {
            if (glm::length(d.pos - pocket) < captureRadius)
            {
                d.isPocketed = true;
                d.sinkAnim = 0.0f;
                d.sinkPocketPos = pocket;
                d.vel = glm::vec2(0.0f);
                break;
            }
        }
    }

    // 3. Resolve disc-to-border and disc-to-disc collisions
    resolveCollisions();
}

void CarromGame::resolveCollisions()
{
    const float borderRest = 0.75f;
    const float discRest   = 0.88f;

    // Run 4 relaxation iterations for stability in dense coin clusters
    for (int iter = 0; iter < 4; ++iter)
    {
        // Disc-to-border collision
        for (auto& d : discs)
        {
            if (!d.isActive || d.isPocketed) continue;

            float r = d.radius;
            if (d.pos.x - r < minX)
            {
                d.pos.x = minX + r;
                if (d.vel.x < 0.0f) d.vel.x = -d.vel.x * borderRest;
            }
            else if (d.pos.x + r > maxX)
            {
                d.pos.x = maxX - r;
                if (d.vel.x > 0.0f) d.vel.x = -d.vel.x * borderRest;
            }

            if (d.pos.y - r < minZ)
            {
                d.pos.y = minZ + r;
                if (d.vel.y < 0.0f) d.vel.y = -d.vel.y * borderRest;
            }
            else if (d.pos.y + r > maxZ)
            {
                d.pos.y = maxZ - r;
                if (d.vel.y > 0.0f) d.vel.y = -d.vel.y * borderRest;
            }
        }

        // Disc-to-disc collision
        for (size_t i = 0; i < discs.size(); ++i)
        {
            for (size_t j = i + 1; j < discs.size(); ++j)
            {
                auto& a = discs[i];
                auto& b = discs[j];
                if (!a.isActive || !b.isActive || a.isPocketed || b.isPocketed) continue;

                glm::vec2 delta = b.pos - a.pos;
                float distSq = glm::dot(delta, delta);
                float minDist = a.radius + b.radius;

                if (distSq < minDist * minDist && distSq > 1e-8f)
                {
                    float dist = std::sqrt(distSq);
                    glm::vec2 normal = delta / dist;
                    float penetration = minDist - dist;

                    float invMassA = 1.0f / a.mass;
                    float invMassB = 1.0f / b.mass;
                    float totalInvMass = invMassA + invMassB;

                    // Positional separation
                    a.pos -= normal * (penetration * (invMassA / totalInvMass) * 0.85f);
                    b.pos += normal * (penetration * (invMassB / totalInvMass) * 0.85f);

                    // Impulse response
                    glm::vec2 relVel = a.vel - b.vel;
                    float velAlongNormal = glm::dot(relVel, normal);
                    if (velAlongNormal > 0.0f)
                    {
                        float impulseMag = (1.0f + discRest) * velAlongNormal / totalInvMass;
                        glm::vec2 impulse = normal * impulseMag;
                        a.vel -= impulse * invMassA;
                        b.vel += impulse * invMassB;
                    }
                }
            }
        }
    }
}

bool CarromGame::isBoardStationary() const
{
    for (const auto& d : discs)
    {
        if (!d.isActive) continue;
        if (glm::length(d.vel) > 0.015f) return false;
        if (d.isPocketed && d.sinkAnim < 1.0f) return false;
    }
    return true;
}

// ============================================================
//  Rendering
// ============================================================
void CarromGame::drawMeshRaw(Shader& shader, Mesh& mesh,
                             const glm::mat4& model, const Material& mat) const
{
    shader.setMat4("model", model);
    shader.setVec3("materialBaseColor", mat.baseColor);
    shader.setFloat("materialShininess", mat.shininess);
    shader.setFloat("materialSpecularStrength", mat.specularStrength);
    shader.setVec3("materialEmission", mat.emission);
    shader.setInt("useDiffuseTexture", mat.diffuseTex != 0 ? 1 : 0);
    shader.setFloat("materialTexScale", mat.texScale);
    if (mat.diffuseTex != 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mat.diffuseTex);
        shader.setInt("diffuseTexture", 0);
    }
    mesh.Draw();
}

void CarromGame::DrawDynamic(Shader& phongShader, Shader& lightShader, Mesh& cylinder) const
{
    if (state == CarromState::INACTIVE) return;

    auto strikerMat = Material::Make(glm::vec3(0.95f, 0.18f, 0.12f), 96.0f, 0.80f);
    auto queenMat   = Material::Make(glm::vec3(0.88f, 0.12f, 0.14f), 64.0f, 0.60f);
    auto whiteMat   = Material::Make(glm::vec3(0.95f, 0.92f, 0.82f), 64.0f, 0.50f);
    auto blackMat   = Material::Make(glm::vec3(0.12f, 0.12f, 0.14f), 64.0f, 0.40f);

    // 1. Draw all simulated discs
    for (const auto& d : discs)
    {
        if (!d.isActive) continue;

        glm::vec3 pos3D;
        float heightScale = 0.05f;
        float diameter = d.radius * 2.0f;

        if (d.type == DiscType::STRIKER)
        {
            heightScale = 0.06f;
        }

        if (d.isPocketed)
        {
            // Sinking animation: blend toward pocket center and drop Y
            glm::vec2 renderPos = glm::mix(d.pos, d.sinkPocketPos, d.sinkAnim);
            float renderY = surfaceY + heightScale * 0.5f - d.sinkAnim * 0.22f;
            pos3D = glm::vec3(renderPos.x, renderY, renderPos.y);
            diameter *= (1.0f - d.sinkAnim * 0.35f);
        }
        else
        {
            pos3D = glm::vec3(d.pos.x, surfaceY + heightScale * 0.5f, d.pos.y);
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos3D);
        model = glm::scale(model, glm::vec3(diameter, heightScale, diameter));

        const Material* mat = &whiteMat;
        if (d.type == DiscType::STRIKER) mat = &strikerMat;
        else if (d.type == DiscType::QUEEN) mat = &queenMat;
        else if (d.type == DiscType::BLACK) mat = &blackMat;

        drawMeshRaw(phongShader, cylinder, model, *mat);
    }

    // 2. Draw aiming guide and target marker (when in AIMING state)
    if (state == CarromState::AIMING && !discs.empty() && discs[0].isActive)
    {
        lightShader.use();
        lightShader.setVec3("lightColor", glm::vec3(1.0f, 0.85f, 0.2f)); // luminous golden-yellow

        // Series of glowing dots along aim vector
        for (int i = 1; i <= 8; ++i)
        {
            float dist = i * 0.11f;
            glm::vec2 dotPos = discs[0].pos + aimDir * dist;

            // Stop guide at border
            if (dotPos.x < minX || dotPos.x > maxX || dotPos.y < minZ || dotPos.y > maxZ)
                break;

            glm::mat4 dotModel = glm::mat4(1.0f);
            dotModel = glm::translate(dotModel, glm::vec3(dotPos.x, surfaceY + 0.005f, dotPos.y));
            dotModel = glm::scale(dotModel, glm::vec3(0.024f, 0.006f, 0.024f));
            lightShader.setMat4("model", dotModel);
            cylinder.Draw();
        }

        // Target marker dot
        glm::mat4 targetModel = glm::mat4(1.0f);
        targetModel = glm::translate(targetModel, glm::vec3(aimTarget.x, surfaceY + 0.006f, aimTarget.y));
        targetModel = glm::scale(targetModel, glm::vec3(0.045f, 0.006f, 0.045f));
        lightShader.setMat4("model", targetModel);
        cylinder.Draw();

        // Switch back to phong shader
        phongShader.use();
    }
}

// ============================================================
//  HUD & Power Bar
// ============================================================
void CarromGame::DrawHUD(HudRenderer& hud, int scrW, int scrH) const
{
    if (state == CarromState::INACTIVE) return;

    hud.BeginFrame();

    // Controls instructions (top-left)
    hud.RenderText("A / D - Position Striker", 10.0f, 10.0f, 1.4f, glm::vec3(0.9f, 0.9f, 0.9f));
    hud.RenderText("Move Mouse - Aim", 10.0f, 26.0f, 1.4f, glm::vec3(0.9f, 0.9f, 0.9f));
    hud.RenderText("Hold Left Click - Charge Power", 10.0f, 42.0f, 1.4f, glm::vec3(0.9f, 0.9f, 0.9f));
    hud.RenderText("Release Left Click - Shoot", 10.0f, 58.0f, 1.4f, glm::vec3(0.9f, 0.9f, 0.9f));
    hud.RenderText("E / ESC - Exit to Room", 10.0f, 74.0f, 1.4f, glm::vec3(0.7f, 0.7f, 0.7f));

    // Power bar when charging
    if (state == CarromState::AIMING && isCharging)
    {
        float fraction = std::clamp(chargeTime / CHARGE_DURATION, 0.0f, 1.0f);
        int totalSegments = 20;
        int filled = (int)(fraction * totalSegments);

        std::string barStr = "POWER [";
        for (int i = 0; i < totalSegments; ++i)
        {
            if (i < filled) barStr += "=";
            else barStr += "-";
        }
        barStr += "] " + std::to_string((int)(fraction * 100.0f)) + "%";

        glm::vec3 barColor(0.2f + 0.8f * fraction, 1.0f - 0.4f * fraction, 0.2f);
        hud.RenderTextCentered(barStr, (float)scrH - 60.0f, 2.0f, barColor);
    }

    // Contextual message (center)
    if (!feedbackMsg.empty())
    {
        glm::vec3 col(0.9f, 0.9f, 0.9f);
        if (feedbackMsg == "Board cleared!")
            col = glm::vec3(0.4f, 1.0f, 0.4f);
        else if (feedbackMsg.find("Striker pocketed") != std::string::npos)
            col = glm::vec3(1.0f, 0.35f, 0.35f);

        hud.RenderTextCentered(feedbackMsg, (float)scrH * 0.45f, 2.2f, col);
    }

    hud.EndFrame();
}
