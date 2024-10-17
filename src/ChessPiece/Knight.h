
#pragma once
#include "ChessPiece.h"

class Knight : public ChessPiece
{
public:
	Knight(PieceColor color) : ChessPiece(PieceType::Knight, color)
	{
	}

	bool							 isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard &board) const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;
}