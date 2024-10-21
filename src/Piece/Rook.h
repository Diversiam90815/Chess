/*
  ==============================================================================

	Class:          Rook

	Description:    Rook class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"


class Rook : public ChessPiece
{
public:
	Rook(PieceColor color) : ChessPiece(PieceType::Rook, color)
	{
	}

	int getPieceValue() const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;
};
