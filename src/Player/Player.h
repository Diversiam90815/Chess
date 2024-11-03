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

	Player(PlayerColor color);

	~Player();


	bool		  isOnTurn() const;
	void		  setOnTurn(bool value);

	int			  getScore() const;
	void		  setScore(int value);

	PlayerColor	  getPlayerColor() const;
	void		  setPlayerColor(PlayerColor value);


	void		  addCapturedPiece(const PieceType piece);

	void		  updateScore();

	constexpr int getPieceValue(PieceType piece);

private:
	PlayerColor			   mPlayerColor = PlayerColor::NoColor;

	std::vector<PieceType> mCapturedPieces;

	bool				   mIsCurrentTurn = false;

	int					   mScore		  = 0;
};
