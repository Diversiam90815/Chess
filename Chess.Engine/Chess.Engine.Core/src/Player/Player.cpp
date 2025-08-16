/*
  ==============================================================================
	Module:         Player
	Description:    Stats and settings for the player
  ==============================================================================
*/


#include "Player.h"


Player::Player(PlayerColor color) : mPlayerColor(color)
{
	setScore(0);
}


Score Player::getScore() const
{
	return mScore;
}


void Player::setScore(int value)
{
	Score newScore = Score(this->getPlayerColor(), value);

	if (mScore != newScore)
	{
		mScore = newScore;
	}
}


PlayerColor Player::getPlayerColor() const
{
	return mPlayerColor;
}


void Player::setPlayerColor(PlayerColor value)
{
	if (mPlayerColor != value)
	{
		mPlayerColor = value;
	}
}


void Player::addCapturedPiece(const PieceType piece)
{
	mCapturedPieces.push_back(piece);
	updateScore();

	for (auto &observer : mObservers)
	{
		auto obs = observer.lock();

		if (obs)
			obs->onAddCapturedPiece(getPlayerColor(), piece);
	}
}


void Player::removeLastCapturedPiece()
{
	if (mCapturedPieces.empty())
	{
		// Handle the case where there are no captured pieces to remove
		LOG_WARNING("No captured pieces to remove.");
		return;
	}

	// Retrieve and remove the last captured piece
	PieceType lastCapture = mCapturedPieces.back();
	mCapturedPieces.pop_back();

	updateScore();

	for (auto &observer : mObservers)
	{
		auto obs = observer.lock();

		if (obs)
			obs->onRemoveLastCapturedPiece(getPlayerColor(), lastCapture);
	}
}


void Player::updateScore()
{
	int score = 0;
	for (auto piece : mCapturedPieces)
	{
		score += getPieceValue(piece);
	}
	setScore(score);

	for (auto &observer : mObservers)
	{
		auto obs = observer.lock();

		if (obs)
			obs->onScoreUpdate(mScore.getPlayerColor(), mScore.getValue());
	}

	LOG_INFO("Updated Score for {} : {}", LoggingHelper::playerColourToString(mPlayerColor).c_str(), score);
}


constexpr int Player::getPieceValue(PieceType piece)
{
	switch (piece)
	{
	case PieceType::Pawn: return pawnValue;
	case PieceType::Knight: return knightValue;
	case PieceType::Bishop: return bishopValue;
	case PieceType::Rook: return rookValue;
	case PieceType::Queen: return queenValue;
	case PieceType::King: return kingValue;
	default: return 0;
	}
}


void Player::reset()
{
	setScore(0);
	mCapturedPieces.clear();
}
