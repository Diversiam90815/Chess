/*
  ==============================================================================
	Module:         LoggingHelper
	Description:    Helper class for simplifying logging messages
  ==============================================================================
*/

#include "LoggingHelper.h"
#include "Logging.h"


std::string LoggingHelper::playerColourToString(PlayerColor player)
{
	switch (player)
	{
	case PlayerColor::NoColor: return "No Color";
	case PlayerColor::White: return "White";
	case PlayerColor::Black: return "Black";
	default: return "Unknown PlayerColor";
	}
}


std::string LoggingHelper::positionToString(Position pos)
{
	return "(x=" + std::to_string(pos.x) + ", y=" + std::to_string(pos.y) + ")";
}


std::string LoggingHelper::pieceTypeToString(PieceType piece)
{
	switch (piece)
	{
	case PieceType::DefaultType: return "Default Type";
	case PieceType::Pawn: return "Pawn";
	case PieceType::Knight: return "Knight";
	case PieceType::Bishop: return "Bishop";
	case PieceType::Rook: return "Rook";
	case PieceType::Queen: return "Queen";
	case PieceType::King: return "King";
	default: return "Unknown PieceType";
	}
}


std::string LoggingHelper::gameStateToString(GameState state)
{
	switch (state)
	{
	case GameState::Undefined: return "Undefined";
	case GameState::Init: return "Init";
	case GameState::InitSucceeded: return "Init Succeeded";
	case GameState::MoveInitiated: return "Move Initiated";
	case GameState::ExecutingMove: return "Executing Move";
	case GameState::ValidatingMove: return "Validating Move";
	case GameState::WaitingForInput: return "Waiting For Input";
	case GameState::WaitingForTarget: return "Waiting For Target";
	case GameState::GameOver: return "Game Over";
	case GameState::PawnPromotion: return "Pawn Promotion";
	case GameState::WaitingForRemoteMove: return "Waiting for remote move";
	default: return "Unknown GameState";
	}
}


std::string LoggingHelper::moveTypeToString(MoveType type)
{
	if (type == MoveType::None)
		return "None";

	std::vector<std::string> types;

	// Helper lambda to check if a flag is set
	auto					 hasFlag = [&](MoveType flag) -> bool { return static_cast<int>(type) & static_cast<int>(flag); };

	if (hasFlag(MoveType::Normal))
		types.emplace_back("Normal");
	if (hasFlag(MoveType::DoublePawnPush))
		types.emplace_back("Double Pawn Push");
	if (hasFlag(MoveType::PawnPromotion))
		types.emplace_back("Pawn Promotion");
	if (hasFlag(MoveType::Capture))
		types.emplace_back("Capture");
	if (hasFlag(MoveType::EnPassant))
		types.emplace_back("En Passant");
	if (hasFlag(MoveType::CastlingKingside))
		types.emplace_back("Castling Kingside");
	if (hasFlag(MoveType::CastlingQueenside))
		types.emplace_back("Castling Queenside");
	if (hasFlag(MoveType::Check))
		types.emplace_back("Check");
	if (hasFlag(MoveType::Checkmate))
		types.emplace_back("Checkmate");

	std::string result;
	for (size_t i = 0; i < types.size(); ++i)
	{
		result += types[i];
		if (i != types.size() - 1)
			result += " | ";
	}

	return result;
}


std::string LoggingHelper::boardStateToString(const int *boardState)
{
	std::ostringstream oss;

	// Column labels
	oss << "  ";
	for (char col = 'a'; col < 'a' + BOARD_SIZE; ++col)
	{
		oss << " " << col;
	}
	oss << "\n";

	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		// Row label (8 to 1)
		oss << (BOARD_SIZE - y) << " ";

		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			// Access the 1D array as 2D
			int	 encoded = boardState[y * BOARD_SIZE + x];
			char piece	 = encodeToChar(encoded);
			oss << " " << piece;
		}

		oss << " " << (BOARD_SIZE - y) << "\n";
	}

	// Column labels at the bottom
	oss << "  ";
	for (char col = 'a'; col < 'a' + BOARD_SIZE; ++col)
	{
		oss << " " << col;
	}
	oss << "\n";

	return oss.str();
}


void LoggingHelper::logMove(Move &move)
{
	LOG_INFO("----- Move Number {} -----", move.number);

	LOG_INFO("\tPlayer:\t\t{}", playerColourToString(move.player).c_str());

	LOG_INFO("\tStart:\t\t{}", positionToString(move.startingPosition).c_str());
	LOG_INFO("\tEnd:\t\t{}", positionToString(move.endingPosition).c_str());

	LOG_INFO("\tMoved Piece:\t\t{}", pieceTypeToString(move.movedPiece).c_str());

	if (move.capturedPiece != PieceType::DefaultType)
	{
		LOG_INFO("\tCaptured Piece:\t\t{}", pieceTypeToString(move.capturedPiece).c_str());
	}
	else
	{
		LOG_INFO("\tCaptured Piece:\t\tNone");
	}

	if (move.promotionType != PieceType::DefaultType)
	{
		LOG_INFO("\tPromotion Type:\t\t{}", pieceTypeToString(move.promotionType).c_str());
	}
	else
	{
		LOG_INFO("\tPromotion Type:\t\tNone");
	}

	LOG_INFO("\tMove Type:\t\t{}", moveTypeToString(move.type).c_str());
	LOG_INFO("\tNotation:\t\t{}", move.notation.c_str());
	LOG_INFO("\tHalf Move Clock:\t{}", move.halfMoveClock);

	LOG_INFO("------------------------");
}


void LoggingHelper::logBoardState(const int *boardState)
{
	std::string boardStateString = boardStateToString(boardState);
	LOG_INFO("\n{}", boardStateString.c_str());
}


char LoggingHelper::encodeToChar(int encoded)
{
	int	 colorVal  = (encoded >> 4) & 0xF; // High nibble
	int	 typeVal   = encoded & 0xF;		   // Low nibble

	// Map PieceType to characters
	char pieceChar = '.';

	switch (static_cast<PieceType>(typeVal))
	{
	case PieceType::DefaultType: pieceChar = '.'; break;
	case PieceType::Pawn: pieceChar = 'P'; break;
	case PieceType::Knight: pieceChar = 'N'; break;
	case PieceType::Bishop: pieceChar = 'B'; break;
	case PieceType::Rook: pieceChar = 'R'; break;
	case PieceType::Queen: pieceChar = 'Q'; break;
	case PieceType::King: pieceChar = 'K'; break;
	default:
		pieceChar = '?'; // Unknown piece type
		break;
	}

	// Apply color -> Uppercase for White, Lowercase for Black (pieceChar is already uppercase)
	if (colorVal == static_cast<int>(PlayerColor::Black))
	{
		pieceChar = tolower(pieceChar);
	}

	return pieceChar;
}
