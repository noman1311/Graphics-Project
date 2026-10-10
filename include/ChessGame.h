#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Primitives.h"
#include "Material.h"
#include "HudRenderer.h"
#include <vector>
#include <string>

enum class PieceType {
    NONE = 0,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum class PieceColor {
    NONE = 0,
    WHITE,
    BLACK
};

struct ChessPiece {
    PieceType type = PieceType::NONE;
    PieceColor color = PieceColor::NONE;
    bool hasMoved = false;
};

struct ChessMove {
    int fromFile = 0;
    int fromRank = 0;
    int toFile = 0;
    int toRank = 0;
    PieceType promotion = PieceType::QUEEN;
    bool isCastling = false;
    bool isCapture = false;
};

enum class ChessGameState {
    INACTIVE,
    WHITE_TURN,
    BLACK_THINKING,
    CHECKMATE_WHITE_WINS,
    CHECKMATE_BLACK_WINS,
    STALEMATE
};

struct PieceAnim {
    bool active = false;
    ChessPiece piece;
    glm::vec3 startPos = glm::vec3(0.0f);
    glm::vec3 endPos = glm::vec3(0.0f);
    float elapsed = 0.0f;
    float duration = 0.28f;
};

class ChessGame {
public:
    ChessGame();

    // Lifecycle
    void Start();
    void Stop();
    void ResetGame();
    void Update(float dt);

    // Controls
    void HandleMouseMove(double xpos, double ypos, int scrW, int scrH,
                        const glm::mat4& view, const glm::mat4& proj);
    void HandleMouseButton(int button, int action);

    // Queries
    bool IsActive() const { return state != ChessGameState::INACTIVE; }
    ChessGameState GetState() const { return state; }
    glm::mat4 GetGameplayViewMatrix() const;
    glm::vec3 GetGameplayCameraPos() const;

    static bool IsNearTable(const glm::vec3& camPos);

    // Dynamic Rendering
    void DrawDynamic(Shader& phongShader, Shader& lightShader,
                     Mesh& cube, Mesh& cylinder, Mesh& sphere) const;
    void DrawHUD(HudRenderer& hud, int scrW, int scrH) const;

private:
    ChessGameState state = ChessGameState::INACTIVE;
    ChessPiece board[8][8];

    // Selected piece
    int selectedFile = -1;
    int selectedRank = -1;
    std::vector<ChessMove> legalMovesForSelected;

    // Hovered square
    int hoverFile = -1;
    int hoverRank = -1;
    bool isHovering = false;

    // Last move played
    bool hasLastMove = false;
    ChessMove lastMove;

    // AI thinking timer
    float aiTimer = 0.0f;

    // Moving piece animation
    PieceAnim moveAnim;

    // Captured pieces
    std::vector<ChessPiece> capturedByWhite;
    std::vector<ChessPiece> capturedByBlack;

    // Board geometry
    const glm::vec3 tableCenter = glm::vec3(8.0f, 2.05f, -5.0f);
    const float boardY = 2.125f;
    const float surfaceY = 2.175f;
    const float startX = 7.405f;  // File a center
    const float startZ = -5.595f; // Rank 1 center
    const float step = 0.170f;

    // Kinematics / Math helpers
    glm::vec3 GetSquarePos(int file, int rank) const;
    bool IsInsideBoard(int f, int r) const { return f >= 0 && f < 8 && r >= 0 && r < 8; }

    // Rules & Engine
    void SetupBoard();
    void ExecuteMove(const ChessMove& m, bool isAI);
    bool IsInCheck(PieceColor col, const ChessPiece b[8][8]) const;
    bool IsSquareAttacked(int f, int r, PieceColor byColor, const ChessPiece b[8][8]) const;
    std::vector<ChessMove> GeneratePseudoLegalMoves(PieceColor col, const ChessPiece b[8][8]) const;
    std::vector<ChessMove> GenerateLegalMoves(PieceColor col, const ChessPiece b[8][8]) const;
    bool HasAnyLegalMoves(PieceColor col) const;
    void CheckGameEnd();

    // AI
    ChessMove ComputeAIMove();
    int EvaluateBoard(const ChessPiece b[8][8]) const;
    int Minimax(int depth, int alpha, int beta, bool isMaximizing, ChessPiece b[8][8]);

    // Piece & Mesh Drawing Helpers
    void drawMeshRaw(Shader& shader, Mesh& mesh, const glm::mat4& model, const Material& mat) const;
    void drawPieceAt(Shader& shader, const ChessPiece& piece, const glm::vec3& p,
                     Mesh& cube, Mesh& cylinder, Mesh& sphere, float scale = 1.0f) const;
    void drawPawn(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& sphere, float s) const;
    void drawRook(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, float s) const;
    void drawKnight(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& cube, bool isWhite, float s) const;
    void drawBishop(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& sphere, float s) const;
    void drawQueen(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& sphere, float s) const;
    void drawKing(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& cube, float s) const;
};

#endif
