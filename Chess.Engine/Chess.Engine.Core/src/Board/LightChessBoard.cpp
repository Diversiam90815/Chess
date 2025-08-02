/*
  ==============================================================================
	Module:         LightChessBoard
	Description:    Lightweight version of the Chessboard with minimal data for fast access and manipulation
  ==============================================================================
*/

#include "LightChessBoard.h"


LightChessBoard::LightChessBoard()
{
	clear();
}


LightChessBoard::LightChessBoard(ChessBoard &board)
{
	copyFromChessBoard(board);
}


LightChessBoard::LightChessBoard(const LightChessBoard &other)
	: mBoard(other.mBoard), mCurrentPlayer(other.mCurrentPlayer), mWhiteKingPos(other.mWhiteKingPos), mBlackKingPos(other.mBlackKingPos), mEnPassantTarget(other.mEnPassantTarget),
	  mWhiteCanCastleKingside(other.mWhiteCanCastleKingside), mWhiteCanCastleQueenside(other.mWhiteCanCastleQueenside), mBlackCanCastleKingside(other.mBlackCanCastleKingside),
	  mBlackCanCastleQueenside(other.mBlackCanCastleQueenside), mHalfMoveClock(other.mHalfMoveClock), mFullMoveNumber(other.mFullMoveNumber), mHashKey(other.mHashKey),
	  mHashKeyValid(other.mHashKeyValid)
{
}


LightChessBoard &LightChessBoard::operator=(const LightChessBoard &other)
{
	if (this != &other)
	{
		mBoard					 = other.mBoard;
		mCurrentPlayer			 = other.mCurrentPlayer;
		mWhiteKingPos			 = other.mWhiteKingPos;
		mBlackKingPos			 = other.mBlackKingPos;
		mEnPassantTarget		 = other.mEnPassantTarget;
		mWhiteCanCastleKingside	 = other.mWhiteCanCastleKingside;
		mWhiteCanCastleQueenside = other.mWhiteCanCastleQueenside;
		mBlackCanCastleKingside	 = other.mBlackCanCastleKingside;
		mBlackCanCastleQueenside = other.mBlackCanCastleQueenside;
		mHalfMoveClock			 = other.mHalfMoveClock;
		mFullMoveNumber			 = other.mFullMoveNumber;
		mHashKey				 = other.mHashKey;
		mHashKeyValid			 = other.mHashKeyValid;
	}
	return *this;
}


void LightChessBoard::initializeStartingPosition()
{
	clear();
	initializePieces();
}


void LightChessBoard::copyFromChessBoard(ChessBoard &board)
{
	clear();

	// Copy pieces from the main board
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			Position pos{x, y};
			if (board.isEmpty(pos))
				continue;

			auto &piece = board.getPiece(pos);
			if (!piece)
				continue;

			LightPiece lightPiece(piece->getType(), piece->getColor(), piece->hasMoved());
			setPiece(pos, lightPiece);
		}
	}

	// Copy king positions
	mWhiteKingPos = board.getKingsPosition(PlayerColor::White);
	mBlackKingPos = board.getKingsPosition(PlayerColor::Black);

	inferCastlingRights();
}


void LightChessBoard::clear()
{
	// Clear the board
	for (auto &row : mBoard)
	{
		for (auto &square : row)
		{
			square = LightPiece();
		}
	}

	// Reset game state
	mCurrentPlayer			 = PlayerColor::White;
	mWhiteKingPos			 = {4, 7};
	mBlackKingPos			 = {4, 0};
	mEnPassantTarget		 = {-1, -1};
	mWhiteCanCastleKingside	 = true;
	mWhiteCanCastleQueenside = true;
	mBlackCanCastleKingside	 = true;
	mBlackCanCastleQueenside = true;
	mHalfMoveClock			 = 0;
	mFullMoveNumber			 = 1;
	mHashKeyValid			 = false;
}


