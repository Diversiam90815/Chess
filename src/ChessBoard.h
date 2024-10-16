
#pragma once

#include "ChessPiece.h"
#include <array>
#include <memory>


class ChessBoard
{
private:
	// 8x8 board represented by a 2D array of pointers to ChessPiece
	std::array<std::array<std::shared_ptr<ChessPiece>, 8>, 8> board;

public:
	ChessBoard();
	~ChessBoard();

	std::shared_ptr<ChessPiece> getPiece(int x, int y) const;
	void						setPiece(int x, int y, std::shared_ptr<ChessPiece> piece);
	bool						isEmpty(int x, int y) const;

	bool						movePiece(int fromX, int fromY, int toX, int toY);
	bool						isValidMove(int fromX, int fromY, int toX, int toY) const;

	void						initializeBoard();
};