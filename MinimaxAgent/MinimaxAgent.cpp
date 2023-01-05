//
// Created by boynextdoor on 12/27/22.
//

#include "MinimaxAgent.h"

bool MinimaxAgent::canKillGeneral(GameState state, Action action, Player player) {
	auto [x, y] = action;
	Piece target = state[y];
	if (target == Piece::RGeneral && player == Player::Black) {
		return true;
	}
	else if (target == Piece::BGeneral && player == Player::Red) {
		return true;
	}
	return false;
}

Action MinimaxAgent::step() {
	fprintf(stdout, "Minimax starts thinking...\n");
	GameState gameState = game->getGameState();
	size_t totalPieces = gameState.getSide(direction).size() + gameState.getSide(direction.reverse()).size();
	float fraction = 1 - ((float) totalPieces / 32.f);
	max_depth = 2 + (int) (fraction * 4);
	std::vector<Action> legalMoves = gameState.getLegalActionsBySide(direction);
	Action bestAction;
	double bestValue = -1e9;
	double alpha = -1e9;
	for (auto &action: legalMoves) {
		if (canKillGeneral(gameState, action, direction)) {
			return action;
		}
	}
//#pragma omp parallel for default(none) schedule(dynamic) firstprivate(legalMoves, gameState) shared(bestAction, bestValue, alpha)
	for (auto &action: legalMoves) {
		double value = minValue(gameState.getNextState(action), 1, direction.reverse(), alpha, 1e9);
		if (value > bestValue) {
			bestValue = value;
			bestAction = action;
		}
		alpha = std::max(alpha, value);
	}
	fprintf(stdout, "Minimax finished thinking...\n");
	return bestAction;
}

double MinimaxAgent::evaluationFunction(GameState state) {
	state.swapDirection();
	Player winner = state.getWinner();
	state.swapDirection();
	if (winner == direction)
		return 1e8;
	else if (winner == direction.reverse())
		return -1e8;
	std::vector<Position> myPiece = state.getSide(direction);
	std::vector<Position> enemyPiece = state.getSide(direction.reverse());
	double myScore = 0;
	double enemyScore = 0;
	std::unordered_map<Position, std::vector<Position>> myThreat = state.getThreatBySide(direction);
	std::unordered_map<Position, std::vector<Position>> enemyThreat = state.getThreatBySide(direction.reverse());
	for (auto piece: myPiece) {
		auto [x, y] = piece;
		Piece myPieceType = state[piece];
		myScore += (pieceValue[myPieceType.value()] * pieceScore[myPieceType.value()][x][y]);

		std::vector<Position> attackPosition = state.getRange(piece);
		int flexibility = 0;
		for (auto &position: attackPosition) {
			Piece pieceType = state[position];
			if (pieceType == Piece::NoneType)
				flexibility++;
		}
		myScore += flexibility * pieceFlexibility[myPieceType.value()];
		myScore -= static_cast<double>((size_t) pieceValue[myPieceType.value()] * myThreat[piece].size());
		std::vector<Position> protector = state.getProtectorBySide(direction, piece);
		myScore += static_cast<double>(protector.size() * (size_t) pieceValue[myPieceType.value()]);
	}
	for (auto piece: enemyPiece) {
		Piece enemyPieceType = state[piece];
		auto [x, y] = piece;
		enemyScore += (pieceValue[enemyPieceType.value()] * pieceScore[enemyPieceType.value()][x][y]);

		std::vector<Position> attackPosition = state.getRange(piece);
		int flexibility = 0;
		for (auto &position: attackPosition) {
			Piece pieceType = state[position];
			if (pieceType == Piece::NoneType)
				flexibility++;
		}
		enemyScore += flexibility * pieceFlexibility[enemyPieceType.value()];
		enemyScore -= static_cast<double>((size_t) pieceValue[enemyPieceType.value()] * enemyThreat[piece].size());
		std::vector<Position> protector = state.getProtectorBySide(direction.reverse(), piece);
		enemyScore += static_cast<double>(protector.size() * (size_t) pieceValue[enemyPieceType.value()]);
	}
	return myScore - enemyScore;
}

double MinimaxAgent::maxValue(GameState state, int depth, Player player, double alpha, double beta) {
	if (depth == max_depth * 2 || state.isMatchOver())
		return evaluationFunction(state);
	double value = -1e9;
	std::vector<Action> legalActions = state.getLegalActionsBySide(player);
	for (auto &action: legalActions) {
		value = std::max(value, minValue(state.getNextState(action), depth + 1, player.reverse(), alpha, beta));
		if (value > beta)
			return value;
		alpha = std::max(alpha, value);
	}
	return value;
}

double MinimaxAgent::minValue(GameState state, int depth, Player player, double alpha, double beta) {
	if (depth == max_depth * 2 || state.isMatchOver())
		return evaluationFunction(state);
	double value = 1e9;
	std::vector<Action> legalActions = state.getLegalActionsBySide(player);
	for (auto &action: legalActions) {
		value = std::min(value, maxValue(state.getNextState(action), depth + 1, player.reverse(), alpha, beta));
		if (value < alpha)
			return value;
		beta = std::min(beta, value);
	}
	return value;
}