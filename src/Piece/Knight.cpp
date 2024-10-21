/*
  ==============================================================================

	Class:          Knight

	Description:    Knight class inheriting from ChessPiece

  ==============================================================================
*/


#include "Knight.h"
#include "ChessBoard.h"
#include <cmath>


int Knight::getPieceValue() const
{
	return knightValue;
}


std::vector<std::pair<int, int>> Knight::getPossibleMoves(int x, int y, ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;
	int								 moveOffsets[8][2] = {{1, 2}, {2, 1}, {-1, 2}, {-2, 1}, {1, -2}, {2, -1}, {-1, -2}, {-2, -1}};

	for (auto &offset : moveOffsets)
	{
		int newX = x + offset[0];
		int newY = y + offset[1];
		if (newX >= 0 && newX <= 7 && newY >= 0 && newY <= 7)
		{
			auto targetPiece = board.getPiece(newX, newY);
			if (targetPiece == nullptr || targetPiece->getColor() != color)
			{
				moves.emplace_back(newX, newY);
			}
		}
	}

	return moves;
}