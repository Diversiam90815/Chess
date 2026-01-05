/*
  ==============================================================================
	Module:         Parameters
	Description:    Parameters and constants definitions
  ==============================================================================
*/

#pragma once

#include <array>

constexpr int BOARD_SIZE		= 8;
constexpr int PLAYER_PIECES_NUM = 2 * BOARD_SIZE;


/// <summary>
/// Represents the possible end states of a chess game.
/// </summary>
enum class EndGameState
{
	OnGoing	  = 1,
	Checkmate = 2,
	StaleMate = 3,
	Draw	  = 4,
	Reset	  = 5
};


/// <summary>
/// Represents the various states of a game used in the StateMachine.
/// </summary>
enum class GameState
{
	Undefined			 = 0,
	Init				 = 1,
	InitSucceeded		 = 2,
	WaitingForInput		 = 3,
	MoveInitiated		 = 4,
	WaitingForTarget	 = 5,
	ValidatingMove		 = 6,
	ExecutingMove		 = 7,
	PawnPromotion		 = 8,
	WaitingForRemoteMove = 9,
	WaitingForCPUMove	 = 10,
	GameOver			 = 11,
};


/// <summary>
/// Enumerates the different types of settings available.
/// </summary>
enum class SettingsType
{
	BoardStyle,
	ChessPieceStyle,
	PlayerName,
	AudioSFXEnabled,
	AudioSFXVolume,
	AudioAtmosEnabled,
	AudioAtmosVolume,
	AudioAtmosScenario,
	AudioMasterVolume,
	DiscoveryUDPPort,
};


/// <summary>
/// Enumerates the available game mode selections.
/// </summary>
enum class GameModeSelection
{
	None		= 0,
	LocalCoop	= 1,
	VsCPU		= 2,
	Multiplayer = 3
};


/// <summary>
/// Represents the configuration settings for a game session.
/// </summary>
struct GameConfiguration
{
	GameModeSelection mode;
	Side			  localPlayer;
	int				  difficulty;
};


//============================================================
//			File Manager
//============================================================

constexpr auto								LoggingFolder					   = "Logs";
constexpr auto								LogFile							   = "Chess.log";

constexpr auto								UserSettingsFile				   = "Config.json";
constexpr auto								SettingsFolder					   = "Settings";


//============================================================
//			Config File Settings
//============================================================

constexpr auto								BoardStyleSetting				   = "BoardStyle";
constexpr auto								PieceStyleSetting				   = "PieceStyle";
constexpr auto								SelectedAdapter					   = "Selected_Adapter";
constexpr auto								PlayerNameSetting				   = "PlayerName";
constexpr auto								AudioSFXVolumeSetting			   = "Audio_SFX_Volume";
constexpr auto								AudioSFXEnabledSetting			   = "Audio_SFX_Enabled";
constexpr auto								AudioAtmosVolumeSetting			   = "Audio_Atmos_Volume";
constexpr auto								AudioAtmosEnabledSetting		   = "Audio_Atmos_Enabled";
constexpr auto								AudioAtmosScenarioSetting		   = "Audio_Atmos_Scenario";
constexpr auto								AudioMasterVolumeSetting		   = "Audio_Master_Volume";
constexpr auto								DiscoveryUDPPortSetting			   = "Discovery_UDP_Port";


//============================================================
//			Multiplayer - Network Communication
//============================================================

constexpr int								PackageBufferSize				   = 65536;
constexpr const char					   *RemoteComSecret					   = "316";

constexpr auto								ConnectionStateKey				   = "ConnectionState";
constexpr auto								MoveKey							   = "Move";
constexpr auto								ChatMessageKey					   = "Chat";
constexpr auto								InvitationMessageKey			   = "Invitation";
constexpr auto								InvitationResponseMessageKey	   = "InvResponse";
constexpr auto								PlayerChosenKey					   = "PlayerChosen";
constexpr auto								PlayerReadyFlagKey				   = "PlayerReady";


//============================================================
//			JSON Conversion
//============================================================

constexpr auto								jNetworkAdapterID				   = "ID";
constexpr auto								jNetworkAdapterDesc				   = "adapterName";
constexpr auto								jNetworkAdapterIP				   = "IPv4";
constexpr auto								jNetworkAdapterISubnet			   = "subnet";

constexpr auto								jPositionX						   = "x";
constexpr auto								jPositionY						   = "y";

constexpr auto								jMoveStart						   = "start";
constexpr auto								jMoveEnd						   = "end";
constexpr auto								jMoveType						   = "type";
constexpr auto								jMovePromotion					   = "promotion";

constexpr auto								jDiscoveryIP					   = "IPAddress";
constexpr auto								jDiscoveryPort					   = "tcpPort";
constexpr auto								jDiscoveryName					   = "player";

constexpr auto								jInvitationPlayerName			   = "PlayerName";
constexpr auto								jInvitationVersion				   = "Version";
constexpr auto								jInvitationAccepted				   = "Accepted";
constexpr auto								jInvitationReason				   = "Reason";

constexpr auto								jConnectEventType				   = "Type";
constexpr auto								jConnectEventError				   = "Error";
constexpr auto								jConnectEventEndpoint			   = "Endpoint";
