#ifndef AGENT_H
#define AGENT_H

#include "include.h"
#include "utils.h"
#include "gameModel.h"

class Agent {
public:
    explicit Agent(Player player) : direction(player) {}

    virtual ~Agent() = default;

    void setGameModel(GameModel *gameModel) {
        game = gameModel;
    }

    virtual Action step() = 0;

protected:
    GameModel *game{nullptr};
    Player direction;
};

#endif //AGENT_H