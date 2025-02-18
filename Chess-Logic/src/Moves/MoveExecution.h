/*
  ==============================================================================
	Module:			MoveExecution
	Description:    Manages the execution of moves in chess
  ==============================================================================
*/

#pragma once

#include <memory>
#include <set>

#include "ChessBoard.h"
#include "MoveNotationHelper.h"
#include "Move.h"


class MoveExecution
{
public:
	MoveExecution(std::shared_ptr<ChessBoard> board);
	~MoveExecution();

private:
	std::shared_ptr<ChessBoard> mChessboard;
};
