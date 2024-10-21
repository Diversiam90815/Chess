/*
  ==============================================================================

	Class:          Rook

	Description:    Rook class inheriting from ChessPiece

  ==============================================================================
*/


#include "Rook.h"
#include "ChessBoard.h"
#include <cmath>


int Rook::getPieceValue() const
{
	return rookValue;
}


std::vector<std::pair<int, int>> Rook::getPossibleMoves(int x, int y, ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;

	int								 directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

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
				break;
			}
			newX += dir[0];
			newY += dir[1];
		}
	}

	return moves;
}
