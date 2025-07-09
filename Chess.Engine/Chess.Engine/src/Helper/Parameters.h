/*
  ==============================================================================
	Module:         Parameters
	Description:    Parameters and constants definitions
  ==============================================================================
*/

#pragma once


//============================================================
//			Chess Piece Score Values
//============================================================

constexpr int pawnValue			= 1;
constexpr int kingValue			= 0; // Technically does not have a value, since the game revolves around protecting the king
constexpr int knightValue		= 3;
constexpr int rookValue			= 5;
constexpr int bishopValue		= 3;
constexpr int queenValue		= 9;

constexpr int BOARD_SIZE		= 8;
constexpr int PLAYER_PIECES_NUM = 2 * BOARD_SIZE;


enum class PieceType
{
	DefaultType,
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};


enum class PlayerColor
{
	NoColor,
	White,
	Black
};


enum class EndGameState
{
	OnGoing	  = 1,
	Checkmate = 2,
	StaleMate = 3,
	Reset	  = 4
};


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
	GameOver			 = 10
};


enum class SettingsType
{
	boardTheme,
	piecesTheme,
	playerName
};


//============================================================
//			File Manager
//============================================================

constexpr auto		  LoggingFolder				   = "Logs";
constexpr auto		  LogFile					   = "Chess.log";

constexpr auto		  UserSettingsFile			   = "Config.json";
constexpr auto		  SettingsFolder			   = "Settings";


//============================================================
//			Config File Settings
//============================================================

constexpr auto		  BoardTheme				   = "BoardTheme";
constexpr auto		  PieceTheme				   = "PieceTheme";
constexpr auto		  SelectedAdapter			   = "Selected_Adapter";
constexpr auto		  PlayerNameSetting			   = "PlayerName";


//============================================================
//			Multiplayer - Network Communication
//============================================================

constexpr int		  PackageBufferSize			   = 65536;
constexpr const char *RemoteComSecret			   = "316";

constexpr auto		  ConnectionStateKey		   = "ConnectionState";
constexpr auto		  MoveKey					   = "Move";
constexpr auto		  ChatMessageKey			   = "Chat";
constexpr auto		  InvitationMessageKey		   = "Invitation";
constexpr auto		  InvitationResponseMessageKey = "InvResponse";
constexpr auto		  PlayerChosenKey			   = "PlayerChosen";
constexpr auto		  PlayerReadyFlagKey		   = "PlayerReady";


//============================================================
//			JSON Conversion
//============================================================

constexpr auto		  jNetworkAdapterID			   = "ID";
constexpr auto		  jNetworkAdapterDesc		   = "description";
constexpr auto		  jNetworkAdapterIP			   = "IPv4";
constexpr auto		  jNetworkAdapterISubnet	   = "subnet";
constexpr auto		  jNetworkAdapterIEligible	   = "eligible";
constexpr auto		  jNetworkAdapterISelected	   = "selected";

constexpr auto		  jPositionX				   = "x";
constexpr auto		  jPositionY				   = "y";

constexpr auto		  jMoveStart				   = "start";
constexpr auto		  jMoveEnd					   = "end";
constexpr auto		  jMoveType					   = "type";
constexpr auto		  jMovePromotion			   = "promotion";

constexpr auto		  jDiscoveryIP				   = "IPAddress";
constexpr auto		  jDiscoveryPort			   = "tcpPort";
constexpr auto		  jDiscoveryName			   = "player";

constexpr auto		  jInvitationPlayerName		   = "PlayerName";
constexpr auto		  jInvitationVersion		   = "Version";
constexpr auto		  jInvitationAccepted		   = "Accepted";
constexpr auto		  jInvitationReason			   = "Reason";

constexpr auto		  jConnectEventType			   = "Type";
constexpr auto		  jConnectEventError		   = "Error";
constexpr auto		  jConnectEventEndpoint		   = "Endpoint";