const LightPiece &LightChessBoard::getPiece(Position pos) const
{
	return mBoard[pos.x][pos.y];
}


const LightPiece &LightChessBoard::getPiece(int x, int y) const
{
	return mBoard[x][y];
}


void LightChessBoard::setPiece(Position pos, const LightPiece &piece)
{
	mBoard[pos.x][pos.y] = piece;
	mHashKeyValid		 = false;
}


void LightChessBoard::setPiece(int x, int y, const LightPiece &piece)
{
	mBoard[x][y]  = piece;
	mHashKeyValid = false;
}


void LightChessBoard::removePiece(int x, int y)
{
	mBoard[x][y]  = LightPiece();
	mHashKeyValid = false;
}


void LightChessBoard::removePiece(Position pos)
{
	mBoard[pos.x][pos.y] = LightPiece();
	mHashKeyValid		 = false;
}


bool LightChessBoard::isEmpty(int x, int y) const
{
	return mBoard[x][y].isEmpty();
}


bool LightChessBoard::isEmpty(Position pos) const
{
	return mBoard[pos.x][pos.y].isEmpty();
}


bool LightChessBoard::isValidPosition(Position pos) const
{
	return pos.x >= 0 && pos.x < BOARD_SIZE && pos.y >= 0 && pos.y < BOARD_SIZE;
}


bool LightChessBoard::isValidPosition(int x, int y) const
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}


Position LightChessBoard::getKingPosition(PlayerColor player) const
{
	return (player == PlayerColor::White) ? mWhiteKingPos : mBlackKingPos;
}


void LightChessBoard::updateKingPosition(Position pos, PlayerColor player)
{
	if (player == PlayerColor::White)
		mWhiteKingPos = pos;
	else
		mBlackKingPos = pos;

	mHashKeyValid = false;
}


void LightChessBoard::inferCastlingRights()
{
	// Infer castling rights based on king and rook positions and move status
	auto whiteKing			 = getPiece(4, 7);
	auto blackKing			 = getPiece(4, 0);
	auto whiteKingsideRook	 = getPiece(7, 7);
	auto whiteQueensideRook	 = getPiece(0, 7);
	auto blackKingsideRook	 = getPiece(7, 0);
	auto blackQueensideRook	 = getPiece(0, 0);

	mWhiteCanCastleKingside	 = (whiteKing.type == PieceType::King && !whiteKing.hasMoved && whiteKingsideRook.type == PieceType::Rook && !whiteKingsideRook.hasMoved);
	mWhiteCanCastleQueenside = (whiteKing.type == PieceType::King && !whiteKing.hasMoved && whiteQueensideRook.type == PieceType::Rook && !whiteQueensideRook.hasMoved);
	mBlackCanCastleKingside	 = (blackKing.type == PieceType::King && !blackKing.hasMoved && blackKingsideRook.type == PieceType::Rook && !blackKingsideRook.hasMoved);
	mBlackCanCastleQueenside = (blackKing.type == PieceType::King && !blackKing.hasMoved && blackQueensideRook.type == PieceType::Rook && !blackQueensideRook.hasMoved);
}


bool LightChessBoard::canCastleKingside(PlayerColor player) const
{
	return (player == PlayerColor::White) ? mWhiteCanCastleKingside : mBlackCanCastleKingside;
}


bool LightChessBoard::canCastleQueenside(PlayerColor player) const
{
	return (player == PlayerColor::White) ? mWhiteCanCastleQueenside : mBlackCanCastleQueenside;
}


void LightChessBoard::setCastlingRights(PlayerColor player, bool kingside, bool queenside)
{
	if (player == PlayerColor::White)
	{
		mWhiteCanCastleKingside	 = kingside;
		mWhiteCanCastleQueenside = queenside;
	}
	else
	{
		mBlackCanCastleKingside	 = kingside;
		mBlackCanCastleQueenside = queenside;
	}
	mHashKeyValid = false;
}


