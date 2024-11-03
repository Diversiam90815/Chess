/*
  ==============================================================================

	Class:          Player

	Description:    Stats and settings for the player

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"


class Player
{
public:
	Player();

	Player(PieceColor color);

	~Player();


	bool		  isOnTurn() const;
	void		  setOnTurn(bool value);

	int			  getScore() const;
	void		  setScore(int value);

	PieceColor	  getPlayerColor() const;
	void		  setPlayerColor(PieceColor value);


	void		  addCapturedPiece(const PieceType piece);

	void		  updateScore();

	constexpr int getPieceValue(PieceType piece);

private:
	PieceColor			   mPlayerColor = PieceColor::NoColor;

	std::vector<PieceType> mCapturedPieces;

	bool				   mIsCurrentTurn = false;

	int					   mScore		  = 0;
};
