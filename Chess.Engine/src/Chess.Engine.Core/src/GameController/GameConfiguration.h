/*
  ==============================================================================
	Module:         Game Configuration
	Description:    Unifying multiple configuration specs
  ==============================================================================
*/

#pragma once

#include <assert.h>

#include "BitboardTypes.h"
#include "Parameters.h"


// Configuration of local two-player mode.
struct LocalCoopConfig
{
	// might add settings later such as time controls, board orientation,...
};


// Configuration for single-player mode against computer AI.
struct SinglePlayerConfig
{
	Side		  humanPlayerColor;
	CPUDifficulty aiDifficulty;
};


// Configuration of networked multiplayer mode.
struct MultiplayerConfig
{
	Side localPlayerColor;
};


//=============================================================================
// Unified Configuration
//=============================================================================

enum class GameModeSelection
{
	None		 = 0,
	LocalCoop	 = 1,
	SinglePlayer = 2,
	Multiplayer	 = 3
};


struct GameConfiguration
{
	GameModeSelection mode;

	union
	{
		LocalCoopConfig	   localCoop;
		SinglePlayerConfig singlePlayer;
		MultiplayerConfig  multiPlayer;
	};


	//=========================================================================
	// Factory Methods
	//=========================================================================

	static GameConfiguration createLocalCoop()
	{
		GameConfiguration config{};
		config.mode		 = GameModeSelection::LocalCoop;
		config.localCoop = LocalCoopConfig{};
		return config;
	}

	static GameConfiguration createSinglePlayer(Side humanColor, CPUDifficulty difficulty)
	{
		GameConfiguration config{};
		config.mode							 = GameModeSelection::SinglePlayer;
		config.singlePlayer.humanPlayerColor = humanColor;
		config.singlePlayer.aiDifficulty	 = difficulty;
		return config;
	}

	static GameConfiguration createMultiplayer(Side localColor)
	{
		GameConfiguration config{};
		config.mode							= GameModeSelection::Multiplayer;
		config.multiPlayer.localPlayerColor = localColor;
		return config;
	}

	
    //=========================================================================
	// Accessors
	//=========================================================================

	const SinglePlayerConfig &getSinglePlayer() const
	{
		assert(mode == GameModeSelection::SinglePlayer);
		return singlePlayer;
	}

	const MultiplayerConfig &getMultiplayer() const
	{
		assert(mode == GameModeSelection::Multiplayer);
		return multiPlayer;
	}
};