bool LightChessBoard::isCastlingLegal(PlayerColor player, bool kingside) const
{
	// Check basic castling rights
	if (kingside && !canCastleKingside(player))
		return false;

	if (!kingside && !canCastleQueenside(player))
		return false;

	// King must not be in check
	if (isInCheck(player))
		return false;

	// Path must be clear
	if (!isCastlingPathClear(player, kingside))
		return false;

	// King must not pass through check
	if (wouldKingPassThroughCheck(player, kingside))
		return false;

	return true;
}


MoveUndo LightChessBoard::makeMove(const PossibleMove &move)
{
	// Store undo information
	MoveUndo undoInfo;
	undoInfo.move					 = move;
	undoInfo.capturedPiece			 = getPiece(move.end);
	undoInfo.enPassantTarget		 = mEnPassantTarget;
	undoInfo.whiteCanCastleKingside	 = mWhiteCanCastleKingside;
	undoInfo.whiteCanCastleQueenside = mWhiteCanCastleQueenside;
	undoInfo.blackCanCastleKingside	 = mBlackCanCastleKingside;
	undoInfo.blackCanCastleQueenside = mBlackCanCastleQueenside;
	undoInfo.halfMoveClock			 = mHalfMoveClock;
	undoInfo.fullMoveNumber			 = mFullMoveNumber;

	// Get the moving piece
	LightPiece movingPiece			 = getPiece(move.start);

	// Handle special moves
	if (((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside) || ((move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside))
	{
		executeCastling(move);
	}
	else if ((move.type & MoveType::EnPassant) == MoveType::EnPassant)
	{
		executeEnPassant(move);
	}
	else
	{
		// Normal move
		setPiece(move.end, movingPiece);
		removePiece(move.start);

		// Handle pawn promotion
		if ((move.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
		{
			LightPiece promotedPiece(move.promotionPiece, movingPiece.color, true);
			setPiece(move.end, promotedPiece);
		}
	}

	// Update king position if king moved
	if (movingPiece.type == PieceType::King)
	{
		updateKingPosition(move.end, movingPiece.color);
	}

	// Mark piece as moved
	LightPiece &piece = const_cast<LightPiece &>(getPiece(move.end));
	piece.hasMoved	  = true;

	// Update game state
	updateGameState(move);

	return undoInfo;
}


void LightChessBoard::unmakeMove(const MoveUndo &undoInfo)
{
	const PossibleMove &move		= undoInfo.move;
	LightPiece			movingPiece = getPiece(move.end);

	// Restore the moving piece to its original position
	setPiece(move.start, movingPiece);

	// Handle special moves
	if (((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside) || ((move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside))
	{
		// Undo castling - move rook back
		int		   rookStartX = (move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside ? 7 : 0;
		int		   rookEndX	  = (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside ? 5 : 3;
		int		   y		  = (movingPiece.color == PlayerColor::White) ? 7 : 0;

		LightPiece rook		  = getPiece(rookEndX, y);
		setPiece(rookStartX, y, rook);
		removePiece(rookEndX, y);
	}
	else if ((move.type & MoveType::EnPassant) == MoveType::EnPassant)
	{
		// Restore captured pawn
		int capturedY = (movingPiece.color == PlayerColor::White) ? move.end.y + 1 : move.end.y - 1;
		setPiece(move.end.x, capturedY, undoInfo.capturedPiece);
		removePiece(move.end);
	}
	else
	{
		// Restore captured piece (if any)
		if (!undoInfo.capturedPiece.isEmpty())
		{
			setPiece(move.end, undoInfo.capturedPiece);
		}
		else
		{
			removePiece(move.end);
		}
	}

	// Restore king position if king moved
	if (movingPiece.type == PieceType::King)
	{
		updateKingPosition(move.start, movingPiece.color);
	}

	// Restore game state
	mEnPassantTarget		 = undoInfo.enPassantTarget;
	mWhiteCanCastleKingside	 = undoInfo.whiteCanCastleKingside;
	mWhiteCanCastleQueenside = undoInfo.whiteCanCastleQueenside;
	mBlackCanCastleKingside	 = undoInfo.blackCanCastleKingside;
	mBlackCanCastleQueenside = undoInfo.blackCanCastleQueenside;
	mHalfMoveClock			 = undoInfo.halfMoveClock;
	mFullMoveNumber			 = undoInfo.fullMoveNumber;

	// Switch player back
	switchPlayer();

	mHashKeyValid = false;
}


std::vector<Position> LightChessBoard::getPiecePositions(PlayerColor player) const
{
	std::vector<Position> positions;

	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			const LightPiece &piece = getPiece(x, y);

			if (!piece.isEmpty() && piece.color == player)
				positions.emplace_back(x, y);
		}
	}

	return positions;
}


std::vector<Position> LightChessBoard::getPiecePositions(PlayerColor player, PieceType type) const
{
	std::vector<Position> positions;

	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			const LightPiece &piece = getPiece(x, y);

			if (!piece.isEmpty() && piece.color == player && piece.type == type)
				positions.emplace_back(x, y);
		}
	}

	return positions;
}


int LightChessBoard::getMaterialValue(PlayerColor player) const
{
	int value = 0;

	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			const LightPiece &piece = getPiece(x, y);

			if (!piece.isEmpty() && piece.color == player)
				value += PIECE_VALUES[static_cast<int>(piece.type)];
		}
	}

	return value;
}


int LightChessBoard::getPieceCount(PlayerColor player) const
{
	int count = 0;

	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			const LightPiece &piece = getPiece(x, y);

			if (!piece.isEmpty() && piece.color == player)
				count++;
		}
	}

	return count;
}


