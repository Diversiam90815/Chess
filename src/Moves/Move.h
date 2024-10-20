/*
  ==============================================================================

	Class:          Move

	Description:    Storing information classifying a move in chess

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"


struct Position
{
	int x = 0;
	int y = 0;
};


struct Move
{
	Move()
	{
	}

	Move(int x, int y) : X(x), Y(y)
	{
	}

	Move(
		int fx, int fy, int tx, int ty, PieceType moved, PieceType captured = PieceType::None, bool castling = false, bool enPassant = false, PieceType promotion = PieceType::None)
		: X(fx), Y(fy), toX(tx), toY(ty), movedPiece(moved), capturedPiece(captured), isCastling(castling), isEnPassant(enPassant), promotionType(promotion)
	{
	}


	int		  X				  = 0;
	int		  Y				  = 0;
	int		  toX			  = 0;
	int		  toY			  = 0;

	PieceType movedPiece	  = PieceType::None;
	PieceType capturedPiece	  = PieceType::None;
	PieceType promotionType	  = PieceType::None;

	bool	  isCastling	  = false;
	bool	  isEnPassant	  = false;
	bool	  canCapturePiece = false;
};
