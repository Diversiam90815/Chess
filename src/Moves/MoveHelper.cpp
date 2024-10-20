/*
  ==============================================================================

	Class:          MoveHelper

	Description:    Helper class for generating general moves in chess

  ==============================================================================
*/


#include "MoveHelper.h"


MoveHelper::MoveHelper()
{
}


MoveHelper::~MoveHelper()
{
}


bool MoveHelper::checkAvailableMoves(PieceType piece)
{
	return false;
}


std::vector<Move> MoveHelper::getAvailableMoves()
{
	return mPossibleMovesAndCaptures;
}


bool MoveHelper::checkDiagonalMoves(const Position &position, ChessBoard &board, const Player& player)
{
	return checkMovesInDirection(position, board, player, mDiagonalDirections, false);
}


bool MoveHelper::checkAdjacentMoves(const Position &position, ChessBoard &board, const Player &player)
{
	return checkMovesInDirection(position, board, player, mAdjacentPositions, true);
}


bool MoveHelper::checkLinearForwardMove(const Position &position, ChessBoard &board, const Player &player)
{
	return false;
}


bool MoveHelper::checkPawnCaptureMoves(const Position &position, ChessBoard &board, const Player &player)
{
	return false;
}


bool MoveHelper::checkLShapedMoves(const Position &position, ChessBoard &board, const Player &player)
{
	return checkMovesInDirection(position, board, player, mLShapedDirections, true);
}


bool MoveHelper::checkFileMoves(const Position &position, ChessBoard &board, const Player &player)
{
	return checkMovesInDirection(position, board, player, mFileDirections, false);
}


template <std::size_t N>
bool MoveHelper::checkMovesInDirection(const Position &position, ChessBoard &board, const Player &player, const std::array<std::pair<int, int>, N> &directions, bool oneStep)
{
	for (const auto &dir : directions)
	{
		int newX = position.x + dir.first;
		int newY = position.y + dir.second;

		while (checkForBorders(newX, newY))
		{
			Move move;

			if (board.isEmpty(newX, newY))
			{
				move.X = newX;
				move.Y = newY;
				addToAvailableMoves(move);
			}
			else
			{
				if (board.getPiece(newX, newY)->getColor() != player.getPlayerColor())
				{
					move.X				 = newX;
					move.Y				 = newY;
					move.canCapturePiece = true;
					addToAvailableMoves(move);
					break;
				}
			}

			if (oneStep)
			{
				break; // Stop after one step in this direction
			}

			newX += dir.first;
			newY += dir.second;
		}
	}

	return mPossibleMovesAndCaptures.empty();
}


bool MoveHelper::checkForBorders(const int x, const int y)
{
	return (x >= 0 && x <= 7 && y >= 0 && y <= 7);
}


bool MoveHelper::checkIfPositionAlreadyExists(const int x, const int y)
{
	for (const auto& pos : mPossibleMovesAndCaptures)
	{
		if (pos.X == x && pos.Y == y)
		{
			return true;
		}
	}
	return false;
}


void MoveHelper::addToAvailableMoves(Move &move)
{
	if (!checkIfPositionAlreadyExists(move.X, move.Y))
	{
		mPossibleMovesAndCaptures.push_back(move);
	}
}
