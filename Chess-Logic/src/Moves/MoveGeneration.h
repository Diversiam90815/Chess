/*
  ==============================================================================
	Module:         MoveGeneration
	Description:    Manages the generation of moves
  ==============================================================================
*/

#pragma once

#include <vector>
#include <unordered_map>
#include <future>

#include "ChessBoard.h"
#include "Move.h"


class MoveGeneration
{
public:
	MoveGeneration(std::shared_ptr<ChessBoard> board);
	~MoveGeneration();


private:
	std::shared_ptr<ChessBoard> mChessboard;
};
