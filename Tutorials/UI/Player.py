import pygame
import os
from Slash import Slash

KEY_ACTION_MAPPING = {
    pygame.K_a: 'Attack_1',
    pygame.K_d: 'Dead',
    pygame.K_h: 'Hurt',
    pygame.K_i: 'Idle',
    pygame.K_r: 'Run',
    pygame.K_j: 'Jump',
    pygame.K_s: 'Shield',
    pygame.K_w: 'Walk'
}

# Kích thước mỗi frame trên sprite sheet (đổi lại theo kích thước thực tế của bạn)
FRAME_WIDTH = 128
FRAME_HEIGHT = 128
ANIMATION_SPEED = 0.15

class Player(pygame.sprite.Sprite):
    def __init__(self, link_shinobi, player_start_x, player_start_y, speed, link_slash, n, speed_slash):
        super().__init__()
        self.actions = {}
        self.load_sprites()
        self.action = 'Idle'
        self.frame_index = 0
        self.image = self.actions[self.action][self.frame_index]
        self.rect = self.image.get_rect()
        self.last_update = pygame.time.get_ticks()
        self.last_update_action = 0
        # Slash initialization
        self.slash = None
        self.speed_slash = speed_slash
        self.link_slash = link_slash
        self.n = n

    def load_sprites(self):
        for action in KEY_ACTION_MAPPING.values():
            sprite_sheet = pygame.image.load(os.path.join('./image/shinobi-sprites-pixel-art/Shinobi', f'{action}.png')).convert_alpha()
            num_frames = sprite_sheet.get_width() 
            frames = [sprite_sheet.subsurface(pygame.Rect(i * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT)) for i in range(int(num_frames/FRAME_WIDTH))]
            self.actions[action] = frames

    def change_action(self, action):
        now = pygame.time.get_ticks()
        if self.action != action and now > self.last_update_action + ANIMATION_SPEED * 1000:
            self.last_update_action = now
            self.action = action
            self.frame_index = 0
        # Create a new slash when performing the Attack_1 action
        if self.action == 'Attack_1':
            self.create_slash()
    
    def create_slash(self):
        # Set the position of the slash relative to the player
        x_pos = self.rect.x + self.image.get_width() - 50
        y_pos = self.rect.y + 20

        # Create a new Slash object
        self.slash = Slash(self.speed_slash, self.link_slash, self.n, x_pos, y_pos)


    def update(self):
        now = pygame.time.get_ticks()
        if now - self.last_update >= ANIMATION_SPEED * 1000:
            if self.action != 'Idle' and self.frame_index == len(self.actions[self.action])-1:
                self.frame_index = 0
                self.image = self.actions[self.action][self.frame_index]
                self.change_action('Idle')
                self.action_done = True
            else:
                self.frame_index = (self.frame_index + 1) % len(self.actions[self.action])
                self.image = self.actions[self.action][self.frame_index]
                self.last_update = now
        # Update the slash if it exists
        if self.slash:
            self.slash.update()