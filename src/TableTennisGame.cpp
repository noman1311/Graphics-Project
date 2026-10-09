#include "TableTennisGame.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

// ============================================================
//  Construction
// ============================================================
TableTennisGame::TableTennisGame()
{
    ballPos   = glm::vec3(0.5f, TABLE_Y + BALL_RADIUS, 0.5f);
    ballVel   = glm::vec3(0.0f);
    playerPaddlePos = glm::vec3(PLAYER_PADDLE_X, TABLE_Y + 0.15f, 0.0f);
    aiPaddlePos     = glm::vec3(AI_PADDLE_X,     TABLE_Y + 0.15f, 0.0f);
}

// ============================================================
//  Proximity detection
// ============================================================
bool TableTennisGame::IsNearTable(const glm::vec3& camPos) const
{
    // Interaction point: positive-X end of the table, slightly in front.
    glm::vec3 interactPt(5.5f, TABLE_Y, 0.0f);
    float dist = glm::length(glm::vec3(camPos.x, 0.0f, camPos.z) -
                             glm::vec3(interactPt.x, 0.0f, interactPt.z));
    return dist < 4.0f;
}

// ============================================================
//  Gameplay camera
// ============================================================
glm::mat4 TableTennisGame::GetGameplayViewMatrix() const
{
    glm::vec3 eye(7.0f, 5.5f, 0.0f);
    glm::vec3 target(0.0f, 3.2f, 0.0f);
    return glm::lookAt(eye, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

// ============================================================
//  State transitions
// ============================================================
void TableTennisGame::EnterGame()
{
    state = GameState::PLAYING;
    resetRally(true);  // AI serves first
    feedbackMsg.clear();
    feedbackTimer = 0.0f;
}

void TableTennisGame::ExitGame()
{
    state = GameState::EXPLORATION;
    // Put things back to resting positions
    ballPos  = glm::vec3(0.5f, TABLE_Y + BALL_RADIUS, 0.5f);
    ballVel  = glm::vec3(0.0f);
    playerSwingTimer = -1.0f;
    playerSwingAngle = 0.0f;
    aiSwingTimer = -1.0f;
    aiSwingAngle = 0.0f;
    playerPaddlePos = glm::vec3(PLAYER_PADDLE_X, TABLE_Y + 0.15f, 0.0f);
    aiPaddlePos     = glm::vec3(AI_PADDLE_X,     TABLE_Y + 0.15f, 0.0f);
}

// ============================================================
//  Rally reset
// ============================================================
void TableTennisGame::resetRally(bool aiServes)
{
    playerPaddlePos = glm::vec3(PLAYER_PADDLE_X, TABLE_Y + 0.15f, 0.0f);
    aiPaddlePos     = glm::vec3(AI_PADDLE_X,     TABLE_Y + 0.15f, 0.0f);
    playerSwingTimer = -1.0f;
    playerSwingAngle = 0.0f;
    aiSwingTimer     = -1.0f;
    aiSwingAngle     = 0.0f;
    playerHitRegistered = false;
    aiHitRegistered     = false;
    aiReactionClock = 0.0f;

    if (aiServes)
    {
        // Ball starts on AI side, hit toward player
        ballPos = glm::vec3(AI_PADDLE_X + 0.3f, TABLE_Y + 0.35f, 0.0f);
        ballVel = glm::vec3(5.8f, 3.35f, 0.2f);
        rallyPhase = RallyPhase::TO_PLAYER;
    }
    else
    {
        // Player serves
        ballPos = glm::vec3(PLAYER_PADDLE_X - 0.3f, TABLE_Y + 0.35f, 0.0f);
        ballVel = glm::vec3(-5.8f, 3.35f, -0.2f);
        rallyPhase = RallyPhase::TO_AI;
    }
}

// ============================================================
//  Swing easing: 0..1 -> angle multiplier
//  Goes: 0 -> backswing -> forward -> follow-through -> 0
// ============================================================
float TableTennisGame::swingEasing(float t) const
{
    // t in [0, 1]:
    //   [0.0, 0.2)  backswing:        angle goes 0 -> -15 degrees
    //   [0.2, 0.5)  forward strike:   angle goes -15 -> +45 degrees
    //   [0.5, 1.0]  follow-through:   angle goes +45 -> 0 degrees
    if (t < 0.2f)
    {
        float s = t / 0.2f;
        return -15.0f * s;
    }
    else if (t < 0.5f)
    {
        float s = (t - 0.2f) / 0.3f;
        return -15.0f + 60.0f * s;
    }
    else
    {
        float s = (t - 0.5f) / 0.5f;
        return 45.0f * (1.0f - s);
    }
}

// ============================================================
//  Player swing input
// ============================================================
void TableTennisGame::PlayerSwing()
{
    if (state != GameState::PLAYING) return;
    if (playerSwingTimer >= 0.0f) return; // already swinging
    playerSwingTimer = 0.0f;
    playerHitRegistered = false;
}

// ============================================================
//  Main update
// ============================================================
void TableTennisGame::Update(float dt)
{
    // Clamp large dt (window drag, breakpoint, etc.)
    dt = std::min(dt, 0.05f);

    if (feedbackTimer > 0.0f)
    {
        feedbackTimer -= dt;
        if (feedbackTimer <= 0.0f) feedbackMsg.clear();
    }

    if (state == GameState::RALLY_RESET)
    {
        resetTimer -= dt;
        if (resetTimer <= 0.0f)
        {
            state = GameState::PLAYING;
            resetRally(true); // AI always serves after a reset
        }
        return;
    }

    if (state != GameState::PLAYING) return;

    updateBall(dt);
    updatePlayerPaddle(dt);
    updateAI(dt);
    checkPaddleBallCollision(true);   // player
    checkPaddleBallCollision(false);  // AI
    checkNetCollision();
    checkOutOfBounds();
}

// ============================================================
//  Ball physics
// ============================================================
void TableTennisGame::updateBall(float dt)
{
    // Gravity
    ballVel.y += GRAVITY * dt;

    // Integrate position
    ballPos += ballVel * dt;

    // Table bounce: ball descending, bottom of ball touches table surface,
    // and ball is within the table XZ rectangle.
    float ballBottom = ballPos.y - BALL_RADIUS;
    if (ballVel.y < 0.0f && ballBottom <= TABLE_Y &&
        ballPos.x >= -TABLE_HALF_X && ballPos.x <= TABLE_HALF_X &&
        ballPos.z >= -TABLE_HALF_Z && ballPos.z <= TABLE_HALF_Z)
    {
        ballPos.y = TABLE_Y + BALL_RADIUS;
        ballVel.y = -ballVel.y * BOUNCE_COEFF;
        // Prevent micro-bouncing
        if (std::abs(ballVel.y) < 0.3f) ballVel.y = 0.3f;
    }
}

// ============================================================
//  Net collision
// ============================================================
void TableTennisGame::checkNetCollision()
{
    // Net is at X=0, from Z = -TABLE_HALF_Z to +TABLE_HALF_Z,
    // from Y = TABLE_Y to NET_TOP_Y.  Half-thickness ~ 0.05.
    float halfThick = 0.05f;

    // Only check if ball is near X=0
    if (std::abs(ballPos.x) > halfThick + BALL_RADIUS) return;

    // Check Y and Z bounds
    if (ballPos.y - BALL_RADIUS > NET_TOP_Y) return; // clears the net
    if (ballPos.y + BALL_RADIUS < TABLE_Y) return;
    if (ballPos.z < -TABLE_HALF_Z || ballPos.z > TABLE_HALF_Z) return;

    // The ball hit the net
    if (ballPos.y + BALL_RADIUS >= TABLE_Y && ballPos.y - BALL_RADIUS <= NET_TOP_Y)
    {
        // Reflect X velocity and dampen
        ballVel.x = -ballVel.x * 0.3f;
        ballVel.y *= 0.5f;
        // Push ball to whichever side it was approaching from
        if (ballPos.x >= 0.0f)
            ballPos.x = halfThick + BALL_RADIUS + 0.01f;
        else
            ballPos.x = -(halfThick + BALL_RADIUS + 0.01f);
    }
}

// ============================================================
//  Out of bounds
// ============================================================
void TableTennisGame::checkOutOfBounds()
{
    bool outOfBounds = false;

    // Ball fell below the table significantly
    if (ballPos.y < TABLE_Y - 2.0f) outOfBounds = true;

    // Ball went past the player end
    if (ballPos.x > TABLE_HALF_X + 3.0f) outOfBounds = true;

    // Ball went past the AI end
    if (ballPos.x < -TABLE_HALF_X - 3.0f) outOfBounds = true;

    // Ball flew way off to the sides
    if (std::abs(ballPos.z) > TABLE_HALF_Z + 4.0f) outOfBounds = true;

    // Ball bounced twice on the same side without crossing (simplified: if
    // ball has very low velocity and is below table+0.3 on one side)
    if (!outOfBounds && std::abs(ballVel.x) < 0.3f &&
        std::abs(ballVel.y) < 0.5f && ballPos.y < TABLE_Y + 0.2f)
    {
        outOfBounds = true;
    }

    if (outOfBounds)
    {
        rallyPhase = RallyPhase::ENDED;
        state = GameState::RALLY_RESET;
        resetTimer = RESET_DELAY;
        feedbackMsg = "Rally ended - restarting...";
        feedbackTimer = RESET_DELAY;
    }
}

// ============================================================
//  Player paddle update
// ============================================================
void TableTennisGame::updatePlayerPaddle(float dt)
{
    // The player paddle stays at a fixed X position.
    // Its Z can track the ball somewhat (auto-assist for a coursework demo).
    float targetZ = 0.0f;
    if (rallyPhase == RallyPhase::TO_PLAYER)
    {
        // Predict landing Z roughly
        targetZ = ballPos.z;
    }
    float maxMove = 5.0f * dt;
    float diff = targetZ - playerPaddlePos.z;
    if (std::abs(diff) > maxMove)
        playerPaddlePos.z += (diff > 0.0f ? maxMove : -maxMove);
    else
        playerPaddlePos.z = targetZ;

    // Clamp to table width
    playerPaddlePos.z = std::clamp(playerPaddlePos.z, -TABLE_HALF_Z + 0.3f, TABLE_HALF_Z - 0.3f);

    // Animate swing
    if (playerSwingTimer >= 0.0f)
    {
        playerSwingTimer += dt;
        float t = playerSwingTimer / SWING_DURATION;
        if (t >= 1.0f)
        {
            playerSwingTimer = -1.0f;
            playerSwingAngle = 0.0f;
        }
        else
        {
            playerSwingAngle = swingEasing(t);
        }
    }
}

// ============================================================
//  AI controller
// ============================================================
void TableTennisGame::updateAI(float dt)
{
    // AI paddle X is fixed.
    float targetZ = 0.0f;

    if (rallyPhase == RallyPhase::TO_AI && ballVel.x < 0.0f)
    {
        // Reaction delay
        aiReactionClock += dt;
        if (aiReactionClock >= AI_REACTION_DELAY)
        {
            // Simple prediction: estimate time for ball to reach AI_PADDLE_X
            float dx = AI_PADDLE_X - ballPos.x;
            if (ballVel.x != 0.0f)
            {
                float timeToArrive = dx / ballVel.x;
                if (timeToArrive > 0.0f)
                    targetZ = ballPos.z + ballVel.z * timeToArrive;
                else
                    targetZ = ballPos.z;
            }
            else
            {
                targetZ = ballPos.z;
            }
        }
    }

    float maxMove = AI_SPEED * dt;
    float diff = targetZ - aiPaddlePos.z;
    if (std::abs(diff) > maxMove)
        aiPaddlePos.z += (diff > 0.0f ? maxMove : -maxMove);
    else
        aiPaddlePos.z = targetZ;

    aiPaddlePos.z = std::clamp(aiPaddlePos.z, -TABLE_HALF_Z + 0.3f, TABLE_HALF_Z - 0.3f);

    // AI auto-swing when ball is close
    if (rallyPhase == RallyPhase::TO_AI && aiSwingTimer < 0.0f)
    {
        float distX = std::abs(ballPos.x - AI_PADDLE_X);
        if (distX < 0.8f && ballVel.x < 0.0f)
        {
            aiSwingTimer = 0.0f;
            aiHitRegistered = false;
        }
    }

    // Animate AI swing
    if (aiSwingTimer >= 0.0f)
    {
        aiSwingTimer += dt;
        float t = aiSwingTimer / SWING_DURATION;
        if (t >= 1.0f)
        {
            aiSwingTimer = -1.0f;
            aiSwingAngle = 0.0f;
        }
        else
        {
            aiSwingAngle = swingEasing(t);
        }
    }
}

// ============================================================
//  Paddle-ball collision
// ============================================================
void TableTennisGame::checkPaddleBallCollision(bool isPlayer)
{
    glm::vec3 paddlePos = isPlayer ? playerPaddlePos : aiPaddlePos;
    float swingTimer    = isPlayer ? playerSwingTimer : aiSwingTimer;
    bool& hitRegistered = isPlayer ? playerHitRegistered : aiHitRegistered;

    // Only collide when actively swinging, in the contact portion
    if (swingTimer < 0.0f) return;
    float t = swingTimer / SWING_DURATION;
    if (t < 0.15f || t > 0.65f) return;  // contact window
    if (hitRegistered) return;

    // Blade center is offset upward along Y from the handle base
    glm::vec3 bladeCenter = paddlePos + glm::vec3(0.0f, 0.22f, 0.0f);
    float dx = std::abs(ballPos.x - bladeCenter.x);
    float dy = std::abs(ballPos.y - bladeCenter.y);
    float dz = std::abs(ballPos.z - bladeCenter.z);

    if (dx < 0.55f && dy < 0.45f && dz < 0.45f)
    {
        hitRegistered = true;

        // Base return speed and lateral Z influence based on impact spot
        float speedOut = 5.8f;
        float lateralZ = (ballPos.z - paddlePos.z) * 1.8f;
        lateralZ = std::clamp(lateralZ, -1.2f, 1.2f);

        if (isPlayer)
        {
            // Ball arcs cleanly over net (-X direction) toward AI
            ballVel = glm::vec3(-speedOut, 3.35f, lateralZ);
            rallyPhase = RallyPhase::TO_AI;
            aiReactionClock = 0.0f; // reset AI reaction
            feedbackMsg = "Hit!";
            feedbackTimer = 0.8f;
        }
        else
        {
            // Ball arcs cleanly over net (+X direction) toward player
            ballVel = glm::vec3(speedOut, 3.35f, lateralZ);
            rallyPhase = RallyPhase::TO_PLAYER;
            feedbackMsg.clear();
        }

        // Ensure ball is cleanly above table surface
        if (ballPos.y < TABLE_Y + BALL_RADIUS + 0.05f)
            ballPos.y = TABLE_Y + BALL_RADIUS + 0.05f;
    }
    else if (isPlayer && t > 0.55f && !hitRegistered &&
             rallyPhase == RallyPhase::TO_PLAYER)
    {
        // Player missed — swing completed without hitting
        if (ballPos.x > 1.0f)
        {
            feedbackMsg = "Missed!";
            feedbackTimer = 0.8f;
        }
    }
}

// ============================================================
//  Drawing helpers
// ============================================================
void TableTennisGame::drawMeshRaw(Shader& shader, Mesh& mesh,
                                   const glm::mat4& model,
                                   const Material& mat) const
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

void TableTennisGame::drawPaddle(Shader& shader, Mesh& cylinder,
                                  glm::vec3 pos, float swingAngle,
                                  const Material& bladeMat,
                                  const Material& handleMat,
                                  bool facingPositiveX) const
{
    glm::mat4 base = glm::mat4(1.0f);
    base = glm::translate(base, pos);

    // Direction toward net: -X for player (+X end), +X for AI (-X end)
    float dir = facingPositiveX ? -1.0f : 1.0f;

    // Forward stroke displacement during swing
    float fwdOffset = 0.0f;
    if (swingAngle != 0.0f)
    {
        // swingAngle goes: 0 -> -15 (backswing) -> +45 (strike) -> 0 (follow through)
        fwdOffset = (swingAngle / 45.0f) * 0.22f * dir;
    }
    base = glm::translate(base, glm::vec3(fwdOffset, 0.0f, 0.0f));

    // Dynamic rotation around Z and Y for authentic stroke
    float angleRad = glm::radians(swingAngle);
    if (!facingPositiveX) angleRad = -angleRad;
    base = glm::rotate(base, angleRad * 0.8f, glm::vec3(0.0f, 0.0f, 1.0f));
    base = glm::rotate(base, angleRad * 0.4f * dir, glm::vec3(0.0f, 1.0f, 0.0f));

    // Blade: vertical circular disc facing along X axis toward the net
    {
        glm::mat4 bladeM = base;
        // Blade centered above handle
        bladeM = glm::translate(bladeM, glm::vec3(0.0f, 0.22f, 0.0f));
        // Rotate cylinder from Y-axis to X-axis so flat circular face is normal to X
        bladeM = glm::rotate(bladeM, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        bladeM = glm::scale(bladeM, glm::vec3(0.36f, 0.03f, 0.36f));
        drawMeshRaw(shader, cylinder, bladeM, bladeMat);
    }

    // Handle: wooden grip cylinder below the blade
    {
        glm::mat4 handleM = base;
        handleM = glm::translate(handleM, glm::vec3(0.0f, 0.04f, 0.0f));
        handleM = glm::scale(handleM, glm::vec3(0.06f, 0.22f, 0.06f));
        drawMeshRaw(shader, cylinder, handleM, handleMat);
    }
}

// ============================================================
//  Draw dynamic gameplay objects
// ============================================================
void TableTennisGame::DrawDynamic(Shader& shader, Mesh& cylinder,
                                   Mesh& sphere) const
{
    if (state == GameState::EXPLORATION) return;

    auto redBlade  = Material::Make(glm::vec3(0.85f, 0.12f, 0.12f), 32.0f, 0.35f);
    auto blueBlade = Material::Make(glm::vec3(0.12f, 0.25f, 0.85f), 32.0f, 0.35f);
    auto handleMat = Material::Make(glm::vec3(0.60f, 0.40f, 0.25f), 24.0f, 0.25f);
    auto ballMat   = Material::Make(glm::vec3(1.0f, 0.98f, 0.92f), 64.0f, 0.60f);

    // Player paddle (red)
    drawPaddle(shader, cylinder, playerPaddlePos, playerSwingAngle,
               redBlade, handleMat, true);

    // AI paddle (blue)
    drawPaddle(shader, cylinder, aiPaddlePos, aiSwingAngle,
               blueBlade, handleMat, false);

    // Ball
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, ballPos);
        model = glm::scale(model, glm::vec3(0.10f));
        drawMeshRaw(shader, sphere, model, ballMat);
    }
}

// ============================================================
//  HUD
// ============================================================
void TableTennisGame::DrawHUD(HudRenderer& hud, const glm::vec3& camPos) const
{
    hud.BeginFrame();

    if (state == GameState::EXPLORATION)
    {
        if (IsNearTable(camPos))
        {
            hud.RenderTextCentered("TABLE TENNIS", 200.0f, 3.0f,
                                    glm::vec3(1.0f, 0.95f, 0.7f));
            hud.RenderTextCentered("Press 1 to Play", 250.0f, 2.0f,
                                    glm::vec3(0.8f, 0.9f, 1.0f));
            hud.RenderTextCentered("Move closer or away to interact", 290.0f, 1.5f,
                                    glm::vec3(0.7f, 0.75f, 0.85f));
        }
    }
    else if (state == GameState::PLAYING || state == GameState::RALLY_RESET)
    {
        // Controls reminder (top-left)
        hud.RenderText("SPACE - Swing Paddle", 10.0f, 10.0f, 1.5f,
                        glm::vec3(0.9f, 0.9f, 0.9f));
        hud.RenderText("ESC / E - Exit to Room", 10.0f, 26.0f, 1.5f,
                        glm::vec3(0.7f, 0.7f, 0.7f));

        // Feedback message (center)
        if (!feedbackMsg.empty() && feedbackTimer > 0.0f)
        {
            glm::vec3 col(0.4f, 1.0f, 0.4f); // bright green for Hit!
            if (feedbackMsg == "Missed!")
                col = glm::vec3(1.0f, 0.3f, 0.3f); // red for Missed!
            else if (feedbackMsg.find("restarting") != std::string::npos)
                col = glm::vec3(0.85f, 0.85f, 0.85f); // white for restart

            hud.RenderTextCentered(feedbackMsg, 300.0f, 2.5f, col);
        }
    }

    hud.EndFrame();
}
