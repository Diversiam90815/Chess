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


void ChessBoard::setPiece(int x, int y, std::shared_ptr<ChessPiece> piece)
{
	squares[y][x].piece = piece;
}


std::vector<std::pair<Position, std::shared_ptr<ChessPiece>>> ChessBoard::getPiecesFromPlayer(PieceColor playerColor)
{
	std::vector<std::pair<Position, std::shared_ptr<ChessPiece>>> playerPieces;

	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			Position pos{x, y};
			auto	 piece = getPiece(pos);
			if (piece && piece->getColor() == playerColor)
			{
				playerPieces.emplace_back(pos, piece);
			}
		}
	}

	return playerPieces;
}


std::shared_ptr<ChessPiece> ChessBoard::getPiece(Position pos)
{
	return squares[pos.y][pos.x].piece;
}


void ChessBoard::removePiece(Position pos)
{
	squares[pos.y][pos.x].piece = nullptr;
}


bool ChessBoard::movePiece(Position start, Position end)
{
	auto piece = getPiece(start);
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


const Move *ChessBoard::getLastMove()
{
	if (moveHistory.empty())
		return nullptr;
	return &moveHistory.back();
}


void ChessBoard::addMoveToHistory(const Move &move)
{
	moveHistory.push_back(move);
}


void ChessBoard::updateKingsPosition(Position &pos, PieceColor player)
{
	player == PieceColor::White ? mWhiteKingPosition = pos : mBlackKingPosition = pos;
}


Position ChessBoard::getKingsPosition(PieceColor player) const
{
	return player == PieceColor::White ? mWhiteKingPosition : mBlackKingPosition;
}


void ChessBoard::initializeBoard()
{
	// Place pieces for White
	setPiece(0, 0, std::make_shared<Rook>(PieceColor::White));
	setPiece(1, 0, std::make_shared<Knight>(PieceColor::White));
	setPiece(2, 0, std::make_shared<Bishop>(PieceColor::White));
	setPiece(3, 0, std::make_shared<Queen>(PieceColor::White));
	setPiece(4, 0, std::make_shared<King>(PieceColor::White));
	setPiece(5, 0, std::make_shared<Bishop>(PieceColor::White));
	setPiece(6, 0, std::make_shared<Knight>(PieceColor::White));
	setPiece(7, 0, std::make_shared<Rook>(PieceColor::White));
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		setPiece(x, 1, std::make_shared<Pawn>(PieceColor::White));
	}

	// Place pieces for Black
	setPiece(0, 7, std::make_shared<Rook>(PieceColor::Black));
	setPiece(1, 7, std::make_shared<Knight>(PieceColor::Black));
	setPiece(2, 7, std::make_shared<Bishop>(PieceColor::Black));
	setPiece(3, 7, std::make_shared<Queen>(PieceColor::Black));
	setPiece(4, 7, std::make_shared<King>(PieceColor::Black));
	setPiece(5, 7, std::make_shared<Bishop>(PieceColor::Black));
	setPiece(6, 7, std::make_shared<Knight>(PieceColor::Black));
	setPiece(7, 7, std::make_shared<Rook>(PieceColor::Black));
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		setPiece(x, 6, std::make_shared<Pawn>(PieceColor::Black));
	}
}
