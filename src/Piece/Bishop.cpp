/*
  ==============================================================================

	Class:          Bishop

	Description:    Bishop class inheriting from ChessPiece

  ==============================================================================
*/


#include "Bishop.h"
#include "ChessBoard.h"
#include <cmath>


bool Bishop::isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard &board) const
{
	int dx = abs(toX - fromX);
	int dy = abs(toY - fromY);

	if (dx == dy && dx != 0)
	{
		int stepX = (toX - fromX) / dx;
		int stepY = (toY - fromY) / dy;

		for (int i = 1; i < dx; ++i)
		{
			int x = fromX + i * stepX;
			int y = fromY + i * stepY;

			if (!board.isEmpty(x, y))
			{
				return false; // Path is blocked
			}
		}
		auto targetPiece = board.getPiece(toX, toY);
		if (targetPiece == nullptr || targetPiece->getColor() != color)
		{
			return true;
		}
	}
	return false;
}


std::vector<std::pair<int, int>> Bishop::getPossibleMoves(int x, int y, ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;

	int								 directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

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