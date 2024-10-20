/*
  ==============================================================================

	Class:          ChessBoard

	Description:    Class charecterizing a virtual chess board

  ==============================================================================
*/

#pragma once
#include <array>
#include <memory>

#include "Move.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"


struct Square
{
	int							x;
	int							y;
	std::shared_ptr<ChessPiece> piece;
};


class ChessBoard
{
public:
	ChessBoard();
	~ChessBoard();


	void						initializeBoard();

	Square					   &getSquare(int x, int y);

	void						setPiece(int x, int y, std::shared_ptr<ChessPiece> piece);

	std::shared_ptr<ChessPiece> getPiece(int x, int y);

	void						removePiece(int x, int y);

	bool						movePiece(int fromX, int fromY, int toX, int toY);

	bool						isEmpty(int x, int y) const;

	const Move				   *getLastMove();

	void						addMoveToHistory(const Move &move);


private:
	std::vector<std::vector<Square>> squares;

	std::vector<Move>				 moveHistory;
};