/*
  ==============================================================================

	Class:          MovementManager

	Description:    Manages and executes valid moves in chess

  ==============================================================================
*/


#pragma once

#include "ChessBoard.h"
#include "Move.h"
#include <set>
#include <unordered_map>


class MovementManager
{
public:
	MovementManager(ChessBoard &board);

	//bool isValidMove(const Move &move, PieceColor playerColor);
	//bool executeMove(const Move &move);

	//bool isKingInCheck(PieceColor color);
	//bool isCheckmate(PieceColor color);
	//bool isStalemate(PieceColor color);

	std::unordered_map<Position, std::vector<PossibleMove>> getAllPossibleMoves(PieceColor playerColor);

private:
	ChessBoard &board;

	bool		wouldKingBeInCheckAfterMove(Move &move, PieceColor playerColor);

	//bool		wouldKingBeInCheckAfterMove(const Move &move, PieceColor color);
	//bool		isSquareUnderAttack(int x, int y, PieceColor color);

	//bool		isCastlingMove(const Move &move, PieceColor color);
	//bool		validateCastling(const Move &move, PieceColor color);
	//void		performCastling(const Move &move);

	//bool		isEnPassantMove(const Move &move, PieceColor color);
	//bool		validateEnPassant(const Move &move, PieceColor color);
	//void		performEnPassant(const Move &move);

	//void		promotePawn(int x, int y, PieceColor color, PieceType promotionType);
};
