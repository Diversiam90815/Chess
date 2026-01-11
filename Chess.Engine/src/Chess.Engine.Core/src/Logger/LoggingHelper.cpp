/*
  ==============================================================================
	Module:         LoggingHelper
	Description:    Helper class for simplifying logging messages
  ==============================================================================
*/

#include "LoggingHelper.h"
#include "Logging.h"


std::string LoggingHelper::gameStateToString(GameState state)
{
	switch (state)
	{
	case GameState::Init: return "Init";
	case GameState::WaitingForInput: return "Waiting For Input";
	case GameState::WaitingForTarget: return "Waiting For Target";
	case GameState::GameOver: return "Game Over";
	case GameState::PawnPromotion: return "Pawn Promotion";
	case GameState::WaitingForRemoteMove: return "Waiting for remote move";
	default: return "Unknown GameState";
	}
}


std::string LoggingHelper::boolToString(const bool value)
{
	return value ? "True" : "False";
}


std::string LoggingHelper::connectionStateToString(const ConnectionState state)
{
	switch (state)
	{
	case ConnectionState::None: return "None";
	case ConnectionState::Disconnected: return "Disconnects";
	case ConnectionState::HostingSession: return "Hosting Session";
	case ConnectionState::WaitingForARemote: return "Waiting for a remote";
	case ConnectionState::Connected: return "Connected";
	case ConnectionState::Error: return "Error";
	case ConnectionState::ConnectionRequested: return "Connection requested";
	case ConnectionState::PendingHostApproval: return "Pending host approval";
	case ConnectionState::ClientFoundHost: return "Client found host";
	case ConnectionState::SetPlayerColor: return "Set player color";
	case ConnectionState::GameStarted: return "Game Started";
	default: return "Unknown ConnectionState";
	}
}


std::string LoggingHelper::sideToString(const Side side)
{
	switch (side)
	{
	case Side::None: return "None";
	case Side::White: return "White";
	case Side::Black: return "Black";
	case Side::Both: return "Both";
	default: return "Unknown Side";
	}
}


std::string LoggingHelper::cpuDifficultyToString(const CPUDifficulty diff)
{
	switch (diff)
	{
	case CPUDifficulty::Easy: return "Easy";
	case CPUDifficulty::Medium: return "Medium";
	case CPUDifficulty::Hard: return "Hard";
	default: return "Unknown Difficulty";
	}
}
