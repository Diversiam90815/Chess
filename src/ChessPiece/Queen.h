
#pragma once
#include "ChessPiece.h"

class Queen : public ChessPiece
{
public:
	Queen(PieceColor color) : ChessPiece(PieceType::Queen, color)
	{
	}

	bool							 isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard &board) const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;
};