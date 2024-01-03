class GameState:
    def __init__(self):
        self.me = None
        self.you = None
        self.winner = None
        self.loser = None
        self.time = None

    def set_you(self, name, position):
        self.you = User(name, position)

    def set_me(self, name, position):
        self.me = User(name, position)

    def update_me(self, position):
        self.me.position = position

    def update_winner_loser(self, winner, loser, time):
        self.winner = winner
        self.loser = loser
        self.time = time

class User:
    def __init__(self, name, position):
        self.name = name
        self.position = position # left or right or none
        self.hp = 100