int LightChessBoard::getPieceCount(PlayerColor player, PieceType type) const
{
	int count = 0;

	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			const LightPiece &piece = getPiece(x, y);

			if (!piece.isEmpty() && piece.color == player && piece.type == type)
				count++;
		}
	}
	return count;
}


bool LightChessBoard::isSquareAttacked(Position pos, PlayerColor attacker) const
{
	// Check all pieces of the attacking color
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			const LightPiece &piece = getPiece(x, y);

			if (piece.isEmpty() || piece.color != attacker)
				continue;

			Position from{x, y};

			if (canPieceMove(from, pos, piece.type, piece.color))
				return true;
		}
	}
	return false;
}


bool LightChessBoard::isInCheck(PlayerColor player) const
{
	Position	kingPos	 = getKingPosition(player);
	PlayerColor opponent = (player == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;
	return isSquareAttacked(kingPos, opponent);
}


std::vector<Position> LightChessBoard::getAttackingSquares(Position target, PlayerColor attacker) const
{
	return std::vector<Position>();
}


bool LightChessBoard::canPieceMove(Position from, Position to, PieceType piece, PlayerColor player) const
{
	if (!isValidPosition(from) || !isValidPosition(to) || from == to)
		return false;

	int dx	  = to.x - from.x;
	int dy	  = to.y - from.y;
	int absDx = abs(dx);
	int absDy = abs(dy);

	switch (piece)
	{
	case PieceType::Pawn:
	{
		int direction = (player == PlayerColor::White) ? -1 : 1;
		if (dx == 0) // Forward move
		{
			if (dy == direction && isEmpty(to))
				return true;
			if (dy == 2 * direction && from.y == ((player == PlayerColor::White) ? 6 : 1) && isEmpty(to) && isEmpty(from.x, from.y + direction))
				return true;
		}
		else if (absDx == 1 && dy == direction) // Capture
		{
			if (!isEmpty(to) && getPiece(to).color != player)
				return true;
			// En passant
			if (to == mEnPassantTarget)
				return true;
		}
	}
	break;

	case PieceType::Knight: return (absDx == 2 && absDy == 1) || (absDx == 1 && absDy == 2);

	case PieceType::Bishop: return absDx == absDy && isPathClear(from, to);

	case PieceType::Rook: return (dx == 0 || dy == 0) && isPathClear(from, to);

	case PieceType::Queen: return (dx == 0 || dy == 0 || absDx == absDy) && isPathClear(from, to);

	case PieceType::King: return absDx <= 1 && absDy <= 1;

	default: return false;
	}

	return false;
}


std::vector<PossibleMove> LightChessBoard::generatePseudoLegalMoves(PlayerColor player) const
{
	std::vector<PossibleMove> moves;

	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			const LightPiece &piece = getPiece(x, y);
			if (!piece.isEmpty() && piece.color == player)
			{
				Position			  from{x, y};
				std::vector<Position> targets;

				switch (piece.type)
				{
				case PieceType::Pawn: targets = getPawnMoves(from, player); break;
				case PieceType::Knight: targets = getKnightMoves(from); break;
				case PieceType::Bishop: targets = getDiagonalMoves(from); break;
				case PieceType::Rook: targets = getOrthogonalMoves(from); break;
				case PieceType::Queen:
				{
					auto diag = getDiagonalMoves(from);
					auto orth = getOrthogonalMoves(from);
					targets.insert(targets.end(), diag.begin(), diag.end());
					targets.insert(targets.end(), orth.begin(), orth.end());
				}
				break;
				case PieceType::King:
					targets = getKingMoves(from);
					// Add castling moves
					if (isCastlingLegal(player, true))
					{
						Position kingsideTo = {6, (player == PlayerColor::White) ? 7 : 0};
						moves.emplace_back(from, kingsideTo, MoveType::CastlingKingside);
					}
					if (isCastlingLegal(player, false))
					{
						Position queensideTo = {2, (player == PlayerColor::White) ? 7 : 0};
						moves.emplace_back(from, queensideTo, MoveType::CastlingQueenside);
					}
					break;
				}

				// Convert targets to moves
				for (const Position &to : targets)
				{
					MoveType type = MoveType::Normal;

					// Check for capture
					if (!isEmpty(to) && getPiece(to).color != player)
					{
						type = static_cast<MoveType>(type | MoveType::Capture);
					}

					// Check for pawn promotion
					if (piece.type == PieceType::Pawn)
					{
						int promotionRank = (player == PlayerColor::White) ? 0 : 7;
						if (to.y == promotionRank)
						{
							type = static_cast<MoveType>(type | MoveType::PawnPromotion);
							// Add different promotion pieces
							moves.emplace_back(from, to, type, PieceType::Queen);
							moves.emplace_back(from, to, type, PieceType::Rook);
							moves.emplace_back(from, to, type, PieceType::Bishop);
							moves.emplace_back(from, to, type, PieceType::Knight);
							continue;
						}

						// Check for en passant
						if (to == mEnPassantTarget)
						{
							type = static_cast<MoveType>(type | MoveType::EnPassant);
						}
					}

					moves.emplace_back(from, to, type);
				}
			}
		}
	}

	return moves;
}


