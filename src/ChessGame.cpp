#include "ChessGame.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Piece values in centipawns
static const int VAL_PAWN   = 100;
static const int VAL_KNIGHT = 320;
static const int VAL_BISHOP = 330;
static const int VAL_ROOK   = 500;
static const int VAL_QUEEN  = 900;
static const int VAL_KING   = 20000;

// Standard Simplified Piece-Square Tables (from Black's perspective: rank 7 down to 0)
static const int pstPawn[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

static const int pstKnight[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

static const int pstBishop[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

static const int pstRook[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0},
    {  5, 10, 10, 10, 10, 10, 10,  5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    {  0,  0,  0,  5,  5,  0,  0,  0}
};

static const int pstQueen[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

static const int pstKing[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};

ChessGame::ChessGame()
{
    std::srand((unsigned int)std::time(nullptr));
    SetupBoard();
    state = ChessGameState::INACTIVE;
}

void ChessGame::Start()
{
    SetupBoard();
    selectedFile = -1;
    selectedRank = -1;
    legalMovesForSelected.clear();
    hoverFile = -1;
    hoverRank = -1;
    isHovering = false;
    hasLastMove = false;
    aiTimer = 0.0f;
    moveAnim.active = false;
    capturedByWhite.clear();
    capturedByBlack.clear();
    state = ChessGameState::WHITE_TURN;
}

void ChessGame::Stop()
{
    state = ChessGameState::INACTIVE;
    selectedFile = -1;
    selectedRank = -1;
    legalMovesForSelected.clear();
    moveAnim.active = false;
}

void ChessGame::ResetGame()
{
    SetupBoard();
    selectedFile = -1;
    selectedRank = -1;
    legalMovesForSelected.clear();
    hoverFile = -1;
    hoverRank = -1;
    isHovering = false;
    hasLastMove = false;
    aiTimer = 0.0f;
    moveAnim.active = false;
    capturedByWhite.clear();
    capturedByBlack.clear();
    if (state != ChessGameState::INACTIVE)
    {
        state = ChessGameState::WHITE_TURN;
    }
}

void ChessGame::SetupBoard()
{
    for (int f = 0; f < 8; ++f)
    {
        for (int r = 0; r < 8; ++r)
        {
            board[f][r] = ChessPiece();
        }
    }

    // White major pieces on rank 0
    board[0][0] = {PieceType::ROOK,   PieceColor::WHITE, false};
    board[1][0] = {PieceType::KNIGHT, PieceColor::WHITE, false};
    board[2][0] = {PieceType::BISHOP, PieceColor::WHITE, false};
    board[3][0] = {PieceType::QUEEN,  PieceColor::WHITE, false};
    board[4][0] = {PieceType::KING,   PieceColor::WHITE, false};
    board[5][0] = {PieceType::BISHOP, PieceColor::WHITE, false};
    board[6][0] = {PieceType::KNIGHT, PieceColor::WHITE, false};
    board[7][0] = {PieceType::ROOK,   PieceColor::WHITE, false};

    // White pawns on rank 1
    for (int f = 0; f < 8; ++f)
    {
        board[f][1] = {PieceType::PAWN, PieceColor::WHITE, false};
    }

    // Black pawns on rank 6
    for (int f = 0; f < 8; ++f)
    {
        board[f][6] = {PieceType::PAWN, PieceColor::BLACK, false};
    }

    // Black major pieces on rank 7
    board[0][7] = {PieceType::ROOK,   PieceColor::BLACK, false};
    board[1][7] = {PieceType::KNIGHT, PieceColor::BLACK, false};
    board[2][7] = {PieceType::BISHOP, PieceColor::BLACK, false};
    board[3][7] = {PieceType::QUEEN,  PieceColor::BLACK, false};
    board[4][7] = {PieceType::KING,   PieceColor::BLACK, false};
    board[5][7] = {PieceType::BISHOP, PieceColor::BLACK, false};
    board[6][7] = {PieceType::KNIGHT, PieceColor::BLACK, false};
    board[7][7] = {PieceType::ROOK,   PieceColor::BLACK, false};
}

glm::vec3 ChessGame::GetSquarePos(int file, int rank) const
{
    return glm::vec3(startX + (float)file * step, surfaceY, startZ + (float)rank * step);
}

glm::mat4 ChessGame::GetGameplayViewMatrix() const
{
    return glm::lookAt(GetGameplayCameraPos(), glm::vec3(8.0f, surfaceY, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 ChessGame::GetGameplayCameraPos() const
{
    // Positioned behind White's side (South) looking North-East towards the board
    return glm::vec3(8.0f, 3.42f, -6.65f);
}

bool ChessGame::IsNearTable(const glm::vec3& camPos)
{
    float dist = glm::length(glm::vec2(camPos.x - 8.0f, camPos.z - (-5.0f)));
    return (dist < 3.2f);
}

void ChessGame::Update(float dt)
{
    if (state == ChessGameState::INACTIVE) return;

    if (moveAnim.active)
    {
        moveAnim.elapsed += dt;
        if (moveAnim.elapsed >= moveAnim.duration)
        {
            moveAnim.active = false;
            CheckGameEnd();

            if (state == ChessGameState::WHITE_TURN)
            {
                // After White's move, hand over to AI
                state = ChessGameState::BLACK_THINKING;
                aiTimer = 0.50f; // Natural thinking delay
            }
            else if (state == ChessGameState::BLACK_THINKING)
            {
                // After Black's move, back to player
                state = ChessGameState::WHITE_TURN;
            }
        }
        return;
    }

    if (state == ChessGameState::BLACK_THINKING)
    {
        aiTimer -= dt;
        if (aiTimer <= 0.0f)
        {
            ChessMove aiMove = ComputeAIMove();
            if (aiMove.fromFile != aiMove.toFile || aiMove.fromRank != aiMove.toRank)
            {
                ExecuteMove(aiMove, true);
            }
            else
            {
                CheckGameEnd();
            }
        }
    }
}

void ChessGame::HandleMouseMove(double xpos, double ypos, int scrW, int scrH,
                                const glm::mat4& view, const glm::mat4& proj)
{
    if (state == ChessGameState::INACTIVE) return;

    float xndc = (2.0f * (float)xpos) / (float)scrW - 1.0f;
    float yndc = 1.0f - (2.0f * (float)ypos) / (float)scrH;

    glm::mat4 invVP = glm::inverse(proj * view);
    glm::vec4 pNear = invVP * glm::vec4(xndc, yndc, -1.0f, 1.0f);
    pNear /= pNear.w;
    glm::vec4 pFar  = invVP * glm::vec4(xndc, yndc,  1.0f, 1.0f);
    pFar /= pFar.w;

    glm::vec3 r0 = glm::vec3(pNear);
    glm::vec3 rd = glm::normalize(glm::vec3(pFar - pNear));

    isHovering = false;
    hoverFile = -1;
    hoverRank = -1;

    if (std::abs(rd.y) > 1e-5f)
    {
        float t = (surfaceY - r0.y) / rd.y;
        if (t > 0.0f)
        {
            glm::vec3 hit = r0 + t * rd;
            float gridOriginX = startX - step * 0.5f;
            float gridOriginZ = startZ - step * 0.5f;

            int f = (int)std::floor((hit.x - gridOriginX) / step);
            int r = (int)std::floor((hit.z - gridOriginZ) / step);

            if (IsInsideBoard(f, r))
            {
                hoverFile = f;
                hoverRank = r;
                isHovering = true;
            }
        }
    }
}

void ChessGame::HandleMouseButton(int button, int action)
{
    if (state != ChessGameState::WHITE_TURN || moveAnim.active) return;

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        selectedFile = -1;
        selectedRank = -1;
        legalMovesForSelected.clear();
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (!isHovering)
        {
            selectedFile = -1;
            selectedRank = -1;
            legalMovesForSelected.clear();
            return;
        }

        // Check if user clicked a destination square of the currently selected piece
        if (selectedFile != -1 && selectedRank != -1)
        {
            for (const auto& m : legalMovesForSelected)
            {
                if (m.toFile == hoverFile && m.toRank == hoverRank)
                {
                    ExecuteMove(m, false);
                    selectedFile = -1;
                    selectedRank = -1;
                    legalMovesForSelected.clear();
                    return;
                }
            }
        }

        // If clicking on user's own White piece, select it
        if (board[hoverFile][hoverRank].color == PieceColor::WHITE)
        {
            selectedFile = hoverFile;
            selectedRank = hoverRank;
            legalMovesForSelected.clear();

            auto allLegals = GenerateLegalMoves(PieceColor::WHITE, board);
            for (const auto& m : allLegals)
            {
                if (m.fromFile == selectedFile && m.fromRank == selectedRank)
                {
                    legalMovesForSelected.push_back(m);
                }
            }
        }
        else
        {
            selectedFile = -1;
            selectedRank = -1;
            legalMovesForSelected.clear();
        }
    }
}

void ChessGame::ExecuteMove(const ChessMove& m, bool isAI)
{
    ChessPiece moving = board[m.fromFile][m.fromRank];
    ChessPiece target = board[m.toFile][m.toRank];

    if (target.type != PieceType::NONE && target.color != moving.color)
    {
        if (moving.color == PieceColor::WHITE)
            capturedByWhite.push_back(target);
        else
            capturedByBlack.push_back(target);
    }

    // Launch animation
    moveAnim.active = true;
    moveAnim.piece = moving;
    moveAnim.startPos = GetSquarePos(m.fromFile, m.fromRank);
    moveAnim.endPos   = GetSquarePos(m.toFile, m.toRank);
    moveAnim.elapsed  = 0.0f;
    moveAnim.duration = 0.28f;

    lastMove = m;
    hasLastMove = true;

    // Remove from origin
    board[m.fromFile][m.fromRank] = ChessPiece();

    // Place at target (handle pawn promotion)
    ChessPiece destPiece = moving;
    destPiece.hasMoved = true;
    if (destPiece.type == PieceType::PAWN && (m.toRank == 7 || m.toRank == 0))
    {
        destPiece.type = PieceType::QUEEN;
    }
    board[m.toFile][m.toRank] = destPiece;

    // Handle castling rook repositioning
    if (m.isCastling)
    {
        if (m.toFile == 6) // Kingside
        {
            board[5][m.toRank] = board[7][m.toRank];
            board[5][m.toRank].hasMoved = true;
            board[7][m.toRank] = ChessPiece();
        }
        else if (m.toFile == 2) // Queenside
        {
            board[3][m.toRank] = board[0][m.toRank];
            board[3][m.toRank].hasMoved = true;
            board[0][m.toRank] = ChessPiece();
        }
    }
}

bool ChessGame::IsSquareAttacked(int f, int r, PieceColor byColor, const ChessPiece b[8][8]) const
{
    // Pawn attacks
    int pawnRank = (byColor == PieceColor::WHITE) ? (r - 1) : (r + 1);
    if (pawnRank >= 0 && pawnRank < 8)
    {
        if (f - 1 >= 0 && b[f - 1][pawnRank].type == PieceType::PAWN && b[f - 1][pawnRank].color == byColor) return true;
        if (f + 1 < 8  && b[f + 1][pawnRank].type == PieceType::PAWN && b[f + 1][pawnRank].color == byColor) return true;
    }

    // Knight attacks
    static const int kdx[8] = {-2, -1, 1, 2,  2,  1, -1, -2};
    static const int kdy[8] = { 1,  2, 2, 1, -1, -2, -2, -1};
    for (int i = 0; i < 8; ++i)
    {
        int nf = f + kdx[i], nr = r + kdy[i];
        if (IsInsideBoard(nf, nr))
        {
            if (b[nf][nr].type == PieceType::KNIGHT && b[nf][nr].color == byColor) return true;
        }
    }

    // Straight attacks (Rook / Queen)
    static const int rdx[4] = {0, 0, -1, 1};
    static const int rdy[4] = {1, -1, 0, 0};
    for (int i = 0; i < 4; ++i)
    {
        int nf = f + rdx[i], nr = r + rdy[i];
        while (IsInsideBoard(nf, nr))
        {
            if (b[nf][nr].type != PieceType::NONE)
            {
                if (b[nf][nr].color == byColor && (b[nf][nr].type == PieceType::ROOK || b[nf][nr].type == PieceType::QUEEN))
                    return true;
                break;
            }
            nf += rdx[i];
            nr += rdy[i];
        }
    }

    // Diagonal attacks (Bishop / Queen)
    static const int bdx[4] = {-1, 1, -1, 1};
    static const int bdy[4] = { 1, 1, -1,-1};
    for (int i = 0; i < 4; ++i)
    {
        int nf = f + bdx[i], nr = r + bdy[i];
        while (IsInsideBoard(nf, nr))
        {
            if (b[nf][nr].type != PieceType::NONE)
            {
                if (b[nf][nr].color == byColor && (b[nf][nr].type == PieceType::BISHOP || b[nf][nr].type == PieceType::QUEEN))
                    return true;
                break;
            }
            nf += bdx[i];
            nr += bdy[i];
        }
    }

    // King attacks
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            if (dx == 0 && dy == 0) continue;
            int nf = f + dx, nr = r + dy;
            if (IsInsideBoard(nf, nr))
            {
                if (b[nf][nr].type == PieceType::KING && b[nf][nr].color == byColor) return true;
            }
        }
    }

    return false;
}

bool ChessGame::IsInCheck(PieceColor col, const ChessPiece b[8][8]) const
{
    int kf = -1, kr = -1;
    for (int f = 0; f < 8; ++f)
    {
        for (int r = 0; r < 8; ++r)
        {
            if (b[f][r].type == PieceType::KING && b[f][r].color == col)
            {
                kf = f; kr = r; break;
            }
        }
        if (kf != -1) break;
    }
    if (kf == -1) return false;

    PieceColor enemyCol = (col == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    return IsSquareAttacked(kf, kr, enemyCol, b);
}

std::vector<ChessMove> ChessGame::GeneratePseudoLegalMoves(PieceColor col, const ChessPiece b[8][8]) const
{
    std::vector<ChessMove> moves;
    int dir = (col == PieceColor::WHITE) ? 1 : -1;
    int startRank = (col == PieceColor::WHITE) ? 1 : 6;
    PieceColor enemyCol = (col == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;

    for (int f = 0; f < 8; ++f)
    {
        for (int r = 0; r < 8; ++r)
        {
            const ChessPiece& p = b[f][r];
            if (p.color != col) continue;

            if (p.type == PieceType::PAWN)
            {
                // Single step forward
                int nr = r + dir;
                if (nr >= 0 && nr < 8 && b[f][nr].type == PieceType::NONE)
                {
                    moves.push_back({f, r, f, nr, PieceType::QUEEN, false, false});
                    // Double step forward
                    int nnr = r + 2 * dir;
                    if (r == startRank && b[f][nnr].type == PieceType::NONE)
                    {
                        moves.push_back({f, r, f, nnr, PieceType::NONE, false, false});
                    }
                }

                // Captures
                for (int df = -1; df <= 1; df += 2)
                {
                    int nf = f + df;
                    if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8)
                    {
                        if (b[nf][nr].color == enemyCol)
                        {
                            moves.push_back({f, r, nf, nr, PieceType::QUEEN, false, true});
                        }
                    }
                }
            }
            else if (p.type == PieceType::KNIGHT)
            {
                static const int kdx[8] = {-2, -1, 1, 2,  2,  1, -1, -2};
                static const int kdy[8] = { 1,  2, 2, 1, -1, -2, -2, -1};
                for (int i = 0; i < 8; ++i)
                {
                    int nf = f + kdx[i], nr = r + kdy[i];
                    if (IsInsideBoard(nf, nr) && b[nf][nr].color != col)
                    {
                        moves.push_back({f, r, nf, nr, PieceType::NONE, false, (b[nf][nr].color == enemyCol)});
                    }
                }
            }
            else if (p.type == PieceType::BISHOP || p.type == PieceType::ROOK || p.type == PieceType::QUEEN)
            {
                std::vector<std::pair<int, int>> dirs;
                if (p.type != PieceType::BISHOP)
                {
                    dirs.push_back({0, 1}); dirs.push_back({0, -1});
                    dirs.push_back({-1, 0}); dirs.push_back({1, 0});
                }
                if (p.type != PieceType::ROOK)
                {
                    dirs.push_back({-1, 1}); dirs.push_back({1, 1});
                    dirs.push_back({-1, -1}); dirs.push_back({1, -1});
                }

                for (auto d : dirs)
                {
                    int nf = f + d.first, nr = r + d.second;
                    while (IsInsideBoard(nf, nr))
                    {
                        if (b[nf][nr].type == PieceType::NONE)
                        {
                            moves.push_back({f, r, nf, nr, PieceType::NONE, false, false});
                        }
                        else
                        {
                            if (b[nf][nr].color == enemyCol)
                            {
                                moves.push_back({f, r, nf, nr, PieceType::NONE, false, true});
                            }
                            break;
                        }
                        nf += d.first;
                        nr += d.second;
                    }
                }
            }
            else if (p.type == PieceType::KING)
            {
                for (int df = -1; df <= 1; ++df)
                {
                    for (int dr = -1; dr <= 1; ++dr)
                    {
                        if (df == 0 && dr == 0) continue;
                        int nf = f + df, nr = r + dr;
                        if (IsInsideBoard(nf, nr) && b[nf][nr].color != col)
                        {
                            moves.push_back({f, r, nf, nr, PieceType::NONE, false, (b[nf][nr].color == enemyCol)});
                        }
                    }
                }

                // Castling
                if (!p.hasMoved && !IsInCheck(col, b))
                {
                    int backRank = (col == PieceColor::WHITE) ? 0 : 7;
                    if (r == backRank && f == 4)
                    {
                        // Kingside
                        if (b[7][backRank].type == PieceType::ROOK && !b[7][backRank].hasMoved)
                        {
                            if (b[5][backRank].type == PieceType::NONE && b[6][backRank].type == PieceType::NONE)
                            {
                                if (!IsSquareAttacked(5, backRank, enemyCol, b) && !IsSquareAttacked(6, backRank, enemyCol, b))
                                {
                                    moves.push_back({4, backRank, 6, backRank, PieceType::NONE, true, false});
                                }
                            }
                        }
                        // Queenside
                        if (b[0][backRank].type == PieceType::ROOK && !b[0][backRank].hasMoved)
                        {
                            if (b[1][backRank].type == PieceType::NONE && b[2][backRank].type == PieceType::NONE && b[3][backRank].type == PieceType::NONE)
                            {
                                if (!IsSquareAttacked(2, backRank, enemyCol, b) && !IsSquareAttacked(3, backRank, enemyCol, b))
                                {
                                    moves.push_back({4, backRank, 2, backRank, PieceType::NONE, true, false});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return moves;
}

std::vector<ChessMove> ChessGame::GenerateLegalMoves(PieceColor col, const ChessPiece b[8][8]) const
{
    std::vector<ChessMove> legal;
    auto pseudo = GeneratePseudoLegalMoves(col, b);

    for (const auto& m : pseudo)
    {
        ChessPiece tempB[8][8];
        for (int f = 0; f < 8; ++f)
            for (int r = 0; r < 8; ++r)
                tempB[f][r] = b[f][r];

        ChessPiece moved = tempB[m.fromFile][m.fromRank];
        tempB[m.fromFile][m.fromRank] = ChessPiece();
        moved.hasMoved = true;
        if (moved.type == PieceType::PAWN && (m.toRank == 7 || m.toRank == 0))
        {
            moved.type = PieceType::QUEEN;
        }
        tempB[m.toFile][m.toRank] = moved;

        if (m.isCastling)
        {
            if (m.toFile == 6)
            {
                tempB[5][m.toRank] = tempB[7][m.toRank];
                tempB[7][m.toRank] = ChessPiece();
            }
            else if (m.toFile == 2)
            {
                tempB[3][m.toRank] = tempB[0][m.toRank];
                tempB[0][m.toRank] = ChessPiece();
            }
        }

        if (!IsInCheck(col, tempB))
        {
            legal.push_back(m);
        }
    }
    return legal;
}

bool ChessGame::HasAnyLegalMoves(PieceColor col) const
{
    return !GenerateLegalMoves(col, board).empty();
}

void ChessGame::CheckGameEnd()
{
    PieceColor sideToMove = (state == ChessGameState::WHITE_TURN) ? PieceColor::WHITE : PieceColor::BLACK;
    bool hasMoves = HasAnyLegalMoves(sideToMove);

    if (!hasMoves)
    {
        if (IsInCheck(sideToMove, board))
        {
            if (sideToMove == PieceColor::WHITE)
                state = ChessGameState::CHECKMATE_BLACK_WINS;
            else
                state = ChessGameState::CHECKMATE_WHITE_WINS;
        }
        else
        {
            state = ChessGameState::STALEMATE;
        }
    }
}

int ChessGame::EvaluateBoard(const ChessPiece b[8][8]) const
{
    int score = 0;

    for (int f = 0; f < 8; ++f)
    {
        for (int r = 0; r < 8; ++r)
        {
            const ChessPiece& p = b[f][r];
            if (p.type == PieceType::NONE) continue;

            int val = 0;
            int pstVal = 0;
            int rankIdx = (p.color == PieceColor::BLACK) ? r : (7 - r);

            switch (p.type)
            {
                case PieceType::PAWN:   val = VAL_PAWN;   pstVal = pstPawn[rankIdx][f]; break;
                case PieceType::KNIGHT: val = VAL_KNIGHT; pstVal = pstKnight[rankIdx][f]; break;
                case PieceType::BISHOP: val = VAL_BISHOP; pstVal = pstBishop[rankIdx][f]; break;
                case PieceType::ROOK:   val = VAL_ROOK;   pstVal = pstRook[rankIdx][f]; break;
                case PieceType::QUEEN:  val = VAL_QUEEN;  pstVal = pstQueen[rankIdx][f]; break;
                case PieceType::KING:   val = VAL_KING;   pstVal = pstKing[rankIdx][f]; break;
                default: break;
            }

            int total = val + pstVal;
            if (p.color == PieceColor::BLACK)
                score += total;
            else
                score -= total;
        }
    }

    return score;
}

int ChessGame::Minimax(int depth, int alpha, int beta, bool isMaximizing, ChessPiece b[8][8])
{
    if (depth == 0)
    {
        return EvaluateBoard(b);
    }

    PieceColor col = isMaximizing ? PieceColor::BLACK : PieceColor::WHITE;
    auto legals = GenerateLegalMoves(col, b);

    if (legals.empty())
    {
        if (IsInCheck(col, b))
            return isMaximizing ? -100000 : 100000;
        return 0; // Stalemate
    }

    if (isMaximizing)
    {
        int maxEval = -999999;
        for (const auto& m : legals)
        {
            ChessPiece tempB[8][8];
            for (int f = 0; f < 8; ++f)
                for (int r = 0; r < 8; ++r)
                    tempB[f][r] = b[f][r];

            ChessPiece moved = tempB[m.fromFile][m.fromRank];
            tempB[m.fromFile][m.fromRank] = ChessPiece();
            moved.hasMoved = true;
            if (moved.type == PieceType::PAWN && m.toRank == 0) moved.type = PieceType::QUEEN;
            tempB[m.toFile][m.toRank] = moved;

            int eval = Minimax(depth - 1, alpha, beta, false, tempB);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    }
    else
    {
        int minEval = 999999;
        for (const auto& m : legals)
        {
            ChessPiece tempB[8][8];
            for (int f = 0; f < 8; ++f)
                for (int r = 0; r < 8; ++r)
                    tempB[f][r] = b[f][r];

            ChessPiece moved = tempB[m.fromFile][m.fromRank];
            tempB[m.fromFile][m.fromRank] = ChessPiece();
            moved.hasMoved = true;
            if (moved.type == PieceType::PAWN && m.toRank == 7) moved.type = PieceType::QUEEN;
            tempB[m.toFile][m.toRank] = moved;

            int eval = Minimax(depth - 1, alpha, beta, true, tempB);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

ChessMove ChessGame::ComputeAIMove()
{
    auto legals = GenerateLegalMoves(PieceColor::BLACK, board);
    if (legals.empty()) return ChessMove();

    ChessMove bestMove = legals[0];
    int bestScore = -999999;

    for (const auto& m : legals)
    {
        ChessPiece tempB[8][8];
        for (int f = 0; f < 8; ++f)
            for (int r = 0; r < 8; ++r)
                tempB[f][r] = board[f][r];

        ChessPiece moved = tempB[m.fromFile][m.fromRank];
        tempB[m.fromFile][m.fromRank] = ChessPiece();
        moved.hasMoved = true;
        if (moved.type == PieceType::PAWN && m.toRank == 0) moved.type = PieceType::QUEEN;
        tempB[m.toFile][m.toRank] = moved;

        int score = Minimax(2, -999999, 999999, false, tempB);

        // Add slight random jitter (+/- 3) to introduce varied opening play
        score += (std::rand() % 7) - 3;

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = m;
        }
    }

    return bestMove;
}

// ============================================================
//  3D Rendering
// ============================================================
void ChessGame::drawMeshRaw(Shader& shader, Mesh& mesh, const glm::mat4& model, const Material& mat) const
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

void ChessGame::drawPawn(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& sphere, float s) const
{
    glm::mat4 m;
    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.008f * s, p.z));
    m = glm::scale(m, glm::vec3(0.070f * s, 0.016f * s, 0.070f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.038f * s, p.z));
    m = glm::scale(m, glm::vec3(0.046f * s, 0.046f * s, 0.046f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.063f * s, p.z));
    m = glm::scale(m, glm::vec3(0.050f * s, 0.008f * s, 0.050f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.080f * s, p.z));
    m = glm::scale(m, glm::vec3(0.036f * s, 0.036f * s, 0.036f * s));
    drawMeshRaw(shader, sphere, m, mat);
}

void ChessGame::drawRook(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, float s) const
{
    glm::mat4 m;
    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.010f * s, p.z));
    m = glm::scale(m, glm::vec3(0.080f * s, 0.020f * s, 0.080f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.055f * s, p.z));
    m = glm::scale(m, glm::vec3(0.060f * s, 0.070f * s, 0.060f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.095f * s, p.z));
    m = glm::scale(m, glm::vec3(0.068f * s, 0.012f * s, 0.068f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.112f * s, p.z));
    m = glm::scale(m, glm::vec3(0.074f * s, 0.022f * s, 0.074f * s));
    drawMeshRaw(shader, cylinder, m, mat);
}

void ChessGame::drawKnight(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& cube, bool isWhite, float s) const
{
    float rotY = isWhite ? 0.0f : 180.0f;
    glm::mat4 m;

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.010f * s, p.z));
    m = glm::scale(m, glm::vec3(0.080f * s, 0.020f * s, 0.080f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.035f * s, p.z));
    m = glm::scale(m, glm::vec3(0.058f * s, 0.030f * s, 0.058f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.075f * s, p.z));
    m = glm::rotate(m, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, glm::vec3(0.046f * s, 0.055f * s, 0.065f * s));
    drawMeshRaw(shader, cube, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.105f * s, p.z));
    m = glm::rotate(m, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, glm::vec3(0.035f * s, 0.025f * s, 0.040f * s));
    drawMeshRaw(shader, cube, m, mat);
}

void ChessGame::drawBishop(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& sphere, float s) const
{
    glm::mat4 m;
    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.010f * s, p.z));
    m = glm::scale(m, glm::vec3(0.080f * s, 0.020f * s, 0.080f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.060f * s, p.z));
    m = glm::scale(m, glm::vec3(0.052f * s, 0.080f * s, 0.052f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.105f * s, p.z));
    m = glm::scale(m, glm::vec3(0.058f * s, 0.010f * s, 0.058f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.125f * s, p.z));
    m = glm::scale(m, glm::vec3(0.044f * s, 0.052f * s, 0.044f * s));
    drawMeshRaw(shader, sphere, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.155f * s, p.z));
    m = glm::scale(m, glm::vec3(0.016f * s, 0.016f * s, 0.016f * s));
    drawMeshRaw(shader, sphere, m, mat);
}

void ChessGame::drawQueen(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& sphere, float s) const
{
    glm::mat4 m;
    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.012f * s, p.z));
    m = glm::scale(m, glm::vec3(0.086f * s, 0.024f * s, 0.086f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.070f * s, p.z));
    m = glm::scale(m, glm::vec3(0.056f * s, 0.095f * s, 0.056f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.122f * s, p.z));
    m = glm::scale(m, glm::vec3(0.064f * s, 0.012f * s, 0.064f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.140f * s, p.z));
    m = glm::scale(m, glm::vec3(0.068f * s, 0.024f * s, 0.068f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.160f * s, p.z));
    m = glm::scale(m, glm::vec3(0.022f * s, 0.022f * s, 0.022f * s));
    drawMeshRaw(shader, sphere, m, mat);
}

void ChessGame::drawKing(Shader& shader, const glm::vec3& p, const Material& mat, Mesh& cylinder, Mesh& cube, float s) const
{
    glm::mat4 m;
    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.012f * s, p.z));
    m = glm::scale(m, glm::vec3(0.088f * s, 0.024f * s, 0.088f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.075f * s, p.z));
    m = glm::scale(m, glm::vec3(0.060f * s, 0.102f * s, 0.060f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.130f * s, p.z));
    m = glm::scale(m, glm::vec3(0.066f * s, 0.012f * s, 0.066f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.150f * s, p.z));
    m = glm::scale(m, glm::vec3(0.070f * s, 0.028f * s, 0.070f * s));
    drawMeshRaw(shader, cylinder, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.174f * s, p.z));
    m = glm::scale(m, glm::vec3(0.012f * s, 0.024f * s, 0.010f * s));
    drawMeshRaw(shader, cube, m, mat);

    m = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.178f * s, p.z));
    m = glm::scale(m, glm::vec3(0.024f * s, 0.008f * s, 0.010f * s));
    drawMeshRaw(shader, cube, m, mat);
}

