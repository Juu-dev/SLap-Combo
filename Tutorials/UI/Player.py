import pygame
from Slash import Slash

class SpriteSheet():
    def __init__(self, link):
        self.sheet = pygame.image.load(link)
        
    def split_image(self, n):
        frames = []
        width = self.sheet.get_width() / n
        height = self.sheet.get_height()
        for i in range(n):
            frames.append(self.sheet.subsurface((i * width, 0, width, height)))
            
        return frames

class Player(pygame.sprite.Sprite):
    def __init__(self, link_shinobi, player_start_x, player_start_y, speed, link_slash, n, speed_slash):
        super().__init__()
        self.attack_animation = False
        
        sprite_sheet = SpriteSheet(link_shinobi)
        image_list = sprite_sheet.split_image(5)
        
        self.sprites = image_list
        self.current_sprite = 0
        
        self.image = self.sprites[int(self.current_sprite)]
        # border image
        self.rect = self.image.get_rect(topleft = [player_start_x, player_start_y])
        
        self.slash = pygame.sprite.GroupSingle()
        self.speed = speed
        self.speed_slash = speed_slash
        self.link_slash = link_slash
        self.n = n
        
    def get_input(self):
        keys = pygame.key.get_pressed()
        if keys[pygame.K_SPACE]:
            self.attack()
            
    def attack(self):
        x_pos = self.rect.x + self.image.get_width() - 50
        y_pos = self.rect.y + 20
        self.slash.add(Slash(self.speed_slash, self.link_slash, self.n, x_pos, y_pos))
        self.slash.sprite.attack()
        self.attack_animation = True
        
    def attack_animation_update(self):
        if self.attack_animation == True:
            self.current_sprite += self.speed
            if int(self.current_sprite) >= len(self.sprites):
                self.current_sprite = 0
                self.attack_animation = False
                self.slash.sprite.kill()
        self.image = self.sprites[int(self.current_sprite)]
        
    def update(self):
        self.slash.update()
        self.get_input()
        self.attack_animation_update()