std::vector<PossibleMove> LightChessBoard::generateLegalMoves(PlayerColor player) const
{
	auto					  pseudoLegalMoves = generatePseudoLegalMoves(player);
	std::vector<PossibleMove> legalMoves;

	for (const auto &move : pseudoLegalMoves)
	{
		if (isMoveLegal(move, player))
		{
			legalMoves.push_back(move);
		}
	}

	return legalMoves;
}


bool LightChessBoard::isMoveLegal(const PossibleMove &move, PlayerColor player) const
{
	// Make a copy to test the move
	LightChessBoard testBoard(*this);
	testBoard.makeMove(move);

	// Check if this move leaves the king in check
	return !testBoard.isInCheck(player);
}


uint64_t LightChessBoard::getHashKey() const
{
	if (!mHashKeyValid)
	{
		calculateHashKey();
		mHashKeyValid = true;
	}
	return mHashKey;
}


bool LightChessBoard::isEndgame() const
{
	// Simple endgame detection: total material < 2500 centipawns or few pieces
	int totalMaterial = getMaterialValue(PlayerColor::White) + getMaterialValue(PlayerColor::Black);
	int totalPieces	  = getPieceCount(PlayerColor::White) + getPieceCount(PlayerColor::Black);

	return totalMaterial < 2500 || totalPieces <= 12;
}


