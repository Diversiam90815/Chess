/*
  ==============================================================================
	Module:         ChessBoard
	Description:    Class characterizing a virtual chess board
  ==============================================================================
*/

#pragma once

#include <array>
#include <memory>

#include "Move.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"


#define CHESSBOARD_DEBUG false


using PlayerPiece	  = std::pair<Position, std::shared_ptr<ChessPiece>>;
using BoardStateArray = int[BOARD_SIZE][BOARD_SIZE];


/// <summary>
/// Represents a square on a chessboard, including its position and an optional chess piece.
/// </summary>
struct Square
{
	Position					pos;
	std::shared_ptr<ChessPiece> piece;

	Square(int x, int y) : pos{x, y}, piece(nullptr) {}

	Square() : pos{0, 0}, piece(nullptr) {}
};


/// <summary>
/// Represents a chessboard and provides methods to manage and query its state, including piece placement, movement, and board initialization.
/// </summary>
class ChessBoard
{
public:
	ChessBoard();
	ChessBoard(const ChessBoard &other);
	~ChessBoard();

	void						 initializeBoard();

	Square						&getSquare(Position pos);

	void						 setPiece(Position pos, std::shared_ptr<ChessPiece> piece);
	std::shared_ptr<ChessPiece> &getPiece(Position pos);

	std::vector<PlayerPiece>	 getPiecesFromPlayer(PlayerColor playerColor);

	bool						 movePiece(Position start, Position end);
	void						 removePiece(Position pos);

	bool						 isEmpty(Position pos) const;

	void						 updateKingsPosition(Position &pos, PlayerColor player);
	Position					 getKingsPosition(PlayerColor player) const;

	void						 removeAllPiecesFromBoard();

	bool						 getBoardState(BoardStateArray boardState);

private:
	std::vector<std::vector<Square>> squares;

	Position						 mWhiteKingPosition = Position{-1, -1};

	Position						 mBlackKingPosition = Position{-1, -1};

	bool							 mInitialized		= false;
};
