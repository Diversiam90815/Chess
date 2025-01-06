/*
  ==============================================================================

	Class:          MoveNotationHelper

	Description:    Transforming the Move class into a Standart Algebraic Notation string

  ==============================================================================
*/

#include "MoveNotationHelper.h"


MoveNotationHelper::MoveNotationHelper()
{
}


MoveNotationHelper::~MoveNotationHelper()
{
}


std::string MoveNotationHelper::generateStandardAlgebraicNotation(Move &move)
{
	// Handle castling
	if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
	{
		return castlingToSAN(move);
	}

	std::string notation;

	// Get the piece character
	char		pieceChar = getPieceType(move.movedPiece);

	// Determine if the move is a capture
	bool		isCapture = (move.type & MoveType::Capture) == MoveType::Capture;

	// Handle pawn moves
	if (move.movedPiece == PieceType::Pawn)
	{
		if (isCapture)
		{
			// For pawn captures, include the starting file
			char startFile = getFileFromPosition(move.startingPosition);
			notation += startFile;
			notation += 'x';
		}
		// Append the destination square
		notation += getPositionString(move.endingPosition);

		// Handle En Passant
		if ((move.type & MoveType::EnPassant) == MoveType::EnPassant)
		{
			notation += " e.p.";
		}

		// Handle promotion
		if ((move.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
		{
			notation += '=';
			notation += getPieceType(move.promotionType);
		}
	}
	else
	{
		// Include the piece character
		if (pieceChar != 0)
		{
			notation += pieceChar;
		}

		// Disambiguation is not implemented in this basic version

		if (isCapture)
		{
			notation += 'x';
		}

		// Append the destination square
		notation += getPositionString(move.endingPosition);
	}

	// Handle check and checkmate indicators
	if ((move.type & MoveType::Checkmate) == MoveType::Checkmate)
	{
		notation += '#';
	}
	else if ((move.type & MoveType::Check) == MoveType::Check)
	{
		notation += '+';
	}

	return notation;
}


std::string MoveNotationHelper::castlingToSAN(Move &move)
{
	if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside)
		return "O-O";

	else if ((move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		return "O-O-O";

	return "";
}


std::string MoveNotationHelper::getPositionString(Position &pos)
{
	std::string positionString;
	positionString += getFileFromPosition(pos);
	positionString += getRankFromPosition(pos);
	return positionString;
}


char MoveNotationHelper::getFileFromPosition(Position &pos)
{
	return 'a' + pos.x;
}


char MoveNotationHelper::getRankFromPosition(Position &pos)
{
	return '8' - pos.y;
}


char MoveNotationHelper::getPieceType(PieceType type)
{
	switch (type)
	{
	case PieceType::Pawn: return 0; // Pawns don't have a piece letter in SAN
	case PieceType::Knight: return 'N';
	case PieceType::Bishop: return 'B';
	case PieceType::Rook: return 'R';
	case PieceType::Queen: return 'Q';
	case PieceType::King: return 'K';
	default: return 0;
	}
}
