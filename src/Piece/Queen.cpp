/*
  ==============================================================================

	Class:          Queen

	Description:    Queen class inheriting from ChessPiece

  ==============================================================================
*/


#include "Queen.h"
#include "ChessBoard.h"
#include <cmath>


int Queen::getPieceValue() const
{
	return queenValue;
}


std::vector<std::pair<int, int>> Queen::getPossibleMoves(int x, int y, ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;

	int								 directions[8][2] = {
		 {1, 0},  // Right
		 {-1, 0}, // Left
		 {0, 1},  // Up
		 {0, -1}, // Down
		 {1, 1},  // Up-Right
		 {1, -1}, // Down-Right
		 {-1, 1}, // Up-Left
		 {-1, -1} // Down-Left
	 };

	for (auto &dir : directions)
	{
		int newX = x + dir[0];
		int newY = y + dir[1];
		while (newX >= 0 && newX <= 7 && newY >= 0 && newY <= 7)
		{
			if (board.isEmpty(newX, newY))
			{
				moves.emplace_back(newX, newY);
			}
			else
			{
				if (board.getPiece(newX, newY)->getColor() != color)
				{
					moves.emplace_back(newX, newY); // Can capture
				}
				break;								// Can't move past occupied square
			}
			newX += dir[0];
			newY += dir[1];
		}
	}

	return moves;
}
