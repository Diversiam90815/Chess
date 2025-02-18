/*
  ==============================================================================
	Module:			MoveValidation
	Description:    Manages the validation of moves in chess
  ==============================================================================
*/

#pragma once

#include <memory>

#include "ChessBoard.h"
#include "Move.h"


class MoveValidation
{
public:
	MoveValidation(std::shared_ptr<ChessBoard> board);

	~MoveValidation();


private:
	std::shared_ptr<ChessBoard> mChessBoard;
};
