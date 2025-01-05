/*
  ==============================================================================

	Class:          ChessPiece

	Description:    Virtual class defining a generalized chess piece

  ==============================================================================
*/

#include "ChessPiece.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"


std::shared_ptr<ChessPiece> ChessPiece::CreatePiece(PieceType type, PlayerColor color)
{
	switch (type)
	{
	case PieceType::Pawn: return std::make_shared<Pawn>(color);
	case PieceType::Rook: return std::make_shared<Rook>(color);
	case PieceType::Knight: return std::make_shared<Knight>(color);
	case PieceType::Bishop: return std::make_shared<Bishop>(color);
	case PieceType::Queen: return std::make_shared<Queen>(color);
	case PieceType::King: return std::make_shared<King>(color);
	default: return nullptr;
	}
}
