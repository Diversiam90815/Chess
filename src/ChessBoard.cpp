
#include "ChessBoard.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"
#include <iostream>


ChessBoard::ChessBoard()
{
	// Initialize all board positions to nullptr
	for (auto &row : board)
	{
		row.fill(nullptr);
	}
	initializeBoard();
}


ChessBoard::~ChessBoard()
{
	// Clean up dynamically allocated pieces
	for (auto &row : board)
	{
		for (auto &piece : row)
		{
			piece.reset();
			piece = nullptr;
		}
	}
}


std::shared_ptr<ChessPiece> ChessBoard::getPiece(int x, int y) const
{
	if (x < 0 || x > 7 || y < 0 || y > 7)
		return nullptr;

	return board[y][x];
}


void ChessBoard::setPiece(int x, int y, std::shared_ptr<ChessPiece> piece)
{
	if (x < 0 || x > 7 || y < 0 || y > 7)
		return;

	board[y][x] = piece;
}


bool ChessBoard::isEmpty(int x, int y) const
{
	return getPiece(x, y) == nullptr;
}


bool ChessBoard::movePiece(int fromX, int fromY, int toX, int toY)
{
	std::shared_ptr<ChessPiece> piece = getPiece(fromX, fromY);
	if (piece != nullptr && piece->isValidMove(fromX, fromY, toX, toY, *this))
	{
		std::shared_ptr<ChessPiece> targetPiece = getPiece(toX, toY);
		if (targetPiece != nullptr)
		{
			targetPiece.reset(); // Capture the piece
		}
		setPiece(toX, toY, piece);
		setPiece(fromX, fromY, nullptr);
		piece->setHasMoved(true);
		return true;
	}
	return false;
}


bool ChessBoard::isValidMove(int fromX, int fromY, int toX, int toY) const
{
	std::shared_ptr<ChessPiece> piece = getPiece(fromX, fromY);
	if (piece != nullptr)
	{
		return piece->isValidMove(fromX, fromY, toX, toY, *this);
	}
	return false;
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
	for (int x = 0; x < 8; ++x)
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
	for (int x = 0; x < 8; ++x)
	{
		setPiece(x, 6, std::make_shared<Pawn>(PieceColor::Black));
	}
}
