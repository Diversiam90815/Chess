/*
  ==============================================================================

	Class:          Player

	Description:    Stats and settings for the player

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"


struct Player
{
	Player()
	{
	}

	Player(PieceColor color) : playerColor(color)
	{
	}

	~Player() = default;


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
