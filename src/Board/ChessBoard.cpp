/*
  ==============================================================================

	Class:          ChessBoard

	Description:    Class charecterizing a virtual chess board

  ==============================================================================
*/


#include "ChessBoard.h"
#include "ChessPiece.h"



ChessBoard::ChessBoard()
{
	squares.resize(BOARD_SIZE);
	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			squares[y].emplace_back(x, y);
		}
	}
}


ChessBoard::~ChessBoard()
{
}


Square &ChessBoard::getSquare(Position pos)
{
	return squares[pos.y][pos.x];
}


void ChessBoard::setPiece(Position pos, std::shared_ptr<ChessPiece> piece)
{
	squares[pos.y][pos.x].piece = piece;
}


std::vector<PlayerPiece> ChessBoard::getPiecesFromPlayer(PlayerColor playerColor)
{
	std::vector<PlayerPiece> playerPieces;
	playerPieces.reserve(PLAYER_PIECES_NUM * sizeof(PlayerPiece));

	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			Position pos{x, y};
			auto	&piece = getPiece(pos);
			if (piece && piece->getColor() == playerColor)
			{
				playerPieces.emplace_back(pos, piece);
			}
		}
	}

	return playerPieces;
}


std::shared_ptr<ChessPiece> &ChessBoard::getPiece(Position pos)
{
	return squares[pos.y][pos.x].piece;
}


void ChessBoard::removePiece(Position pos)
{
	squares[pos.y][pos.x].piece = nullptr;
}


bool ChessBoard::movePiece(Position start, Position end)
{
	auto &piece = getPiece(start);
	if (!piece)
		return false;

	removePiece(start);
	setPiece(end, piece);
	piece->setHasMoved(true);
	return true;
}


bool ChessBoard::isEmpty(Position pos) const
{
	return squares[pos.y][pos.x].piece == nullptr;
}


void ChessBoard::updateKingsPosition(Position &pos, PlayerColor player)
{
	player == PlayerColor::White ? mWhiteKingPosition = pos : mBlackKingPosition = pos;
}


Position ChessBoard::getKingsPosition(PlayerColor player) const
{
	return player == PlayerColor::White ? mWhiteKingPosition : mBlackKingPosition;
}


void ChessBoard::initializeBoard()
{
	// Place pieces for White
	setPiece(Position(0, 0), std::make_shared<Rook>(PlayerColor::White));
	setPiece(Position(1, 0), std::make_shared<Knight>(PlayerColor::White));
	setPiece(Position(2, 0), std::make_shared<Bishop>(PlayerColor::White));
	setPiece(Position(3, 0), std::make_shared<Queen>(PlayerColor::White));
	setPiece(Position(4, 0), std::make_shared<King>(PlayerColor::White));
	setPiece(Position(5, 0), std::make_shared<Bishop>(PlayerColor::White));
	setPiece(Position(6, 0), std::make_shared<Knight>(PlayerColor::White));
	setPiece(Position(7, 0), std::make_shared<Rook>(PlayerColor::White));
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		setPiece(Position(x, 1), std::make_shared<Pawn>(PlayerColor::White));
	}

	// Place pieces for Black
	setPiece(Position(0, 7), std::make_shared<Rook>(PlayerColor::Black));
	setPiece(Position(1, 7), std::make_shared<Knight>(PlayerColor::Black));
	setPiece(Position(2, 7), std::make_shared<Bishop>(PlayerColor::Black));
	setPiece(Position(3, 7), std::make_shared<Queen>(PlayerColor::Black));
	setPiece(Position(4, 7), std::make_shared<King>(PlayerColor::Black));
	setPiece(Position(5, 7), std::make_shared<Bishop>(PlayerColor::Black));
	setPiece(Position(6, 7), std::make_shared<Knight>(PlayerColor::Black));
	setPiece(Position(7, 7), std::make_shared<Rook>(PlayerColor::Black));
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		setPiece(Position(x, 6), std::make_shared<Pawn>(PlayerColor::Black));
	}
}
