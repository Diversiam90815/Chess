/*
  ==============================================================================
	Module:         Parameters
	Description:    Parameters and constants definitions
  ==============================================================================
*/

#pragma once

#include <array>


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
	WaitingForCPUMove	 = 10,
	GameOver			 = 11,
};


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


enum class GameModeSelection
{
	None	  = 0,
	LocalCoop = 1,
	VsCPU	  = 2,
};


struct GameConfiguration
{
	GameModeSelection mode;
	PlayerColor		  localPlayer;
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


//============================================================
//			Move / Positional Evaluation
//============================================================

// Game phase determination thresholds
constexpr int								OPENING_MATERIAL_THRESHOLD		   = 6000; // Total material threshold for opening phase
constexpr int								OPENING_PIECE_THRESHOLD			   = 20;   // Piece count threshold for opening phase
constexpr int								MIDDLEGAME_MATERIAL_THRESHOLD	   = 2500; // Total material threshold for middlegame phase
constexpr int								MIDDLEGAME_PIECE_THRESHOLD		   = 12;   // Piece count threshold for middlegame phase

// Standard piece values
constexpr int								PAWN_VALUE						   = 100; // Base value of a pawn
constexpr int								KNIGHT_VALUE					   = 320; // Base value of a knight (~3.2 pawns)
constexpr int								BISHOP_VALUE					   = 330; // Base value of a bishop (~3.3 pawns)
constexpr int								ROOK_VALUE						   = 500; // Base value of a rook (~5 pawns)
constexpr int								QUEEN_VALUE						   = 900; // Base value of a queen (~9 pawns)

// Move type bonuses
constexpr int								CHECKMATE_BONUS					   = 10000; // Massive bonus for checkmate moves
constexpr int								CHECK_BONUS						   = 50;	// Bonus for moves that give check
constexpr int								CAPTURE_BONUS					   = 25;	// Base bonus for any capture move
constexpr int								CASTLE_BONUS					   = 60;	// Bonus for castling moves (king safety)
constexpr int								PROMOTION_BONUS					   = 800;	// Base bonus for pawn promotion moves
constexpr int								CENTER_CONTROL_BONUS			   = 15;	// Bonus for controlling/occupying center squares

// King safety evaluation factors
constexpr int								EXPOSE_KING_FACTOR				   = 100; // Penalty for moves that expose our king to attack
constexpr int								ATTACKER_KING_FACTOR			   = 20;  // Penalty per opponent piece attacking our king
constexpr int								ATTACK_NEAR_KING_FACTOR			   = 30;  // Bonus for attacking squares near opponent's king
constexpr int								DEFENDING_KING_FACTOR			   = 25;  // Bonus for defending squares near our king
constexpr int								KING_CENTRALIZATION_FACTOR		   = 25;  // Bonus for king centralization in endgame
constexpr int								KING_ACTIVITY_FACTOR			   = 10;  // Bonus for any king activity in endgame

// Pawn structure evaluation factors
constexpr int								PASSED_PAWN_FACTOR				   = 50; // Bonus for passed pawns (no enemy pawns can stop them)
constexpr int								ISOLATED_PAWN_FACTOR			   = 20; // Penalty for isolated pawns (no friendly pawns on adjacent files)
constexpr int								DOUBLE_PAWN_FACTOR				   = 15; // Penalty for doubled pawns (multiple pawns on same file)

// Defensive pattern evaluation
constexpr int								BLOCK_ATTACK_FACTOR				   = 10; // Bonus for moves that block opponent attacks

// Evaluation component weights (multipliers for different aspects)
constexpr int								KING_SAFETY_WEIGHT				   = 3; // Weight multiplier for king safety evaluation
constexpr int								MOBILITY_WEIGHT					   = 2; // Weight multiplier for piece mobility evaluation
constexpr int								THREAT_WEIGHT					   = 4; // Weight multiplier for threat level evaluation

// Tactical pattern bonuses
constexpr int								FORK_BONUS						   = 150; // Bonus for moves that create forks (attacking 2+ pieces)
constexpr int								PIN_BONUS						   = 100; // Bonus for moves that create pins (immobilize opponent pieces)
constexpr int								SKEWER_BONUS					   = 120; // Bonus for moves that create skewers (force valuable piece to move)
constexpr int								THREAT_BLOCK_BONUS				   = 60;  // Bonus for moves that block opponent threats

// Positional evaluation
constexpr int								CHECK_FACTOR					   = 50; // Check penalty/bonus in non-endgame phases
constexpr int								PASSED_ADVANCED_FACTOR			   = 10; // Additional bonus per rank for advanced passed pawns
constexpr int								CENTRAL_PAWN_FACTOR				   = 10; // Bonus for pawns controlling center squares
constexpr int								SUPPORTED_PAWN_FACTOR			   = 10; // Bonus for pawns defended by other pawns
constexpr int								PAWN_MAJORITY_FACTOR			   = 25; // Bonus for having more pawns on kingside/queenside
constexpr int								PAWN_CHAIN_FACTOR				   = 15; // Bonus for connected pawn chains (3+ connected pawns)

// Early Game evaluation
constexpr int								OPENING_DEVELOPING_MOVE_FACTOR	   = 50; // Bonus for developing pieces in early game
constexpr int								OPENING_PAWN_CENTER_CONTROL_FACTOR = 30; // Bonus for pushing pawns to the center control in early game
constexpr int								OPENING_EARLY_QUEEN_MOVES_FACTOR   = 25; // Factor of moving the queen too early in the early game
constexpr int								OPENING_CASTLING_FACTOR			   = 40; // Bonus for castling in early game


// Position value tables (from white's perspective, flip for black)
constexpr std::array<std::array<int, 8>, 8> PAWN_TABLE_MG					   = {{{0, 0, 0, 0, 0, 0, 0, 0},
																				   {50, 50, 50, 50, 50, 50, 50, 50},
																				   {10, 10, 20, 30, 30, 20, 10, 10},
																				   {5, 5, 10, 27, 27, 10, 5, 5},
																				   {0, 0, 0, 25, 25, 0, 0, 0},
																				   {5, -5, -10, 0, 0, -10, -5, 5},
																				   {5, 10, 10, -25, -25, 10, 10, 5},
																				   {0, 0, 0, 0, 0, 0, 0, 0}}};

// Endgame pawn table (higher values as pawns become more important)
constexpr std::array<std::array<int, 8>, 8> PAWN_TABLE_EG					   = {{{0, 0, 0, 0, 0, 0, 0, 0},
																				   {80, 80, 80, 80, 80, 80, 80, 80},
																				   {50, 50, 50, 50, 50, 50, 50, 50},
																				   {30, 30, 30, 30, 30, 30, 30, 30},
																				   {20, 20, 20, 20, 20, 20, 20, 20},
																				   {10, 10, 10, 10, 10, 10, 10, 10},
																				   {10, 10, 10, 10, 10, 10, 10, 10},
																				   {0, 0, 0, 0, 0, 0, 0, 0}}};

// Knight middlegame table (prefers center, avoids edges)
constexpr std::array<std::array<int, 8>, 8> KNIGHT_TABLE_MG					   = {{{-50, -40, -30, -30, -30, -30, -40, -50},
																				   {-40, -20, 0, 0, 0, 0, -20, -40},
																				   {-30, 0, 10, 15, 15, 10, 0, -30},
																				   {-30, 5, 15, 20, 20, 15, 5, -30},
																				   {-30, 0, 15, 20, 20, 15, 0, -30},
																				   {-30, 5, 10, 15, 15, 10, 5, -30},
																				   {-40, -20, 0, 5, 5, 0, -20, -40},
																				   {-50, -40, -30, -30, -30, -30, -40, -50}}};

// Knight endgame table (similar to middlegame as knights remain center-oriented)
constexpr std::array<std::array<int, 8>, 8> KNIGHT_TABLE_EG					   = {{{-50, -40, -30, -30, -30, -30, -40, -50},
																				   {-40, -20, 0, 5, 5, 0, -20, -40},
																				   {-30, 0, 10, 15, 15, 10, 0, -30},
																				   {-30, 5, 15, 20, 20, 15, 5, -30},
																				   {-30, 0, 15, 20, 20, 15, 0, -30},
																				   {-30, 5, 10, 15, 15, 10, 5, -30},
																				   {-40, -20, 0, 5, 5, 0, -20, -40},
																				   {-50, -40, -30, -30, -30, -30, -40, -50}}};

// Bishop table (prefers long diagonals, avoids edges)
constexpr std::array<std::array<int, 8>, 8> BISHOP_TABLE					   = {{{-20, -10, -10, -10, -10, -10, -10, -20},
																				   {-10, 0, 0, 0, 0, 0, 0, -10},
																				   {-10, 0, 5, 10, 10, 5, 0, -10},
																				   {-10, 5, 5, 10, 10, 5, 5, -10},
																				   {-10, 0, 10, 10, 10, 10, 0, -10},
																				   {-10, 10, 10, 10, 10, 10, 10, -10},
																				   {-10, 5, 0, 0, 0, 0, 5, -10},
																				   {-20, -10, -10, -10, -10, -10, -10, -20}}};

// Rook table (prefers 7th rank and open files)
constexpr std::array<std::array<int, 8>, 8> ROOK_TABLE						   = {{{0, 0, 0, 0, 0, 0, 0, 0},
																				   {5, 10, 10, 10, 10, 10, 10, 5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {0, 0, 0, 5, 5, 0, 0, 0}}};

// Queen table (prefers center, avoids early development)
constexpr std::array<std::array<int, 8>, 8> QUEEN_TABLE						   = {{{-20, -10, -10, -5, -5, -10, -10, -20},
																				   {-10, 0, 0, 0, 0, 0, 0, -10},
																				   {-10, 0, 5, 5, 5, 5, 0, -10},
																				   {-5, 0, 5, 5, 5, 5, 0, -5},
																				   {0, 0, 5, 5, 5, 5, 0, -5},
																				   {-10, 5, 5, 5, 5, 5, 0, -10},
																				   {-10, 0, 5, 0, 0, 0, 0, -10},
																				   {-20, -10, -10, -5, -5, -10, -10, -20}}};

// King middlegame table (strongly prefers back rank safety)
constexpr std::array<std::array<int, 8>, 8> KING_TABLE_MG					   = {{{-30, -40, -40, -50, -50, -40, -40, -30},
																				   {-30, -40, -40, -50, -50, -40, -40, -30},
																				   {-30, -40, -40, -50, -50, -40, -40, -30},
																				   {-30, -40, -40, -50, -50, -40, -40, -30},
																				   {-20, -30, -30, -40, -40, -30, -30, -20},
																				   {-10, -20, -20, -20, -20, -20, -20, -10},
																				   {20, 20, 0, 0, 0, 0, 20, 20},
																				   {20, 30, 10, 0, 0, 10, 30, 20}}};

// King endgame table (encourages centralization and activity)
constexpr std::array<std::array<int, 8>, 8> KING_TABLE_EG					   = {{{-50, -40, -30, -20, -20, -30, -40, -50},
																				   {-30, -20, -10, 0, 0, -10, -20, -30},
																				   {-30, -10, 20, 30, 30, 20, -10, -30},
																				   {-30, -10, 30, 40, 40, 30, -10, -30},
																				   {-30, -10, 30, 40, 40, 30, -10, -30},
																				   {-30, -10, 20, 30, 30, 20, -10, -30},
																				   {-30, -30, 0, 0, 0, 0, -30, -30},
																				   {-50, -30, -30, -30, -30, -30, -30, -50}}};
