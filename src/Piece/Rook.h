/*
  ==============================================================================

	Class:          Rook

	Description:    Rook class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"


class Rook : public ChessPiece
{
public:
	Rook(PieceColor color) : ChessPiece(PieceType::Rook, color)
	{
	}

	bool							 isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard &board) const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;
};
