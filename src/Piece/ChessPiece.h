/*
  ==============================================================================

	Class:          ChessPiece

	Description:    Virtual class defining a generalized chess piece

  ==============================================================================
*/

#pragma once

#include <vector>
#include <utility>
#include "Move.h"
#include "Parameters.h"

class ChessBoard;

//
// enum PieceType
//{
//	DefaultType,
//	Pawn,
//	Knight,
//	Bishop,
//	Rook,
//	Queen,
//	King
//};
//
//
// enum PieceColor
//{
//	NoColor,
//	White,
//	Black
//};


class ChessPiece
{
public:
	ChessPiece(PieceType type, PieceColor color) : type(type), color(color), hasMoved(false)
	{
	}

	virtual ~ChessPiece()
	{
	}

	PieceType getType() const
	{
		return type;
	}

	PieceColor getColor() const
	{
		return color;
	}

	bool getHasMoved() const
	{
		return hasMoved;
	}

	void setHasMoved(bool moved)
	{
		hasMoved = moved;
	}


	virtual std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board) const = 0;


protected:
	PieceType  type;

	PieceColor color;

	bool	   hasMoved;
};