int LightChessBoard::getGamePhaseValue() const
{
	// Calculate game phase based on remaining material
	int		  totalMaterial		= getMaterialValue(PlayerColor::White) + getMaterialValue(PlayerColor::Black);

	// Starting material is approximately 7800 (39*2*100)
	const int STARTING_MATERIAL = 7800;
	const int ENDGAME_MATERIAL	= 2500;

	if (totalMaterial >= STARTING_MATERIAL)
		return 0; // Opening
	else if (totalMaterial <= ENDGAME_MATERIAL)
		return 2; // Endgame
	else
		return 1; // Middlegame
}


void LightChessBoard::initializePieces()
{
	// White pieces
	setPiece(0, 7, LightPiece(PieceType::Rook, PlayerColor::White));
	setPiece(1, 7, LightPiece(PieceType::Knight, PlayerColor::White));
	setPiece(2, 7, LightPiece(PieceType::Bishop, PlayerColor::White));
	setPiece(3, 7, LightPiece(PieceType::Queen, PlayerColor::White));
	setPiece(4, 7, LightPiece(PieceType::King, PlayerColor::White));
	setPiece(5, 7, LightPiece(PieceType::Bishop, PlayerColor::White));
	setPiece(6, 7, LightPiece(PieceType::Knight, PlayerColor::White));
	setPiece(7, 7, LightPiece(PieceType::Rook, PlayerColor::White));

	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		setPiece(x, 6, LightPiece(PieceType::Pawn, PlayerColor::White));
	}

	// Black pieces
	setPiece(0, 0, LightPiece(PieceType::Rook, PlayerColor::Black));
	setPiece(1, 0, LightPiece(PieceType::Knight, PlayerColor::Black));
	setPiece(2, 0, LightPiece(PieceType::Bishop, PlayerColor::Black));
	setPiece(3, 0, LightPiece(PieceType::Queen, PlayerColor::Black));
	setPiece(4, 0, LightPiece(PieceType::King, PlayerColor::Black));
	setPiece(5, 0, LightPiece(PieceType::Bishop, PlayerColor::Black));
	setPiece(6, 0, LightPiece(PieceType::Knight, PlayerColor::Black));
	setPiece(7, 0, LightPiece(PieceType::Rook, PlayerColor::Black));

	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		setPiece(x, 1, LightPiece(PieceType::Pawn, PlayerColor::Black));
	}
}


bool LightChessBoard::isPathClear(Position from, Position to) const
{
	int dx = (to.x > from.x) ? 1 : (to.x < from.x) ? -1 : 0;
	int dy = (to.y > from.y) ? 1 : (to.y < from.y) ? -1 : 0;

	int x  = from.x + dx;
	int y  = from.y + dy;

	while (x != to.x || y != to.y)
	{
		if (!isEmpty(x, y))
			return false;

		x += dx;
		y += dy;
	}

	return true;
}


std::vector<Position> LightChessBoard::getKnightMoves(Position pos) const
{
	std::vector<Position>							moves;
	static const std::array<std::pair<int, int>, 8> knightMoves = {{{1, 2}, {2, 1}, {-1, 2}, {-2, 1}, {1, -2}, {2, -1}, {-1, -2}, {-2, -1}}};

	for (const auto &offset : knightMoves)
	{
		Position to{pos.x + offset.first, pos.y + offset.second};
		if (isValidPosition(to))
		{
			const LightPiece &targetPiece = getPiece(to);
			const LightPiece &movingPiece = getPiece(pos);

			// Can move to empty square or capture opponent piece
			if (targetPiece.isEmpty() || targetPiece.color != movingPiece.color)
			{
				moves.push_back(to);
			}
		}
	}

	return moves;
}


