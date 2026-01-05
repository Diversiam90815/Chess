/*
  ==============================================================================
	Module:         MoveNotation
	Description:    Transforming the Move class into a Standart Algebraic Notation string
  ==============================================================================
*/

#include "MoveNotation.h"


std::string MoveNotation::toSAN(Move &move, const Chessboard &board, bool isCheck, bool isCheckmate) const
{
	MoveFlag flags = move.flags();

	// handle castling
	if (flags == MoveFlag::KingCastle)
		return "O-O";
	else if (flags == MoveFlag::QueenCastle)
		return "O-O-O";

	std::string notation;

	Square		from   = move.from();
	Square		to	   = move.to();
	PieceType	piece  = board.pieceAt(from);

	bool		isPawn = (piece == WPawn || piece == BPawn);

	if (!isPawn)
	{
		char pieceChar = pieceToSANChar(piece);
		if (pieceChar != '\0')
			notation += pieceChar;

		// TODO: Handle disambiguation
	}

	// capture notation
	if (move.isCapture())
	{
		if (isPawn)
			notation += getFile(from); // include starting file for pawn captures

		notation += 'x';
	}

	// destination square
	notation += squareToString(to);

	// promotion
	if (move.isPromotion())
	{
		notation += '=';
		int				  promoOffset  = move.promotionPieceOffset(); // 0 = Knight, 1 = Bishop, 2 = Rook, 3 = Queen
		static const char promoChars[] = {'N', 'B', 'R', 'Q'};
		notation += promoChars[promoOffset];
	}

	// check/mate indicators
	if (isCheckmate)
		notation += '#';
	else if (isCheck)
		notation += '+';

	return notation;
}


std::string MoveNotation::toUCI(Move move) const
{
	std::string uci;

	uci += squareToString(move.from());
	uci += squareToString(move.to());

	if (move.isPromotion())
	{
		int				  promoOffset  = move.promotionPieceOffset();
		static const char promoChars[] = {'n', 'b', 'r', 'q'};
		uci += promoChars[promoOffset];
	}

	return uci;
}