void ChessGame::drawPieceAt(Shader& shader, const ChessPiece& piece, const glm::vec3& p,
                           Mesh& cube, Mesh& cylinder, Mesh& sphere, float scale) const
{
    if (piece.type == PieceType::NONE) return;

    auto whitePieceMat = Material::Make(glm::vec3(0.96f, 0.93f, 0.84f), 64.0f, 0.50f);
    auto blackPieceMat = Material::Make(glm::vec3(0.12f, 0.10f, 0.10f), 64.0f, 0.50f);
    const auto& mat = (piece.color == PieceColor::WHITE) ? whitePieceMat : blackPieceMat;

    switch (piece.type)
    {
        case PieceType::PAWN:   drawPawn(shader, p, mat, cylinder, sphere, scale); break;
        case PieceType::ROOK:   drawRook(shader, p, mat, cylinder, scale); break;
        case PieceType::KNIGHT: drawKnight(shader, p, mat, cylinder, cube, piece.color == PieceColor::WHITE, scale); break;
        case PieceType::BISHOP: drawBishop(shader, p, mat, cylinder, sphere, scale); break;
        case PieceType::QUEEN:  drawQueen(shader, p, mat, cylinder, sphere, scale); break;
        case PieceType::KING:   drawKing(shader, p, mat, cylinder, cube, scale); break;
        default: break;
    }
}

