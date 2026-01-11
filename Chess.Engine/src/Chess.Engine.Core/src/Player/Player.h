/*
  ==============================================================================
	Module:         Player
	Description:    Stats and settings for the player
  ==============================================================================
*/


#pragma once

#include "Parameters.h"
#include "Logging.h"

#include "IObservable.h"


/**
 * @brief	Lightweight aggregation of per-player state (material score, captured pieces,
 *			color assignment, local/remote designation).
 *
 * Responsibilities:
 *  - Track captured opponent pieces for UI / analytics.
 *  - Distinguish local vs remote (used by multiplayer logic / input gating).
 */
class Player : public IPlayerObservable
{
public:
	Player() = default;
	Player(Side color);
	~Player() = default;

	Side getPlayerColor() const;
	void setPlayerColor(Side value);

	/**
	 * @brief	Record a newly captured opponent piece.
	 */
	void addCapturedPiece(const PieceType piece) override;

	/**
	 * @brief	Remove the most recently captured piece (undo support).
	 */
	void removeLastCapturedPiece() override;

	/**
	 * @brief	Reset player state to initial (fresh game).
	 */
	void reset();

	bool isLocalPlayer() const { return mIsLocalPlayer; }
	void setIsLocalPlayer(const bool isLocal) { mIsLocalPlayer = isLocal; }

private:
	Side				   mSide = Side::None;

	std::vector<PieceType> mCapturedPieces;

	bool				   mIsCurrentTurn = false;

	bool				   mIsLocalPlayer{true}; // Default to local player in single-player mode
};
