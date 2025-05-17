/*
  ==============================================================================
	Module:         ChessLogicAPIDefines
	Description:    Definitions for the Chess Logic API
  ==============================================================================
*/

#pragma once
#include "Windows.h"

#include "Parameters.h"


typedef void(CALLBACK *PFN_CALLBACK)(int messageId, void *pContext);


#define CHESS_API		  __declspec(dllexport)

#define MAX_STRING_LENGTH 250


typedef struct PositionInstance
{
	int x;
	int y;
} PositionInstance;


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


typedef struct PossibleMoveInstance
{
	PositionInstance  start;
	PositionInstance  end;
	MoveTypeInstance  type;
	PieceTypeInstance promotionPiece;
} PossibleMoveInstance;


typedef struct NetworkAdapterInstance
{
	char		 name[MAX_STRING_LENGTH];
	unsigned int ID;
	bool		 selectedByUser;
} NetworkAdapterInstance;