void ChessGame::DrawDynamic(Shader& phongShader, Shader& lightShader,
                            Mesh& cube, Mesh& cylinder, Mesh& sphere) const
{
    if (state == ChessGameState::INACTIVE) return;

    // Materials for board highlights
    auto selectedMat = Material::Make(glm::vec3(0.95f, 0.82f, 0.15f), 32.0f, 0.2f, glm::vec3(0.35f, 0.28f, 0.05f));
    auto hoverMat    = Material::Make(glm::vec3(0.85f, 0.90f, 1.00f), 32.0f, 0.2f, glm::vec3(0.15f, 0.20f, 0.25f));
    auto legalDotMat = Material::Make(glm::vec3(0.20f, 0.85f, 0.35f), 48.0f, 0.3f, glm::vec3(0.10f, 0.35f, 0.15f));
    auto captureMat  = Material::Make(glm::vec3(0.95f, 0.22f, 0.18f), 48.0f, 0.3f, glm::vec3(0.35f, 0.10f, 0.08f));
    auto lastMoveMat = Material::Make(glm::vec3(0.85f, 0.72f, 0.30f), 32.0f, 0.2f, glm::vec3(0.22f, 0.18f, 0.06f));
    auto checkMat    = Material::Make(glm::vec3(1.00f, 0.12f, 0.12f), 64.0f, 0.5f, glm::vec3(0.55f, 0.08f, 0.08f));

    // 1. Last move highlights (from and to squares)
    if (hasLastMove)
    {
        glm::vec3 pFrom = GetSquarePos(lastMove.fromFile, lastMove.fromRank);
        glm::vec3 pTo   = GetSquarePos(lastMove.toFile, lastMove.toRank);

        glm::mat4 mFrom = glm::translate(glm::mat4(1.0f), glm::vec3(pFrom.x, surfaceY + 0.001f, pFrom.z));
        mFrom = glm::scale(mFrom, glm::vec3(0.155f, 0.002f, 0.155f));
        drawMeshRaw(phongShader, cube, mFrom, lastMoveMat);

        glm::mat4 mTo = glm::translate(glm::mat4(1.0f), glm::vec3(pTo.x, surfaceY + 0.001f, pTo.z));
        mTo = glm::scale(mTo, glm::vec3(0.155f, 0.002f, 0.155f));
        drawMeshRaw(phongShader, cube, mTo, lastMoveMat);
    }

    // 2. Check highlight on checked king's square
    for (int colInt = 1; colInt <= 2; ++colInt)
    {
        PieceColor c = (PieceColor)colInt;
        if (IsInCheck(c, board))
        {
            for (int f = 0; f < 8; ++f)
            {
                for (int r = 0; r < 8; ++r)
                {
                    if (board[f][r].type == PieceType::KING && board[f][r].color == c)
                    {
                        glm::vec3 kp = GetSquarePos(f, r);
                        glm::mat4 mk = glm::translate(glm::mat4(1.0f), glm::vec3(kp.x, surfaceY + 0.002f, kp.z));
                        mk = glm::scale(mk, glm::vec3(0.160f, 0.003f, 0.160f));
                        drawMeshRaw(phongShader, cube, mk, checkMat);
                    }
                }
            }
        }
    }

    // 3. Hovered square subtle outline
    if (isHovering && !(hoverFile == selectedFile && hoverRank == selectedRank))
    {
        glm::vec3 hp = GetSquarePos(hoverFile, hoverRank);
        glm::mat4 mh = glm::translate(glm::mat4(1.0f), glm::vec3(hp.x, surfaceY + 0.0025f, hp.z));
        mh = glm::scale(mh, glm::vec3(0.162f, 0.002f, 0.162f));
        drawMeshRaw(phongShader, cube, mh, hoverMat);
    }

    // 4. Selected square highlight
    if (selectedFile != -1 && selectedRank != -1)
    {
        glm::vec3 sp = GetSquarePos(selectedFile, selectedRank);
        glm::mat4 ms = glm::translate(glm::mat4(1.0f), glm::vec3(sp.x, surfaceY + 0.003f, sp.z));
        ms = glm::scale(ms, glm::vec3(0.162f, 0.003f, 0.162f));
        drawMeshRaw(phongShader, cube, ms, selectedMat);

        // 5. Legal destination markers
        for (const auto& m : legalMovesForSelected)
        {
            glm::vec3 lp = GetSquarePos(m.toFile, m.toRank);
            if (m.isCapture || board[m.toFile][m.toRank].type != PieceType::NONE)
            {
                // Capture target ring/disc
                glm::mat4 ml = glm::translate(glm::mat4(1.0f), glm::vec3(lp.x, surfaceY + 0.004f, lp.z));
                ml = glm::scale(ml, glm::vec3(0.145f, 0.004f, 0.145f));
                drawMeshRaw(phongShader, cube, ml, captureMat);
            }
            else
            {
                // Normal move dot
                glm::mat4 ml = glm::translate(glm::mat4(1.0f), glm::vec3(lp.x, surfaceY + 0.004f, lp.z));
                ml = glm::scale(ml, glm::vec3(0.065f, 0.004f, 0.065f));
                drawMeshRaw(phongShader, cylinder, ml, legalDotMat);
            }
        }
    }

    // 6. Draw all active pieces on the board
    for (int f = 0; f < 8; ++f)
    {
        for (int r = 0; r < 8; ++r)
        {
            const ChessPiece& p = board[f][r];
            if (p.type == PieceType::NONE) continue;

            // If this piece is currently in flight during animation, skip drawing static position
            if (moveAnim.active && f == lastMove.toFile && r == lastMove.toRank)
            {
                continue;
            }

            glm::vec3 pos = GetSquarePos(f, r);
            drawPieceAt(phongShader, p, pos, cube, cylinder, sphere, 1.0f);
        }
    }

    // 7. Draw in-flight moving piece with arced trajectory
    if (moveAnim.active)
    {
        float t = std::clamp(moveAnim.elapsed / moveAnim.duration, 0.0f, 1.0f);
        float smoothT = t * t * (3.0f - 2.0f * t);
        glm::vec3 renderPos = glm::mix(moveAnim.startPos, moveAnim.endPos, smoothT);
        renderPos.y += std::sin(t * 3.14159265f) * 0.09f; // graceful arced leap
        drawPieceAt(phongShader, moveAnim.piece, renderPos, cube, cylinder, sphere, 1.0f);
    }

    // 8. Draw captured pieces neatly displayed on the wood tabletop flanks
    // Black pieces captured by White: arranged in rows along the East flank (X ~ 9.0m..9.3m)
    for (size_t i = 0; i < capturedByWhite.size(); ++i)
    {
        int row = (int)(i % 8);
        int col = (int)(i / 8);
        float capX = 8.95f + (float)col * 0.18f;
        float capZ = -5.595f + (float)row * 0.170f;
        drawPieceAt(phongShader, capturedByWhite[i], glm::vec3(capX, surfaceY, capZ), cube, cylinder, sphere, 0.85f);
    }

    // White pieces captured by Black: arranged in rows along the West flank behind clock (X ~ 6.95m..7.15m)
    for (size_t i = 0; i < capturedByBlack.size(); ++i)
    {
        int row = (int)(i % 8);
        int col = (int)(i / 8);
        float capX = 7.05f - (float)col * 0.18f;
        float capZ = -5.595f + (float)row * 0.170f;
        drawPieceAt(phongShader, capturedByBlack[i], glm::vec3(capX, surfaceY, capZ), cube, cylinder, sphere, 0.85f);
    }
}

