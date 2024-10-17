#pragma once
#include "ChessPiece.h"

struct Player
{
	Player(PieceColor color) : playerColor(color)
	{
	}

	bool isOnTurn() const
	{
		return isCurrentTurn;
	}

	void setOnTurn(bool value)
	{
		if(isCurrentTurn != value)
		{
			isCurrentTurn = value;
		}
	}


	PieceColor				playerColor = PieceColor::None;
	std::vector<ChessPiece> capturedPieces;
	bool					isCurrentTurn = false;
	int						score		  = 0;
};
