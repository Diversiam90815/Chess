/*
  ==============================================================================
	Module:         LightChessBoard
	Description:    Lightweight version of the Chessboard with minimal data for fast access and manipulation
  ==============================================================================
*/

#pragma once

#include <array>
#include <vector>
#include <memory>

#include "Move.h"
#include "Parameters.h"
#include "ChessBoard.h"


/// <summary>
/// Represents a lightweight chess piece with type, color, and movement status.
/// </summary>
struct LightPiece
{
	PieceType	type	 = PieceType::DefaultType;
	PlayerColor color	 = PlayerColor::NoColor;
	bool		hasMoved = false;

	LightPiece()		 = default;
	LightPiece(PieceType t, PlayerColor c, bool moved = false) : type(t), color(c), hasMoved(moved) {}

	bool isEmpty() const { return type == PieceType::DefaultType; }
	bool isValid() const { return type != PieceType::DefaultType && color != PlayerColor::NoColor; }
};


/// <summary>
/// Represents the information needed to undo a chess move.
/// </summary>
struct MoveUndo
{
	PossibleMove move;
	LightPiece	 capturedPiece;
	Position	 enPassantTarget		 = {-1, -1};
	bool		 whiteCanCastleKingside	 = false;
	bool		 whiteCanCastleQueenside = false;
	bool		 blackCanCastleKingside	 = false;
	bool		 blackCanCastleQueenside = false;
	int			 halfMoveClock			 = 0;
	int			 fullMoveNumber			 = 0;
};


/// <summary>
/// Represents a lightweight chessboard for fast move generation, evaluation, and state management in chess engines or applications.
/// </summary>
class LightChessBoard
{
public:
	LightChessBoard();
	LightChessBoard(ChessBoard &board);
	LightChessBoard(const LightChessBoard &other);
	LightChessBoard &operator=(const LightChessBoard &other);
	~LightChessBoard() = default;

	// Initialization
	void					  initializeStartingPosition();
	void					  copyFromChessBoard(ChessBoard &board);
	void					  clear();

	// Piece access
	const LightPiece		 &getPiece(Position pos) const;
	const LightPiece		 &getPiece(int x, int y) const;
	void					  setPiece(Position pos, const LightPiece &piece);
	void					  setPiece(int x, int y, const LightPiece &piece);
	void					  removePiece(Position pos);
	void					  removePiece(int x, int y);

	// Board state
	bool					  isEmpty(Position pos) const;
	bool					  isEmpty(int x, int y) const;
	bool					  isValidPosition(Position pos) const;
	bool					  isValidPosition(int x, int y) const;

	// Game state
	PlayerColor				  getCurrentPlayer() const { return mCurrentPlayer; }
	void					  setCurrentPlayer(PlayerColor player) { mCurrentPlayer = player; }
	void					  switchPlayer() { mCurrentPlayer = (mCurrentPlayer == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White; }

	// King positions
	Position				  getKingPosition(PlayerColor player) const;
	void					  updateKingPosition(Position pos, PlayerColor player);

	// Castling rights and validation
	void					  inferCastlingRights();
	bool					  canCastleKingside(PlayerColor player) const;
	bool					  canCastleQueenside(PlayerColor player) const;
	void					  setCastlingRights(PlayerColor player, bool kingside, bool queenside);
	bool					  isCastlingLegal(PlayerColor player, bool kingside) const; // Full castling validation

	// En passant
	Position				  getEnPassantTarget() const { return mEnPassantTarget; }
	void					  setEnPassantTarget(Position pos) { mEnPassantTarget = pos; }

	// Move counters
	int						  getHalfMoveClock() const { return mHalfMoveClock; }
	int						  getFullMoveNumber() const { return mFullMoveNumber; }
	void					  setHalfMoveClock(int count) { mHalfMoveClock = count; }
	void					  setFullMoveNumber(int number) { mFullMoveNumber = number; }

	// Fast move operations for algorithms
	MoveUndo				  makeMove(const PossibleMove &move);
	void					  unmakeMove(const MoveUndo &undoInfo);

	// Quick evaluation helpers
	std::vector<Position>	  getPiecePositions(PlayerColor player) const;
	std::vector<Position>	  getPiecePositions(PlayerColor player, PieceType type) const;
	int						  getMaterialValue(PlayerColor player) const;
	int						  getPieceCount(PlayerColor player) const;
	int						  getPieceCount(PlayerColor player, PieceType type) const;

	// Attack/defend queries
	bool					  isSquareAttacked(Position pos, PlayerColor attacker) const;
	bool					  isInCheck(PlayerColor player) const;

	// Move generation and validation (integrated MoveHelper functionality)
	bool					  canPieceMove(Position from, Position to, PieceType piece, PlayerColor player) const;
	std::vector<PossibleMove> generatePseudoLegalMoves(PlayerColor player) const;
	std::vector<PossibleMove> generateLegalMoves(PlayerColor player) const; // Filters out moves leaving king in check

	// Utility
	uint64_t				  getHashKey() const;
	bool					  isEndgame() const;
	int						  getGamePhaseValue() const;


private:
	// Helper methods
	void													   initializePieces();
	bool													   isPathClear(Position from, Position to) const;

	// Move generation helpers
	std::vector<Position>									   getKnightMoves(Position pos) const;
	std::vector<Position>									   getKingMoves(Position pos) const;
	std::vector<Position>									   getPawnMoves(Position pos, PlayerColor player) const;
	std::vector<Position>									   getSlidingMoves(Position pos, const std::array<std::pair<int, int>, 4> &directions) const;
	std::vector<Position>									   getDiagonalMoves(Position pos) const;
	std::vector<Position>									   getOrthogonalMoves(Position pos) const;

	// Castling validation helpers
	bool													   isCastlingPathClear(PlayerColor player, bool kingside) const;
	bool													   wouldKingPassThroughCheck(PlayerColor player, bool kingside) const;

	// Move execution helpers
	void													   executeCastling(const PossibleMove &move);
	void													   executeEnPassant(const PossibleMove &move);
	void													   updateGameState(const PossibleMove &move);

	// Hash calculation for transposition tables
	void													   calculateHashKey() const;
	mutable uint64_t										   mHashKey		   = 0;
	mutable bool											   mHashKeyValid   = false;

	// Piece values for quick material evaluation
	static constexpr int									   PIECE_VALUES[7] = {0, 100, 320, 330, 500, 900, 0};

	// Board representation -> 8x8 array for fast access
	std::array<std::array<LightPiece, BOARD_SIZE>, BOARD_SIZE> mBoard;

	// Game state
	PlayerColor												   mCurrentPlayer			= PlayerColor::White;
	Position												   mWhiteKingPos			= {4, 7};
	Position												   mBlackKingPos			= {4, 0};
	Position												   mEnPassantTarget			= {-1, -1};

	// Castling rights
	bool													   mWhiteCanCastleKingside	= true;
	bool													   mWhiteCanCastleQueenside = true;
	bool													   mBlackCanCastleKingside	= true;
	bool													   mBlackCanCastleQueenside = true;

	// Move counters
	int														   mHalfMoveClock			= 0;
	int														   mFullMoveNumber			= 1;
};