std::vector<Position> LightChessBoard::getKingMoves(Position pos) const
{
	std::vector<Position>							moves;
	static const std::array<std::pair<int, int>, 8> kingMoves = {{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}}};

	for (const auto &offset : kingMoves)
	{
		Position to{pos.x + offset.first, pos.y + offset.second};
		if (isValidPosition(to))
		{
			const LightPiece &targetPiece = getPiece(to);
			const LightPiece &movingPiece = getPiece(pos);

			if (targetPiece.isEmpty() || targetPiece.color != movingPiece.color)
			{
				moves.push_back(to);
			}
		}
	}

	return moves;
}


std::vector<Position> LightChessBoard::getPawnMoves(Position pos, PlayerColor player) const
{
	std::vector<Position> moves;
	int					  direction = (player == PlayerColor::White) ? -1 : 1;
	int					  startRank = (player == PlayerColor::White) ? 6 : 1;

	// Forward move
	Position			  oneStep{pos.x, pos.y + direction};
	if (isValidPosition(oneStep) && isEmpty(oneStep))
	{
		moves.push_back(oneStep);

		// Two-step move from starting position
		if (pos.y == startRank)
		{
			Position twoStep{pos.x, pos.y + 2 * direction};
			if (isValidPosition(twoStep) && isEmpty(twoStep))
			{
				moves.push_back(twoStep);
			}
		}
	}

	// Captures
	for (int dx : {-1, 1})
	{
		Position capturePos{pos.x + dx, pos.y + direction};
		if (isValidPosition(capturePos))
		{
			const LightPiece &target = getPiece(capturePos);

			// Regular capture
			if (!target.isEmpty() && target.color != player)
			{
				moves.push_back(capturePos);
			}

			// En passant
			if (capturePos == mEnPassantTarget)
			{
				moves.push_back(capturePos);
			}
		}
	}

	return moves;
}


std::vector<Position> LightChessBoard::getSlidingMoves(Position pos, const std::array<std::pair<int, int>, 4> &directions) const
{
	std::vector<Position> moves;
	const LightPiece	 &movingPiece = getPiece(pos);

	for (const auto &dir : directions)
	{
		Position current{pos.x + dir.first, pos.y + dir.second};

		while (isValidPosition(current))
		{
			const LightPiece &target = getPiece(current);

			if (target.isEmpty())
			{
				moves.push_back(current);
			}
			else
			{
				// Can capture opponent piece
				if (target.color != movingPiece.color)
				{
					moves.push_back(current);
				}
				break; // Stop sliding in this direction
			}

			current.x += dir.first;
			current.y += dir.second;
		}
	}

	return moves;
}


