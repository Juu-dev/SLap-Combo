import pygame
from sys import exit
from Player import Player
from constant import *

class Game_Play:
    def __init__(self):

        self.count = 1

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
        self.player_left_ins = Player(LINK_SHINOBI_LEFT, FLOOR_PLAYER_LEFT_WIDTH, FLOOR_PLAYER_LEFT_HEIGHT, 0.1, LINK_SLASH, SLASH_INDEX, 0.2, 'left')
        self.player_left.add(self.player_left_ins)

        # Initialize right player
        self.player_right_ins = Player(LINK_SHINOBI_LEFT, FLOOR_PLAYER_RIGHT_WIDTH, FLOOR_PLAYER_RIGHT_HEIGHT, 0.1, LINK_SLASH, SLASH_INDEX, 0.2, 'right')
        self.player_right.add(self.player_right_ins)
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
            elif event.type == pygame.KEYDOWN:
                if event.key in KEY_ACTION_LEFT_MAPPING:
                    self.player_left_ins.change_animation(KEY_ACTION_LEFT_MAPPING[event.key])
            if event.type == pygame.KEYDOWN:
                if event.key in KEY_ACTION_RIGHT_MAPPING:
                    self.player_right_ins.change_animation(KEY_ACTION_RIGHT_MAPPING[event.key])

    def update(self):
        self.player_left_ins.update()
        self.player_right_ins.update()

    def draw_line_slash(self, player, direction):
        slash = player.slash.sprite
        if direction == 'right':
            rect = slash.image.get_rect(topright = [slash.slash_start_x, slash.slash_start_y])
        else:
            rect = slash.image.get_rect(topleft = [slash.slash_start_x, slash.slash_start_y])
        border_color = (255, 0, 0)
        pygame.draw.rect(self.screen, border_color, rect, 2)

    def draw(self):
        self.screen.fill((0, 0, 0))
        self.screen.blit(self.text_surface, (500, 100))
        # Draw player left
        self.player_left.draw(self.screen)
        # Draw player right
        self.player_right.draw(self.screen)
        # Check if the player_left has a slash before trying to draw it
        if self.player_left_ins.slash:
            self.player_left_ins.slash.draw(self.screen)
            # self.draw_line_slash(self.player_left_ins, 'left')
        if self.player_right_ins.slash:
            self.player_right_ins.slash.draw(self.screen)
            # self.draw_line_slash(self.player_right_ins, 'right')

        pygame.display.flip()
        self.clock.tick(FPS)

    def check_collision(self):
        if self.player_left_ins.slash:
            slash_hitbox = self.player_left_ins.slash.sprite.rect
            if self.player_right_ins.hitbox.colliderect(slash_hitbox):
                # print("check collision", self.count)
                # self.count = self.count + 1
                # print("Collision")
                self.player_right_ins.change_animation('Hurt')
                # self.player_right_ins.create_slash()
        if self.player_right_ins.slash:
            slash_hitbox = self.player_right_ins.slash.sprite.rect
            if self.player_left_ins.hitbox.colliderect(slash_hitbox):
                # print("check collision", self.count)
                # self.count = self.count + 1
                # print("Collision")
                self.player_left_ins.change_animation('Hurt')

    def run(self):
        while True:
            self.handle_events()
            self.update()
            self.check_collision()
            self.draw()

if __name__ == "__main__":
    game = Game_Play()
    game.run()
