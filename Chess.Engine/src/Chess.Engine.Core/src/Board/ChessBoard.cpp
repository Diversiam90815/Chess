/*
  ==============================================================================
	Module:         ChessBoard
	Description:    Class characterizing a virtual chess board
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

ChessBoard::ChessBoard(const ChessBoard &other)
{
	squares.resize(BOARD_SIZE);
	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		squares[y].resize(BOARD_SIZE);
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			// Copy over the Square data
			squares[y][x] = other.squares[y][x];
		}
	}

	// Also copy king positions
	mWhiteKingPosition = other.mWhiteKingPosition;
	mBlackKingPosition = other.mBlackKingPosition;
}


ChessBoard::~ChessBoard() {}


Square &ChessBoard::getSquare(Position pos)
{
	return squares[pos.y][pos.x];
}


void ChessBoard::setPiece(Position pos, std::shared_ptr<ChessPiece> piece)
{
	squares[pos.y][pos.x].piece = piece;
}


std::shared_ptr<ChessPiece> &ChessBoard::getPiece(Position pos)
{
	static std::shared_ptr<ChessPiece> nullPiece = nullptr;

	if (!pos.isValid())
		return nullPiece;

	return squares[pos.y][pos.x].piece;
}


std::vector<PlayerPiece> ChessBoard::getPiecesFromPlayer(PlayerColor playerColor)
{
	std::vector<PlayerPiece> playerPieces;
	playerPieces.reserve(PLAYER_PIECES_NUM);

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


void ChessBoard::removePiece(Position pos)
{
	if (CHESSBOARD_DEBUG)
		LOG_DEBUG("removePiece called at {}", LoggingHelper::positionToString(pos).c_str());

	if (!pos.isValid())
		return;

	squares[pos.y][pos.x].piece = nullptr;
}


bool ChessBoard::movePiece(Position start, Position end)
{
	if (CHESSBOARD_DEBUG)
		LOG_DEBUG("Moved piece from {} to {}", LoggingHelper::positionToString(start).c_str(), LoggingHelper::positionToString(end).c_str());

	if (!start.isValid() || !end.isValid())
		return false;

	auto &piece = getPiece(start);
	if (!piece)
		return false;

	setPiece(end, piece);
	removePiece(start);
	return true;
}


bool ChessBoard::isEmpty(Position pos) const
{
	if (!pos.isValid())
		return true;

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


void ChessBoard::removeAllPiecesFromBoard()
{
	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			squares[y][x].piece = nullptr;
		}
	}

	mWhiteKingPosition = Position{-1, -1}; // invalid position
	mBlackKingPosition = Position{-1, -1}; // invalid position
}


bool ChessBoard::getBoardState(BoardStateArray boardState)
{
	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			Position pos	  = {x, y};
			auto	&piece	  = getPiece(pos);

			int		 colorVal = 0; // 0 = no color
			int		 typeVal  = 0; // 0 = PieceType::DefaultType

			if (piece)
			{
				colorVal = static_cast<int>(piece->getColor()) & 0xF;
				typeVal	 = static_cast<int>(piece->getType())  & 0xF;
			}

			// Pack color in high nibble, type in low nibble:
			// bits [4..7] = color, bits [0..3] = piece type
			int encoded		 = (colorVal << 4) | (typeVal & 0xF);

			boardState[y][x] = encoded;
		}
	}
	return true;
}


void ChessBoard::initializeBoard()
{
	if (!mInitialized)
		removeAllPiecesFromBoard();

	// Place pieces for White
	setPiece(Position(0, 0), std::make_shared<Rook>(PlayerColor::Black));
	setPiece(Position(1, 0), std::make_shared<Knight>(PlayerColor::Black));
	setPiece(Position(2, 0), std::make_shared<Bishop>(PlayerColor::Black));
	setPiece(Position(3, 0), std::make_shared<Queen>(PlayerColor::Black));
	setPiece(Position(4, 0), std::make_shared<King>(PlayerColor::Black));
	setPiece(Position(5, 0), std::make_shared<Bishop>(PlayerColor::Black));
	setPiece(Position(6, 0), std::make_shared<Knight>(PlayerColor::Black));
	setPiece(Position(7, 0), std::make_shared<Rook>(PlayerColor::Black));
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		setPiece(Position(x, 1), std::make_shared<Pawn>(PlayerColor::Black));
	}

	// Place pieces for Black
	setPiece(Position(0, 7), std::make_shared<Rook>(PlayerColor::White));
	setPiece(Position(1, 7), std::make_shared<Knight>(PlayerColor::White));
	setPiece(Position(2, 7), std::make_shared<Bishop>(PlayerColor::White));
	setPiece(Position(3, 7), std::make_shared<Queen>(PlayerColor::White));
	setPiece(Position(4, 7), std::make_shared<King>(PlayerColor::White));
	setPiece(Position(5, 7), std::make_shared<Bishop>(PlayerColor::White));
	setPiece(Position(6, 7), std::make_shared<Knight>(PlayerColor::White));
	setPiece(Position(7, 7), std::make_shared<Rook>(PlayerColor::White));
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		setPiece(Position(x, 6), std::make_shared<Pawn>(PlayerColor::White));
	}

	// Update King's Position
	mWhiteKingPosition = Position(4, 7);
	mBlackKingPosition = Position(4, 0);

	mInitialized	   = true;
}
