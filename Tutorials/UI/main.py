import pygame
from sys import exit
from Player import Player

BACKGROUND_IMG = "../image/summer-pixel-art-backgrounds/PNG/summer 1/Summer1.png"
LINK_SHINOBI = "../image/shinobi-sprites-pixel-art/Shinobi/Attack_1.png"

# link_stask = 1 -> 5
SLASH_INDEX = 2
LINK_SLASH = "../image/slash-sprite-cartoon-effects/PNG/" + str(SLASH_INDEX) + "/"

# SCREEN
SCREEN_WIDTH = 1080
SCREEN_HEIGHT = 600

# Potision
FLOOR_PLAYER_WIDTH = 200
FLOOR_PLAYER_HEIGHT = 400

# FPS
FPS = 60

class Game:
    def __init__(self):
        pygame.init()
        self.clock = pygame.time.Clock()

        # Initialize player
        self.player = pygame.sprite.GroupSingle()
        player_ins = Player(LINK_SHINOBI, FLOOR_PLAYER_WIDTH, FLOOR_PLAYER_HEIGHT, 0.2, LINK_SLASH, SLASH_INDEX, 0.2)
        self.player.add(player_ins)

        # Set up game screen
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Slap Combo")

        # Load background and text surface
        self.background = pygame.image.load(BACKGROUND_IMG).convert_alpha()
        self.text_surface = pygame.font.Font(None, 50).render("My game!", False, 'Green')
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
            if event.type == pygame.KEYDOWN:
                if event.key in KEY_ACTION_MAPPING:
                    self.player.sprite.change_action(KEY_ACTION_MAPPING[event.key])

    def update(self):
        self.player.sprite.update()

    def draw(self):
        self.screen.fill((0, 0, 0))
        self.screen.blit(self.text_surface, (500, 100))
        self.player.draw(self.screen)
        self.player.sprite.slash.draw(self.screen)

        pygame.display.flip()
        self.clock.tick(FPS)

    def run(self):
        while True:
            self.handle_events()
            self.update()
            self.draw()

if __name__ == "__main__":
    game = Game()
    game.run()