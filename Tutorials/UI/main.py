import pygame
from sys import exit
from Player_Left import Player_Left
from Player_Right import Player_Right
from constant import *

class Game:
    def __init__(self):
        pygame.init()
        self.clock = pygame.time.Clock()

        # Set up game screen
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Slap Combo")

        # Load background and text surface
        self.background = pygame.image.load(BACKGROUND_IMG).convert_alpha()
        self.text_surface = pygame.font.Font(None, 50).render("My game!", False, 'Green')

        # Initialize sprite groups
        self.player_left = pygame.sprite.GroupSingle()
        self.player_right = pygame.sprite.GroupSingle()

        # Initialize left player
        player_ins = Player_Left(LINK_SHINOBI_LEFT, FLOOR_PLAYER_LEFT_WIDTH, FLOOR_PLAYER_LEFT_HEIGHT, 0.1, LINK_SLASH, SLASH_INDEX, 0.2)
        self.player_left.add(player_ins)

        # Initialize right player
        player_right_ins = Player_Right(LINK_SHINOBI_LEFT, FLOOR_PLAYER_RIGHT_WIDTH, FLOOR_PLAYER_RIGHT_HEIGHT, 0.1, LINK_SLASH, SLASH_INDEX, 0.2)
        self.player_right.add(player_right_ins)
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
            elif event.type == pygame.KEYDOWN:
                if event.key in KEY_ACTION_LEFT_MAPPING:
                    self.player_left.sprite.change_animation(KEY_ACTION_LEFT_MAPPING[event.key])
            if event.type == pygame.KEYDOWN:
                if event.key in KEY_ACTION_RIGHT_MAPPING:
                    self.player_right.sprite.change_animation(KEY_ACTION_RIGHT_MAPPING[event.key])

    def update(self):
        self.player_left.sprite.update()
        self.player_right.sprite.update()

    def draw(self):
        self.screen.fill((0, 0, 0))
        self.screen.blit(self.text_surface, (500, 100))
        # Draw player left
        self.player_left.draw(self.screen)
        # Draw player right
        self.player_right.draw(self.screen)
        # Check if the player_left has a slash before trying to draw it
        if self.player_left.sprite.slash:
            self.player_left.sprite.slash.draw(self.screen)
        if self.player_right.sprite.slash:
            self.player_right.sprite.slash.draw(self.screen)

        pygame.display.flip()
        self.clock.tick(FPS)

    def check_collision(self):
        if self.player_left.sprite.slash:
            slash_hitbox = self.player_left.sprite.slash.sprite.rect
            if self.player_right.sprite.hitbox.colliderect(slash_hitbox):
                print("Collision")
                # self.player_right.sprite.change_animation('Hurt')
                # self.player_right.sprite.create_slash()

    def run(self):
        while True:
            self.handle_events()
            self.update()
            self.check_collision()
            self.draw()

if __name__ == "__main__":
    game = Game()
    game.run()