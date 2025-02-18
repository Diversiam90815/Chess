/*
  ==============================================================================
	Module:			MoveExecution
	Description:    Manages the execution of moves in chess
  ==============================================================================
*/

#include "MoveExecution.h"


MoveExecution::MoveExecution(std::shared_ptr<ChessBoard> board) : mChessboard(board) {}


MoveExecution::~MoveExecution() {}