void ChessGame::DrawHUD(HudRenderer& hud, int scrW, int scrH) const
{
    if (state == ChessGameState::INACTIVE) return;

    hud.BeginFrame();

    // Title banner
    hud.RenderTextCentered("CHESS: Player (White) vs Simple AI (Black)", 35.0f, 2.2f, glm::vec3(1.0f, 0.95f, 0.75f));

    // Current turn / game status
    if (state == ChessGameState::WHITE_TURN)
    {
        bool inCheck = IsInCheck(PieceColor::WHITE, board);
        if (inCheck)
            hud.RenderTextCentered("YOUR TURN (White) - YOU ARE IN CHECK!", 75.0f, 2.0f, glm::vec3(1.0f, 0.35f, 0.35f));
        else
            hud.RenderTextCentered("YOUR TURN (White) - Select piece to move", 75.0f, 2.0f, glm::vec3(0.85f, 0.95f, 1.0f));
    }
    else if (state == ChessGameState::BLACK_THINKING)
    {
        hud.RenderTextCentered("AI THINKING (Black)...", 75.0f, 2.0f, glm::vec3(1.0f, 0.85f, 0.35f));
    }
    else if (state == ChessGameState::CHECKMATE_WHITE_WINS)
    {
        hud.RenderTextCentered("CHECKMATE! YOU WIN!", 75.0f, 2.6f, glm::vec3(0.40f, 1.00f, 0.40f));
    }
    else if (state == ChessGameState::CHECKMATE_BLACK_WINS)
    {
        hud.RenderTextCentered("CHECKMATE! AI WINS!", 75.0f, 2.6f, glm::vec3(1.00f, 0.30f, 0.30f));
    }
    else if (state == ChessGameState::STALEMATE)
    {
        hud.RenderTextCentered("STALEMATE! DRAW!", 75.0f, 2.6f, glm::vec3(0.85f, 0.85f, 0.90f));
    }

    // Last move display
    if (hasLastMove)
    {
        char fromFileChar = 'a' + lastMove.fromFile;
        char fromRankChar = '1' + lastMove.fromRank;
        char toFileChar   = 'a' + lastMove.toFile;
        char toRankChar   = '1' + lastMove.toRank;
        std::string moveStr = "Last Move: ";
        moveStr += fromFileChar;
        moveStr += fromRankChar;
        moveStr += (lastMove.isCapture ? " x " : " -> ");
        moveStr += toFileChar;
        moveStr += toRankChar;
        if (lastMove.isCastling) moveStr += " (Castle)";
        hud.RenderTextCentered(moveStr, 110.0f, 1.6f, glm::vec3(0.85f, 0.85f, 0.70f));
    }

    // Captured counts summary
    std::string capWhite = "Captured by You: " + std::to_string(capturedByWhite.size());
    std::string capBlack = "Captured by AI: " + std::to_string(capturedByBlack.size());
    hud.RenderText(capWhite, 40.0f, (float)scrH - 80.0f, 1.5f, glm::vec3(0.95f, 0.95f, 0.85f));
    hud.RenderText(capBlack, 40.0f, (float)scrH - 55.0f, 1.5f, glm::vec3(0.70f, 0.70f, 0.75f));

    // Bottom controls guide
    hud.RenderTextCentered("Left Click: Select / Move | Right Click: Deselect | R: New Game | E / Esc: Exit",
                           (float)scrH - 30.0f, 1.4f, glm::vec3(0.75f, 0.80f, 0.90f));

    hud.EndFrame();
}
