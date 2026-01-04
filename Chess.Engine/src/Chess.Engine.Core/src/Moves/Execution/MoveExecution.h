/*
  ==============================================================================
	Module:			MoveExecution
	Description:    Manages the execution of moves in chess
  ==============================================================================
*/

#pragma once

#include <vector>

#include "ChessBoard.h"
#include "Notation/MoveNotation.h"
#include "Move.h"
#include "IObservable.h"
#include "Logging.h"


struct MoveHistoryEntry
{
	Move	   move;
	BoardState previousState;
};


class MoveExecution
{
public:
	explicit MoveExecution(Chessboard &board);
	~MoveExecution() = default;

	// Executes a move on the board, returns false if invalid
	bool											   makeMove(Move move);

	// Undo the last move
	bool											   unmakeMove();

	// History
	[[nodiscard]] const MoveHistoryEntry			  *getLastMove() const;
	[[nodiscard]] size_t							   historySize() const { return mHistory.size(); }
	[[nodiscard]] const std::vector<MoveHistoryEntry> &getHistory() const { return mHistory; }
	void											   clearHistory() { mHistory.clear(); }

private:
	Chessboard					 &mChessBoard;

	std::vector<MoveHistoryEntry> mHistory;

	// clang-format off
	/*
				Castling rights update table

	a1	(white queenside rook)		7		0111	Clears WQ bit 
	h1	(white kingside rook)		11		1011	Clears WK bit 
	e1	(white king)				3		0011	Clears both WK and WQ
	a8	(black queenside rook)		13		1101	Clears BQ bit 
	h8	(black kingside rook)		14		1110	Clears BK bit 
	e8	(black king)				12		1100	Clears both BK and BQ 
	All other squares				15		1111	No change
	*/
	static constexpr uint8_t	  sCastlingRightsUpdate[64] = 
	{
		 13, 15, 15, 15, 12, 15, 15, 14,	// a8-h8
		 15, 15, 15, 15, 15, 15, 15, 15, 
		 15, 15, 15, 15, 15, 15, 15, 15, 
		 15, 15, 15, 15, 15, 15, 15, 15, 
		 15, 15, 15, 15, 15, 15, 15, 15, 
		 15, 15, 15, 15, 15, 15, 15, 15, 
		 15, 15, 15, 15, 15, 15, 15, 15, 
		 7,	 15, 15, 15, 3,	 15, 15, 11		// a1-h1
	 };
	// clang-format on
};
