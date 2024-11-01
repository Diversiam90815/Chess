/*
  ==============================================================================

	Class:          ChessBoard

	Description:    Class charecterizing a virtual chess board

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

using PlayerPiece = std::pair<Position, std::shared_ptr<ChessPiece>>;


struct Square
{
	Position					pos;
	std::shared_ptr<ChessPiece> piece;

	Square(int x, int y) : pos{x, y}, piece(nullptr)
	{
	}

	Square() : pos{0, 0}, piece(nullptr)
	{
	}
};


class ChessBoard
{
public:
	ChessBoard();
	~ChessBoard();


	void						initializeBoard();

	Square					   &getSquare(Position pos);

	void						setPiece(Position pos, std::shared_ptr<ChessPiece> piece);

	void						setPiece(int x, int y, std::shared_ptr<ChessPiece> piece);

	std::vector<PlayerPiece>	getPiecesFromPlayer(PieceColor playerColor);

	std::shared_ptr<ChessPiece> getPiece(Position pos);

	void						removePiece(Position pos);

	bool						movePiece(Position start, Position end);

	bool						isEmpty(Position pos) const;

	void						updateKingsPosition(Position &pos, PieceColor player);

	Position					getKingsPosition(PieceColor player) const;

private:
	std::vector<std::vector<Square>> squares;

	Position						 mWhiteKingPosition;

	Position						 mBlackKingPosition;
};