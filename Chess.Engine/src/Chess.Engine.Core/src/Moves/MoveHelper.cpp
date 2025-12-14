/*
  ==============================================================================
	Module:         MoveHelper
	Description:    Helper class for generating general moves in chess
  ==============================================================================
*/


#include "MoveHelper.h"

//
// MoveHelper::MoveHelper() {}
//
//
// MoveHelper::~MoveHelper() {}
//
//
// bool MoveHelper::checkAvailableMoves(const Position &position, ChessBoard &board, const PlayerColor color, PieceType piece, bool hasMoved, bool attackOnly)
//{
//
//	switch (piece)
//	{
//	case (PieceType::Pawn):
//	{
//		if (attackOnly)
//		{
//			checkPawnCaptureMovement(position, board, color); // For possible check validation we need to differentiate the pawn's movements
//			break;
//		}
//		checkPawnMovement(position, board, color, hasMoved);
//		checkPawnCaptureMovement(position, board, color);
//		break;
//	}
//
//	case (PieceType::Knight):
//	{
//		checkLShapedMoves(position, board, color);
//		break;
//	}
//
//	case (PieceType::Bishop):
//	{
//		checkDiagonalMoves(position, board, color);
//		break;
//	}
//
//	case (PieceType::Rook):
//	{
//		checkFileMoves(position, board, color);
//		break;
//	}
//
//	case (PieceType::Queen):
//	{
//		checkDiagonalMoves(position, board, color);
//		checkAdjacentMoves(position, board, color);
//		checkFileMoves(position, board, color);
//		break;
//	}
//
//	case (PieceType::King):
//	{
//		checkAdjacentMoves(position, board, color);
//		break;
//	}
//
//	default: break;
//	}
//
//	return mPossibleMovesAndCaptures.size() != 0; // maybe need to be changed
//}
//
//
// std::vector<PossibleMove> MoveHelper::getAvailableMoves()
//{
//	return mPossibleMovesAndCaptures;
//}
//
//
// bool MoveHelper::checkPawnMovement(const Position &position, ChessBoard &board, const PlayerColor color, bool hasMoved)
//{
//	int		 colorFactor = (color == PlayerColor::White) ? -1 : +1;
//
//	int		 newX		 = position.x + mPawnMoveDirections[0].first;
//	int		 newY		 = position.y + mPawnMoveDirections[0].second * colorFactor;
//
//	Position newPosition = {newX, newY};
//
//	if (checkForBorders(newX, newY) && board.isEmpty(newPosition))
//	{
//		PossibleMove move;
//		move.start = position;
//		move.end   = newPosition;
//
//		if ((color == PlayerColor::White && newY == 0) || (color == PlayerColor::Black && newY == 7))
//		{
//			move.type = MoveType::PawnPromotion;
//		}
//
//		addToAvailableMoves(move);
//
//		// If it's the pawn's first move, check for the two-step option
//		if (!hasMoved)
//		{
//			newX		= position.x + mPawnMoveDirections[1].first;
//			newY		= position.y + mPawnMoveDirections[1].second * colorFactor;
//
//			newPosition = {newX, newY};
//
//			if (checkForBorders(newX, newY) && board.isEmpty(newPosition))
//			{
//				move.end  = newPosition;
//				move.type = MoveType::DoublePawnPush;
//				addToAvailableMoves(move);
//			}
//		}
//	}
//
//	return mPossibleMovesAndCaptures.empty();
//}
//
//
// bool MoveHelper::checkPawnCaptureMovement(const Position &position, ChessBoard &board, const PlayerColor color)
//{
//	int colorFactor = (color == PlayerColor::White) ? -1 : +1;
//
//	for (const auto &dir : mPawnCaptureDirections)
//	{
//		int		 newX		 = position.x + dir.first;
//		int		 newY		 = position.y + dir.second * colorFactor;
//
//		Position newPosition = {newX, newY};
//
//		if (checkForBorders(newX, newY) && !board.isEmpty(newPosition))
//		{
//			const auto &piece = board.getPiece(newPosition);
//			if (piece->getColor() != color)
//			{
//				PossibleMove move;
//				move.start = position;
//				move.end   = newPosition;
//				move.type  = MoveType::Capture;
//
//				if ((color == PlayerColor::White && newY == 0) || (color == PlayerColor::Black && newY == 7))
//				{
//					move.type |= MoveType::PawnPromotion;
//				}
//
//				addToAvailableMoves(move);
//			}
//		}
//	}
//
//	return mPossibleMovesAndCaptures.empty();
//}
//
//
// bool MoveHelper::checkDiagonalMoves(const Position &position, ChessBoard &board, const PlayerColor color)
//{
//	return checkMovesInDirection(position, board, color, mDiagonalDirections, false);
//}
//
//
// bool MoveHelper::checkAdjacentMoves(const Position &position, ChessBoard &board, const PlayerColor color)
//{
//	return checkMovesInDirection(position, board, color, mAdjacentPositions, true);
//}
//
//
// bool MoveHelper::checkLShapedMoves(const Position &position, ChessBoard &board, const PlayerColor color)
//{
//	return checkMovesInDirection(position, board, color, mLShapedDirections, true);
//}
//
//
// bool MoveHelper::checkFileMoves(const Position &position, ChessBoard &board, const PlayerColor color)
//{
//	return checkMovesInDirection(position, board, color, mFileDirections, false);
//}
//
//
// template <std::size_t N>
// bool MoveHelper::checkMovesInDirection(const Position &position, ChessBoard &board, const PlayerColor color, const std::array<std::pair<int, int>, N> &directions, bool oneStep)
//{
//	for (const auto &dir : directions)
//	{
//		int newX = position.x + dir.first;
//		int newY = position.y + dir.second;
//
//
//		while (checkForBorders(newX, newY))
//		{
//			Position	 newPostion = {newX, newY};
//
//			PossibleMove move;
//			if (board.isEmpty(newPostion))
//			{
//				move.start = position;
//				move.end   = newPostion;
//				addToAvailableMoves(move);
//			}
//			else
//			{
//				if (board.getPiece(newPostion)->getColor() != color)
//				{
//					move.start = position;
//					move.end   = newPostion;
//					move.type  = MoveType::Capture;
//					addToAvailableMoves(move);
//					break;
//				}
//				// If it's our own piece or we captured an opponent, either way we stop going further
//				break;
//			}
//
//			if (oneStep)
//			{
//				break; // Stop after one step in this direction
//			}
//
//			newX += dir.first;
//			newY += dir.second;
//		}
//	}
//
//	return mPossibleMovesAndCaptures.empty();
//}
//
//
// bool MoveHelper::checkForBorders(const int x, const int y)
//{
//	return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE);
//}
//
//
// bool MoveHelper::checkIfPositionAlreadyExists(const Position pos)
//{
//	for (const auto &possibleMove : mPossibleMovesAndCaptures)
//	{
//		if (possibleMove.end == pos)
//		{
//			return true;
//		}
//	}
//	return false;
//}
//
//
// void MoveHelper::addToAvailableMoves(PossibleMove &move)
//{
//	if (!checkIfPositionAlreadyExists(move.end))
//	{
//		mPossibleMovesAndCaptures.push_back(move);
//	}
//}


void MoveHelper::initPawnAttackTable()
{
	for (int square = 0; square < 64; ++square)
	{
		mPawnAttacks[Side::White][square] = maskPawnAttacks(Side::White, square);
		mPawnAttacks[Side::Black][square] = maskPawnAttacks(Side::Black, square);
	}
}


U64 MoveHelper::maskPawnAttacks(int side, int square)
{
	// result attacks bitboard
	U64 attacks	 = 0ULL;

	// piece bitboard
	U64 bitboard = 0ULL;

	// Set bit on board
	set_bit(bitboard, static_cast<int>(square));

	// white pawns
	if (!(side == Side::White))
	{
		if ((bitboard >> 7) & not_A_file)
			attacks |= (bitboard >> 7);

		if ((bitboard >> 9) & not_H_file)
			attacks |= (bitboard >> 9);
	}
	// black pawns
	else
	{
		if ((bitboard << 7) & not_A_file)
			attacks |= (bitboard << 7);

		if ((bitboard << 9) & not_H_file)
			attacks |= (bitboard << 9);
	}
}
