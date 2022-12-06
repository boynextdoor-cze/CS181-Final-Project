from agent import Agent
from gameModel import GameModel
from utils import Side
import random


class ExampleAgent(Agent):
    def __init__(self, direction: Side):
        super().__init__(direction)

    def step(self) -> tuple[tuple[int, int], tuple[int, int]]:
        all_pieces = self.game.getSide(self.direction)
        while True:
            pos: tuple[int, int] = random.choice(all_pieces)
            all_valid_move = self.game.getRange(pos)
            if len(all_valid_move) == 0:
                continue
            return pos, random.choice(all_valid_move)
