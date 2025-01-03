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


bool MoveHelper::checkAvailableMoves(const Position &position, ChessBoard &board, const PlayerColor color, PieceType piece, bool hasMoved)
{

	switch (piece)
	{
	case (PieceType::Pawn):
	{
		checkPawnMovement(position, board, color, hasMoved);
		checkPawnCaptureMovement(position, board, color);
		break;
	}

	case (PieceType::Knight):
	{
		checkLShapedMoves(position, board, color);
		break;
	}

	case (PieceType::Bishop):
	{
		checkDiagonalMoves(position, board, color);
		break;
	}

	case (PieceType::Rook):
	{
		checkFileMoves(position, board, color);
		break;
	}

	case (PieceType::Queen):
	{
		checkDiagonalMoves(position, board, color);
		checkAdjacentMoves(position, board, color);
		checkFileMoves(position, board, color);
		break;
	}

	case (PieceType::King):
	{
		checkAdjacentMoves(position, board, color);
		break;
	}

	default: break;
	}

	return mPossibleMovesAndCaptures.size() != 0; // maybe need to be changed
}


std::vector<PossibleMove> MoveHelper::getAvailableMoves()
{
	return mPossibleMovesAndCaptures;
}


bool MoveHelper::checkPawnMovement(const Position &position, ChessBoard &board, const PlayerColor color, bool hasMoved)
{
	int		 colorFactor = (color == PlayerColor::White) ? 1 : -1;

	int		 newX		 = position.x + mPawnMoveDirections[0].first;
	int		 newY		 = position.y + mPawnMoveDirections[0].second * colorFactor;

	Position newPosition = {newX, newY};

	if (checkForBorders(newX, newY) && board.isEmpty(newPosition))
	{
		PossibleMove move;
		move.start = position;
		move.end   = newPosition;

		if ((color == PlayerColor::White && newY == 8) || (color == PlayerColor::Black && newY == 1))
		{
			move.type = MoveType::PawnPromotion;
		}

		addToAvailableMoves(move);

		// If it's the pawn's first move, check for the two-step option
		if (!hasMoved)
		{
			newX		= position.x + mPawnMoveDirections[1].first;
			newY		= position.y + mPawnMoveDirections[1].second * colorFactor;

			newPosition = {newX, newY};

			if (checkForBorders(newX, newY) && board.isEmpty(newPosition))
			{
				move.end  = newPosition;
				move.type = MoveType::DoublePawnPush;
				addToAvailableMoves(move);
			}
		}
	}

	return mPossibleMovesAndCaptures.empty();
}


bool MoveHelper::checkPawnCaptureMovement(const Position &position, ChessBoard &board, const PlayerColor color)
{
	int colorFactor = (color == PlayerColor::White) ? 1 : -1;

	for (const auto &dir : mPawnCaptureDirections)
	{
		int		 newX		 = position.x + dir.first;
		int		 newY		 = position.y + dir.second * colorFactor;

		Position newPosition = {newX, newY};

		if (checkForBorders(newX, newY) && !board.isEmpty(newPosition))
		{
			const auto &piece = board.getPiece(newPosition);
			if (piece->getColor() != color)
			{
				PossibleMove move;
				move.start = position;
				move.end   = newPosition;
				move.type  = MoveType::Capture;
				addToAvailableMoves(move);
			}
		}
	}

	return mPossibleMovesAndCaptures.empty();
}


bool MoveHelper::checkDiagonalMoves(const Position &position, ChessBoard &board, const PlayerColor color)
{
	return checkMovesInDirection(position, board, color, mDiagonalDirections, false);
}


bool MoveHelper::checkAdjacentMoves(const Position &position, ChessBoard &board, const PlayerColor color)
{
	return checkMovesInDirection(position, board, color, mAdjacentPositions, true);
}


bool MoveHelper::checkLShapedMoves(const Position &position, ChessBoard &board, const PlayerColor color)
{
	return checkMovesInDirection(position, board, color, mLShapedDirections, true);
}


bool MoveHelper::checkFileMoves(const Position &position, ChessBoard &board, const PlayerColor color)
{
	return checkMovesInDirection(position, board, color, mFileDirections, false);
}


template <std::size_t N>
bool MoveHelper::checkMovesInDirection(const Position &position, ChessBoard &board, const PlayerColor color, const std::array<std::pair<int, int>, N> &directions, bool oneStep)
{
	for (const auto &dir : directions)
	{
		int		 newX		= position.x + dir.first;
		int		 newY		= position.y + dir.second;

		Position newPostion = {newX, newY};

		while (checkForBorders(newX, newY))
		{
			PossibleMove move;
			if (board.isEmpty(newPostion))
			{
				move.start = position;
				move.end   = newPostion;
				addToAvailableMoves(move);
			}
			else
			{
				if (board.getPiece(newPostion)->getColor() != color)
				{
					move.start = position;
					move.end   = newPostion;
					move.type  = MoveType::Capture;
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
	return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE);
}


bool MoveHelper::checkIfPositionAlreadyExists(const Position pos)
{
	for (const auto &possibleMove : mPossibleMovesAndCaptures)
	{
		if (possibleMove.end.x == pos.x && possibleMove.end.y == pos.y)
		{
			return true;
		}
	}
	return false;
}


void MoveHelper::addToAvailableMoves(PossibleMove &move)
{
	if (!checkIfPositionAlreadyExists(move.end))
	{
		mPossibleMovesAndCaptures.push_back(move);
	}
}
