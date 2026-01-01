/*
  ==============================================================================
	Module:         Move
	Description:    Storing information classifying a move in chess
  ==============================================================================
*/


#pragma once

#include <string>
#include <functional>

#include "Parameters.h"
#include "MoveType.h"
#include "BitboardTypes.h"


struct PossibleMove
{
	Square	  start{Square::None};
	Square	  end{Square::None};
	MoveType  type			 = MoveType::Normal;
	PieceType promotionPiece = PieceType::DefaultType;

	bool	  operator==(const PossibleMove &other) const { return this->start == other.start && this->end == other.end; }
	bool	  isEmpty() const { return start == Square::None && end == Square::None; }
};


class Move
{
public:
	Move() {}

	Move(PossibleMove &possibleMove)
	{
		this->startingPosition = possibleMove.start;
		this->endingPosition   = possibleMove.end;
		this->type			   = possibleMove.type;
	}

	Move(Square start, Square end, PieceType moved, PieceType captured = PieceType::DefaultType, MoveType type = MoveType::Normal, PieceType promotion = PieceType::DefaultType)
	{
		this->startingPosition = start;
		this->endingPosition   = end;
		this->movedPiece	   = moved;
		this->capturedPiece	   = captured;
		this->type			   = type;
		this->promotionType	   = promotion;
	}

	Square		startingPosition = Square::None;
	Square		endingPosition	 = Square::None;

	PieceType	movedPiece		 = PieceType::DefaultType;
	PieceType	capturedPiece	 = PieceType::DefaultType;
	PieceType	promotionType	 = PieceType::DefaultType;
	PlayerColor player			 = PlayerColor::None;

	MoveType	type			 = MoveType::Normal;

	std::string notation		 = "";

	int			number			 = 0; // Storing the current number of this move. Each move saved should increment this number!

	int			halfMoveClock	 = 0; // Incrememted with every move that is not a capture or pawn move (detect draw if halfMoveClock is 100)


	bool		operator<(const Move &other) const { return this->number < other.number; }

	bool		operator==(const Move &other) const { return this->number == other.number; }
};
