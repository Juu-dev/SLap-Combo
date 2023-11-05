import pygame
from sys import exit

link_shinobi = "../shinobi-sprites-pixel-art/Shinobi/Attack_1.png"

class Player(pygame.sprite.Sprite):
    def __init__(self, image_list, pos_x, pos_y):
        super().__init__()
        self.attack_animation = False
        self.sprites = image_list
        
        self.current_sprite = 0
        self.image = self.sprites[self.current_sprite]
        
        self.rect = self.image.get_rect()
        self.rect.topleft = [pos_x, pos_y]
        
    def attack(self):
        self.attack_animation = True
        
    def update(self, speed):
        if self.attack_animation == True:
            self.current_sprite += speed
            if int(self.current_sprite) >= len(self.sprites):
                self.current_sprite = 0
                self.attack_animation = False
                
        self.image = self.sprites[int(self.current_sprite)]
        
        
class SpriteSheet():
    def __init__(self, link):
        self.sheet = self.image = pygame.image.load(link).convert_alpha()
        
    def split_image(self, n):
        frames = []
        width = self.sheet.get_width() / n
        height = self.sheet.get_height()
        for i in range(n):
            frames.append(self.sheet.subsurface((i * width, 0, width, height)))
            
        return frames
    

# General setup
pygame.init()
clock = pygame.time.Clock()

# Potision
screen_width = 1080
screen_height = 600
floor_width = 200
floor_height = 400

# Game Screen
screen = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption("Slap Combo")

# Background and Name
background = pygame.image.load("../summer-pixel-art-backgrounds/PNG/summer 1/Summer1.png")
text_surface = pygame.font.Font(None, 50).render("My game!", False, 'Green')

# Create a sprite sheet = list of images
sprite_sheet = SpriteSheet(link_shinobi)
shinobi_frames = sprite_sheet.split_image(5)

# Creating the sprites and groups
moving_sprites = pygame.sprite.Group()
player = Player(shinobi_frames, floor_width, floor_height)  # Pass the list of frames
moving_sprites.add(player)

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
        if event.type == pygame.KEYDOWN:
            player.attack()

    # draw our elements

    # draw background with blit and fit it to the screen
    screen.blit(pygame.transform.scale(background, (1080, 600)), (0, 0))
    screen.blit(text_surface, (500, 100))

    # draw the sprites
    moving_sprites.draw(screen)
    moving_sprites.update(0.25)

    pygame.display.update()
    clock.tick(60)
