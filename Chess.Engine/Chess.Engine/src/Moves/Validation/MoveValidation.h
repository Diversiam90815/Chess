/*
  ==============================================================================
	Module:			MoveValidation
	Description:    Manages the validation of moves in chess
  ==============================================================================
*/

#pragma once

#include <memory>

#include "ChessBoard.h"


class MoveValidation
{
public:
	MoveValidation() = default;
	MoveValidation(std::shared_ptr<ChessBoard> board);

	~MoveValidation();

	void init(std::shared_ptr<ChessBoard> board);

	bool validateMove(Move &move, PlayerColor playerColor);

	bool isKingInCheck(const Position &ourKing, PlayerColor playerColor);

	bool isCheckmate(PlayerColor player);

	bool isStalemate(PlayerColor player);

	bool wouldKingBeInCheckAfterMove(Move &move, PlayerColor playerColor);


private:

	bool								isSquareAttacked(const Position &square, PlayerColor attackerColor);
	bool								isSquareAttacked(const Position &square, PlayerColor attackerColor, ChessBoard &chessboard);

	std::shared_ptr<ChessBoard>			mChessBoard;
};
