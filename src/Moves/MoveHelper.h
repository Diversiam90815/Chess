/*
  ==============================================================================

	Class:          MoveHelper

	Description:    Helper class for generating general moves in chess

  ==============================================================================
*/

#pragma once

#include <vector>
#include <utility>
#include <array>

#include "Move.h"
#include "ChessBoard.h"
#include "Player.h"


enum MoveType
{
	Diagonal = 1,  // Bishop, Queen
	LinearForward, // Pawns moving
	Adjacent,	   // King moving one step in any direction
	File,		   // Rook, Queen moving along vertical lines (files)
	LShaped		   // Knight's jump
};


class MoveHelper
{
public:
	MoveHelper();

	~MoveHelper();


	bool			  checkAvailableMoves(PieceType piece);

	std::vector<Move> getAvailableMoves();


private:
	bool checkPawnForwardMovement(const Position &position, ChessBoard &board, const Player &player, bool hasMoved);

	bool checkPawnCaptureMovement(const Position &position, ChessBoard &board, const Player &player);

	bool checkDiagonalMoves(const Position &position, ChessBoard &board, const Player &player);

	bool checkAdjacentMoves(const Position &position, ChessBoard &board, const Player &player);

	bool checkLinearForwardMove(const Position &position, ChessBoard &board, const Player &player);

	bool checkPawnCaptureMoves(const Position &position, ChessBoard &board, const Player &player);

	bool checkLShapedMoves(const Position &position, ChessBoard &board, const Player &player);

	bool checkFileMoves(const Position &position, ChessBoard &board, const Player &player);


	template <std::size_t N>
	bool checkMovesInDirection(const Position &position, ChessBoard &board, const Player &player, const std::array<std::pair<int, int>, N> &directions, bool oneStep);



	// Returns true if the X,Y are within the board
	bool checkForBorders(const int x, const int y);

	// Returns true if the position already exists (might happen if we stack movement algorithms)
	bool checkIfPositionAlreadyExists(const int x, const int y);

	void addToAvailableMoves(Move &move);


	std::array<std::pair<int, int>, 2> mPawnMoveDirections		  = {{{0, 1}, {0, 2}}};

	std::array<std::pair<int, int>, 2> mPawnCaptureDirections	  = {{{1, 1}, {-1, 1}}};


	std::array<std::pair<int, int>, 8> mAdjacentPositions		  = {{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}}};

	std::array<std::pair<int, int>, 4> mDiagonalDirections		  = {{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

	std::array<std::pair<int, int>, 8> mLShapedDirections		  = {{{1, 2}, {2, 1}, {-1, 2}, {-2, 1}, {1, -2}, {2, -1}, {-1, -2}, {-2, -1}}};

	std::array<std::pair<int, int>, 4> mFileDirections			  = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};


	std::vector<Move>				   mPossibleMovesAndCaptures;
};
