#ifndef TABLE_TENNIS_GAME_H
#define TABLE_TENNIS_GAME_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Primitives.h"
#include "Material.h"
#include "HudRenderer.h"
#include <string>

// ============================================================
//  Game states
// ============================================================
enum class GameState {
    EXPLORATION,
    PLAYING,
    RALLY_RESET
};

// ============================================================
//  Rally sub-states
// ============================================================
enum class RallyPhase {
    SERVE,          // ball being launched from AI side
    TO_PLAYER,      // ball traveling toward player (+X)
    TO_AI,          // ball traveling toward AI (-X)
    ENDED           // someone missed; waiting for reset
};

// ============================================================
//  TableTennisGame
// ============================================================
class TableTennisGame {
public:
    TableTennisGame();

    // --- Lifecycle ---
    void EnterGame();               // transition from EXPLORATION -> PLAYING
    void ExitGame();                // transition back to EXPLORATION
    void Update(float dt);          // per-frame simulation tick
    void PlayerSwing();             // triggered by SPACE

    // --- Queries ---
    GameState GetState() const { return state; }
    bool IsNearTable(const glm::vec3& camPos) const;

    // --- Rendering ---
    // Draws the dynamic ball and paddles (call AFTER the static scene).
    void DrawDynamic(Shader& shader, Mesh& cylinder, Mesh& sphere) const;
    // Draws HUD text (call after scene, before swap).
    void DrawHUD(HudRenderer& hud, const glm::vec3& camPos) const;

    // --- Camera ---
    glm::mat4 GetGameplayViewMatrix() const;

    // feedback message
    std::string feedbackMsg;
    float feedbackTimer = 0.0f;

private:
    GameState state = GameState::EXPLORATION;
    RallyPhase rallyPhase = RallyPhase::SERVE;

    // ---- Table geometry constants ----
    static constexpr float TABLE_Y      = 3.10f;  // playing surface height
    static constexpr float TABLE_HALF_X = 4.0f;
    static constexpr float TABLE_HALF_Z = 2.5f;
    static constexpr float NET_X        = 0.0f;
    static constexpr float NET_TOP_Y    = 3.56f;

    // ---- Ball ----
    glm::vec3 ballPos;
    glm::vec3 ballVel;
    static constexpr float BALL_RADIUS  = 0.05f;
    static constexpr float GRAVITY      = -9.8f;
    static constexpr float BOUNCE_COEFF = 0.75f;

    // ---- Player paddle (positive X end) ----
    glm::vec3 playerPaddlePos;
    float playerPaddleZ = 0.0f;       // lateral position along Z
    float playerSwingAngle = 0.0f;    // current swing rotation in degrees
    float playerSwingTimer = -1.0f;   // <0 = idle, >=0 = animating
    bool  playerHitRegistered = false;
    static constexpr float SWING_DURATION = 0.35f;
    static constexpr float PLAYER_PADDLE_X = 3.2f;

    // ---- AI paddle (negative X end) ----
    glm::vec3 aiPaddlePos;
    float aiTargetZ = 0.0f;
    float aiSwingAngle = 0.0f;
    float aiSwingTimer = -1.0f;
    bool  aiHitRegistered = false;
    static constexpr float AI_PADDLE_X = -3.2f;
    static constexpr float AI_SPEED = 4.5f;
    static constexpr float AI_REACTION_DELAY = 0.12f;
    float aiReactionClock = 0.0f;

    // ---- Rally reset ----
    float resetTimer = 0.0f;
    static constexpr float RESET_DELAY = 1.8f;

    // ---- Internal helpers ----
    void resetRally(bool aiServes);
    void updateBall(float dt);
    void updatePlayerPaddle(float dt);
    void updateAI(float dt);
    void checkPaddleBallCollision(bool isPlayer);
    void checkNetCollision();
    void checkOutOfBounds();
    float swingEasing(float t) const;

    // Draw a single paddle (blade + handle) at given transform
    void drawPaddle(Shader& shader, Mesh& cylinder,
                    glm::vec3 pos, float swingAngle,
                    const Material& bladeMat, const Material& handleMat,
                    bool facingPositiveX) const;
    void drawMeshRaw(Shader& shader, Mesh& mesh,
                     const glm::mat4& model, const Material& mat) const;
};

#endif
