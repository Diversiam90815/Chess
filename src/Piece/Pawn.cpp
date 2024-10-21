/*
  ==============================================================================

	Class:          Pawn

	Description:    Pawn class inheriting from ChessPiece

  ==============================================================================
*/


#include "Pawn.h"
#include "ChessBoard.h"


int Pawn::getPieceValue() const
{
	return pawnValue;
}


std::vector<std::pair<int, int>> Pawn::getPossibleMoves(int x, int y, ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;
	int								 direction = (color == PieceColor::White) ? 1 : -1;
	int								 startRow  = (color == PieceColor::White) ? 1 : 6;

	// Standard move
	if (board.isEmpty(x, y + direction))
	{
		moves.emplace_back(x, y + direction);

		// Double move
		if (!hasMoved && board.isEmpty(x, y + 2 * direction))
		{
			moves.emplace_back(x, y + 2 * direction);
		}
	}

	// Captures
	if (x > 0 && board.getPiece(x - 1, y + direction) != nullptr && board.getPiece(x - 1, y + direction)->getColor() != color)
	{
		moves.emplace_back(x - 1, y + direction);
	}
	
	if (x < 7 && board.getPiece(x + 1, y + direction) != nullptr && board.getPiece(x + 1, y + direction)->getColor() != color)
	{
		moves.emplace_back(x + 1, y + direction);
	}

	return moves;
}