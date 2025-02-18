/*
  ==============================================================================
	Module:			MoveValidation
	Description:    Manages the validation of moves in chess
  ==============================================================================
*/


#include "MoveValidation.h"


MoveValidation::MoveValidation(std::shared_ptr<ChessBoard> board) : mChessBoard(board) {}


MoveValidation::~MoveValidation() {}