std::vector<Position> LightChessBoard::getDiagonalMoves(Position pos) const
{
	static const std::array<std::pair<int, int>, 4> diagonalDirs = {{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
	return getSlidingMoves(pos, diagonalDirs);
}


std::vector<Position> LightChessBoard::getOrthogonalMoves(Position pos) const
{
	static const std::array<std::pair<int, int>, 4> orthogonalDirs = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
	return getSlidingMoves(pos, orthogonalDirs);
}


bool LightChessBoard::isCastlingPathClear(PlayerColor player, bool kingside) const
{
	int y = (player == PlayerColor::White) ? 7 : 0;

	if (kingside)
	{
		// Check squares between king and rook (f1/f8, g1/g8)
		return isEmpty(5, y) && isEmpty(6, y);
	}
	else
	{
		// Check squares between king and rook (d1/d8, c1/c8, b1/b8)
		return isEmpty(3, y) && isEmpty(2, y) && isEmpty(1, y);
	}
}


bool LightChessBoard::wouldKingPassThroughCheck(PlayerColor player, bool kingside) const
{
	PlayerColor opponent = (player == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;
	int			y		 = (player == PlayerColor::White) ? 7 : 0;

	if (kingside)
	{
		// King passes through f1/f8 and g1/g8
		return isSquareAttacked({5, y}, opponent) || isSquareAttacked({6, y}, opponent);
	}
	else
	{
		// King passes through d1/d8 and c1/c8
		return isSquareAttacked({3, y}, opponent) || isSquareAttacked({2, y}, opponent);
	}
}


void LightChessBoard::executeCastling(const PossibleMove &move)
{
	LightPiece king = getPiece(move.start);

	// Move king
	setPiece(move.end, king);
	removePiece(move.start);

	// Move rook
	int		   rookStartX = ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside) ? 7 : 0;
	int		   rookEndX	  = ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside) ? 5 : 3;
	int		   y		  = (king.color == PlayerColor::White) ? 7 : 0;

	LightPiece rook		  = getPiece(rookStartX, y);
	setPiece(rookEndX, y, rook);
	removePiece(rookStartX, y);
}


void LightChessBoard::executeEnPassant(const PossibleMove &move)
{
	LightPiece pawn = getPiece(move.start);

	// Move pawn
	setPiece(move.end, pawn);
	removePiece(move.start);

	// Remove captured pawn
	int capturedY = (pawn.color == PlayerColor::White) ? move.end.y + 1 : move.end.y - 1;
	removePiece(move.end.x, capturedY);
}


void LightChessBoard::updateGameState(const PossibleMove &move)
{
	LightPiece movingPiece = getPiece(move.end);

	// Update castling rights
	if (movingPiece.type == PieceType::King)
	{
		setCastlingRights(movingPiece.color, false, false);
	}
	else if (movingPiece.type == PieceType::Rook)
	{
		if (move.start.x == 0) // Queenside rook
		{
			if (movingPiece.color == PlayerColor::White)
				mWhiteCanCastleQueenside = false;
			else
				mBlackCanCastleQueenside = false;
		}
		else if (move.start.x == 7) // Kingside rook
		{
			if (movingPiece.color == PlayerColor::White)
				mWhiteCanCastleKingside = false;
			else
				mBlackCanCastleKingside = false;
		}
	}

	// Update en passant target
	mEnPassantTarget = {-1, -1};
	if (movingPiece.type == PieceType::Pawn && abs(move.end.y - move.start.y) == 2)
	{
		mEnPassantTarget = {move.start.x, (move.start.y + move.end.y) / 2};
	}

	// Update move counters
	if (movingPiece.type == PieceType::Pawn || ((move.type & MoveType::Capture) == MoveType::Capture))
	{
		mHalfMoveClock = 0;
	}
	else
	{
		mHalfMoveClock++;
	}

	if (mCurrentPlayer == PlayerColor::Black)
	{
		mFullMoveNumber++;
	}

	// Switch player
	switchPlayer();
}


void LightChessBoard::calculateHashKey() const
{
	mHashKey = 0;

	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			const LightPiece &piece = getPiece(x, y);
			if (!piece.isEmpty())
			{
				mHashKey ^= (static_cast<uint64_t>(piece.type) + 1) * (x * 8 + y + 1) * (piece.color == PlayerColor::White ? 1 : 2);
			}
		}
	}

	// Include game state in hash
	if (mCurrentPlayer == PlayerColor::Black)
		mHashKey ^= 0x1;
	if (mWhiteCanCastleKingside)
		mHashKey ^= 0x2;
	if (mWhiteCanCastleQueenside)
		mHashKey ^= 0x4;
	if (mBlackCanCastleKingside)
		mHashKey ^= 0x8;
	if (mBlackCanCastleQueenside)
		mHashKey ^= 0x10;
	if (mEnPassantTarget.isValid())
		mHashKey ^= (static_cast<unsigned long long>(mEnPassantTarget.x) * 8 + mEnPassantTarget.y) << 5;
}
