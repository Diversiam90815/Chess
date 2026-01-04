/*
  ==============================================================================
	Module:         MoveValidation
	Description:    Move legality validation
  ==============================================================================
*/

#include "MoveValidation.h"


MoveValidation::MoveValidation(Chessboard &board, MoveGeneration &generation, MoveExecution &execution) : mBoard(board), mGeneration(generation), mExecution(execution) {}


bool MoveValidation::isInCheck() const
{
	return isKingAttacked(mBoard.getCurrentSide());
}


bool MoveValidation::isKingAttacked(Side side) const
{
	Square kingPos = getKingSquare(side);

	if (kingPos == Square::None)
		return false;

	Side attacker = (side == Side::White) ? Side::Black : Side::White;
	return mGeneration.isSquareAttacked(kingPos, attacker);
}


bool MoveValidation::isMoveLegal(Move move)
{
	// Make move
	if (!mExecution.makeMove(move))
		return false;

	// Check if the side that just moved left king in check
	// Note: makeMove flips the side, so we use the opposite side
	Side movedSide	 = (mBoard.getCurrentSide() == Side::White) ? Side::Black : Side::White;

	bool kingInCheck = isKingAttacked(movedSide);

	mExecution.unmakeMove();

	return !kingInCheck;
}


bool MoveValidation::isCheckmate()
{
	if (!isInCheck())
		return false;

	return countLegalMoves() == 0;
}


bool MoveValidation::isStalemate()
{
	if (isInCheck())
		return false;

	return countLegalMoves() == 0;
}


bool MoveValidation::isDraw() const
{
	// 50-move rule
	if (mBoard.getHalfMoveClock() >= 100)
		return true;

	// Insufficient material
	if (hasInsufficientMaterial())
		return true;

	// TODO: Threefold repetition (Zobrist hashing)

	return false;
}


void MoveValidation::generateLegalMoves(MoveList &legalMoves)
{
	MoveList pseudoMoves;
	mGeneration.generateAllMoves(pseudoMoves);

	legalMoves.clear();

	for (size_t i = 0; i < pseudoMoves.size(); ++i)
	{
		if (isMoveLegal(pseudoMoves[i]))
			legalMoves.push(pseudoMoves[i]);
	}
}


size_t MoveValidation::countLegalMoves()
{
	MoveList pseudoMoves;
	mGeneration.generateAllMoves(pseudoMoves);

	size_t count = 0;
	for (size_t i = 0; i < pseudoMoves.size(); ++i)
	{
		if (isMoveLegal(pseudoMoves[i]))
		{
			count++;
			// return count;	// TODO: Early exit for optimization
		}
	}

	return count;
}


Square MoveValidation::getKingSquare(Side side) const
{
	PieceTypes kingType = (side == Side::White) ? WKing : BKing;
	U64		   kingBB	= mBoard.pieces()[kingType];

	if (!kingBB)
		return Square::None;

	return static_cast<Square>(BitUtils::lsb(kingBB));
}


bool MoveValidation::hasInsufficientMaterial() const
{
	const auto &pieces = mBoard.pieces();

	// Any pawns, rooks or queens -> sufficient material
	if (pieces[WPawn] || pieces[BPawn] || pieces[WRook] || pieces[BRook] || pieces[WQueen] || pieces[BQueen])
		return false;

	// TODO: Check for insufficient material

	return false;
}
