
#pragma once

#include "PieceType.h"
#include <vector>
#include <utility> // For std::pair


class ChessBoard;


class ChessPiece
{
protected:
	PieceType  type;
	PieceColor color;
	bool	   hasMoved;

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


	virtual bool							 isValidMove(int fromX, int fromY, int toX, int toY, const ChessBoard &board) const = 0;

	virtual std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, const ChessBoard &board) const						= 0;
};