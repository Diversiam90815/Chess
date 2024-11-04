/*
  ==============================================================================

	Class:          Move

	Description:    Storing information classifying a move in chess

  ==============================================================================
*/

#include "Move.h"
#include "MoveNotationHelper.h"


Move::Move()
{
}

Move::Move(PossibleMove &possibleMove) : startingPosition(possibleMove.start), endingPosition(possibleMove.end), type(possibleMove.type)
{
}

Move::Move(Position start, Position end, PieceType moved, PieceType captured, MoveType type, PieceType promotion)
	: startingPosition(start), endingPosition(end), movedPiece(moved), capturedPiece(captured), type(type), promotionType(promotion)
{
}
