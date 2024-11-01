/*
  ==============================================================================

	Class:          Move

	Description:    Storing information classifying a move in chess

  ==============================================================================
*/


#pragma once

#include "Parameters.h"


struct Position
{
	int x = 0;
	int y = 0;
};


struct Move
{
	Move()
	{
	}

	Move(Position  start,
		 Position  end,
		 PieceType moved,
		 PieceType captured	 = PieceType::DefaultType,
		 bool	   castling	 = false,
		 bool	   enPassant = false,
		 PieceType promotion = PieceType::DefaultType)
		: startingPosition(start), endingPosition(end), movedPiece(moved), capturedPiece(captured), isCastling(castling), isEnPassant(enPassant), promotionType(promotion)
	{
	}

	Position  startingPosition;
	Position  endingPosition;

	PieceType movedPiece	= PieceType::DefaultType;
	PieceType capturedPiece = PieceType::DefaultType;
	PieceType promotionType = PieceType::DefaultType;

	bool	  isCastling	= false;
	bool	  isEnPassant	= false;
};


struct PossibleMove
{
	Position pos;
	bool	 canCapturePiece = false;
};