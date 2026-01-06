/*
  ==============================================================================
	Module:         ChessLogicAPIDefines
	Description:    Definitions for the Chess Logic API
  ==============================================================================
*/

#pragma once

#include <Windows.h>


typedef void(CALLBACK *PFN_CALLBACK)(int messageId, void *pContext);


#define MAX_STRING_LENGTH 250

/// <summary>
/// Defines a plain C structure representing a position with x and y coordinates.
/// </summary>
typedef struct PositionInstance
{
	int x;
	int y;
} PositionInstance;


/// <summary>
/// Defines the possible types of moves in a chess game as an enumeration.
/// </summary>
typedef enum MoveTypeInstance
{
	MoveType_None			   = 0,
	MoveType_Normal			   = 1 << 0, // 1
	MoveType_DoublePawnPush	   = 1 << 1, // 2
	MoveType_PawnPromotion	   = 1 << 2, // 4
	MoveType_Capture		   = 1 << 3, // 8
	MoveType_EnPassant		   = 1 << 4, // 16
	MoveType_CastlingKingside  = 1 << 5, // 32
	MoveType_CastlingQueenside = 1 << 6, // 64
	MoveType_Check			   = 1 << 7, // 128
	MoveType_Checkmate		   = 1 << 8, // 256
} MoveTypeInstance;


/// <summary>
/// Defines the different types of chess pieces.
/// </summary>
typedef enum PieceTypeInstance
{
	DefaultType,
	PawnType,
	KnightType,
	BishopType,
	RookType,
	QueenType,
	KingType
} PieceTypeInstance;


/// <summary>
/// Represents a possible move in a game, including start and end positions, move type, and optional promotion piece.
/// </summary>
typedef struct PossibleMoveInstance
{
	PositionInstance  start;
	PositionInstance  end;
	MoveTypeInstance  type;
	PieceTypeInstance promotionPiece;
} PossibleMoveInstance;


/// <summary>
/// Represents a plain C instance of a network adapter with associated properties.
/// </summary>
typedef struct NetworkAdapterInstance
{
	char		 adapterName[MAX_STRING_LENGTH];
	char		 networkName[MAX_STRING_LENGTH];
	unsigned int ID;
	int			 visibility;
	int			 type;

} NetworkAdapterInstance;


/// <summary>
/// Represents a plain C connection event, including its state, the remote name, and an error message.
/// </summary>
typedef struct CConnectionEvent
{
	int	 state;
	char remoteName[MAX_STRING_LENGTH];
	char errorMessage[MAX_STRING_LENGTH];
} CConnectionEvent;
