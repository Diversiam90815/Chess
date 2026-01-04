/*
  ==============================================================================
	Module:         InputEvent
	Description:    Thread-safe event structure for StateMachine
  ==============================================================================
*/

#pragma once

#include "BitboardTypes.h"
#include "Move.h"
#include "Parameters.h"

/**
 * @brief	Event structure for thread-safe input queuing.
 */
struct InputEvent
{
	enum class Type
	{
		None			= 0,
		SquareSelected	= 1,
		PromotionChosen = 2,
		RemoteMove		= 3,
		CPUMove			= 4,
		UndoRequested	= 5,
		GameStart		= 6,
		GameReset		= 7,
	};

	Type			  type		= Type::None;
	Square			  square	= Square::None;
	PieceTypes		  promotion = PieceTypes::None;
	Move			  move;
	GameConfiguration config{};


	static InputEvent SquareSelected(Square sq)
	{
		InputEvent e;
		e.type	 = Type::SquareSelected;
		e.square = sq;
		return e;
	}

	static InputEvent PromotionChosen(PieceTypes piece)
	{
		InputEvent e;
		e.type		= Type::PromotionChosen;
		e.promotion = piece;
		return e;
	}

	static InputEvent RemoteMove(Move m)
	{
		InputEvent e;
		e.type = Type::RemoteMove;
		e.move = m;
		return e;
	}

	static InputEvent CPUMove(Move m)
	{
		InputEvent e;
		e.type = Type::CPUMove;
		e.move = m;
		return e;
	}

	static InputEvent Undo()
	{
		InputEvent e;
		e.type = Type::UndoRequested;
		return e;
	}

	static InputEvent Start(GameConfiguration cfg)
	{
		InputEvent e;
		e.type	 = Type::GameStart;
		e.config = cfg;
		return e;
	}

	static InputEvent Reset()
	{
		InputEvent e;
		e.type = Type::GameReset;
		return e;
	}
};
