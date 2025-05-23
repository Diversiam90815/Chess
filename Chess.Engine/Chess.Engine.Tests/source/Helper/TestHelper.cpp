/*
  ==============================================================================
	Module:			Test Helper
	Description:    Helper methods for the testing modules
  ==============================================================================
*/

#include "TestHelper.h"


std::shared_ptr<ChessBoard> CreateDefaultBoard()
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();
	return board;
}
