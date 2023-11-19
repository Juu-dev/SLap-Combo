import pygame
import os
from Slash import Slash
from constant import KEY_ACTION_LEFT_MAPPING, LINK_SHINOBI_LEFT, KEY_ACTION_RIGHT_MAPPING

FRAME_WIDTH = 128
FRAME_HEIGHT = 128
ANIMATION_PLAYER_SPEED = 0.15
ANIMATION_SLASH_SPEED = 0.25


class Player(pygame.sprite.Sprite):
    def __init__(self, link_shinobi, player_start_x, player_start_y, speed, link_slash, n, speed_slash, direction):
        super().__init__()
        self.animations = {}
        self.load_animations(direction)
        self.current_animation = 'Idle'
        self.frame_index = 0
        self.image = self.animations[self.current_animation][self.frame_index]
        if direction == 'left':
            self.rect = self.image.get_rect(topleft = [player_start_x, player_start_y])
        else:
            self.rect = self.image.get_rect(topright = [player_start_x, player_start_y])
        self.last_update = pygame.time.get_ticks()
        self.last_update_action = 0

        # Collide
        self.hitbox = pygame.Rect(player_start_x, player_start_y, FRAME_WIDTH, FRAME_HEIGHT)

        # Slash
        self.slash = pygame.sprite.GroupSingle()
        self.speed_slash = speed_slash
        self.link_slash = link_slash
        self.n = n

        # Direction
        self.direction = direction

    def load_animations(self, direction):
        KEY_ACTION_MAPPING = KEY_ACTION_LEFT_MAPPING if direction == 'left' else KEY_ACTION_RIGHT_MAPPING
        for action in KEY_ACTION_MAPPING.values():
            sprite_sheet = pygame.image.load(os.path.join(LINK_SHINOBI_LEFT, f'{action}.png')).convert_alpha()
            num_frames = sprite_sheet.get_width() 
            frames = [sprite_sheet.subsurface(pygame.Rect(i * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT)) for i in range(int(num_frames/FRAME_WIDTH))]
            if direction == 'right':
                frames = [pygame.transform.flip(frame, True, False) for frame in frames]
            self.animations[action] = frames

        print("Action loading", len(self.animations))

    def change_animation(self, animation):
        now = pygame.time.get_ticks()
        if self.current_animation != animation and now > self.last_update_action + ANIMATION_PLAYER_SPEED * 1000:
            self.last_update_action = now
            self.current_animation = animation
            self.frame_index = 0

    def create_slash_left(self):
        x_pos = self.rect.x + self.image.get_width() - 50
        y_pos = self.rect.y + 20
        self.slash_ins = Slash(ANIMATION_SLASH_SPEED, self.link_slash, self.n, x_pos, y_pos, 'left')
        self.slash.add(self.slash_ins)

    def create_slash_right(self):
        x_pos = self.rect.x + self.image.get_width() - 100
        y_pos = self.rect.y + 20
        self.slash_ins = Slash(ANIMATION_SLASH_SPEED, self.link_slash, self.n, x_pos, y_pos, 'right')
        self.slash.add(self.slash_ins)

    def update(self):
        now = pygame.time.get_ticks()
        if now - self.last_update >= ANIMATION_PLAYER_SPEED * 1000:
            if self.current_animation != 'Idle' and self.frame_index == len(self.animations[self.current_animation])-1:
                self.frame_index = 0
                self.image = self.animations[self.current_animation][self.frame_index]
                self.change_animation('Idle')
            else:
                self.frame_index = (self.frame_index + 1) % len(self.animations[self.current_animation])
                self.image = self.animations[self.current_animation][self.frame_index]
                self.last_update = now
        if self.current_animation == 'Attack_1' and self.frame_index == 3:
            if self.direction == 'left':
                self.create_slash_left()
            else:
                self.create_slash_right()
            self.slash_ins.attack()
        if self.slash:
            # if current sprite is last sprite, kill slash
            self.slash_ins.update()
            if self.slash_ins.current_sprite == 0:
                self.slash_ins.kill()
