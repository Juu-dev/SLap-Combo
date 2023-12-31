import pygame
from sys import exit

link_shinobi = "../shinobi-sprites-pixel-art/Shinobi/Attack_1.png"
# link_stask = 1 -> 5
link_slash = "../slash-sprite-cartoon-effects/PNG/2/"

class Player(pygame.sprite.Sprite):
    def __init__(self, image_list, player_start_x, player_start_y):
        super().__init__()
        self.attack_animation = False
        self.sprites = image_list
        
        self.current_sprite = 0
        self.image = self.sprites[self.current_sprite]
        
        self.rect = self.image.get_rect()
        self.rect.topleft = [player_start_x, player_start_y]
        
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
    
class ListImageSlash():
    def __init__(self, link, n):
        self.list_image = []
        for i in range(n):
            image = pygame.image.load(link + str(i+1) + ".png").convert_alpha()
            self.list_image.append(pygame.transform.scale(image, (300, 400)))
    
class Slash(pygame.sprite.Sprite):
    def __init__(self, image_list, slash_start_x, slash_start_y):
        super().__init__()
        self.attack_animation = False
        self.sprites = image_list
        
        self.current_sprite = 0
        self.image = self.sprites[self.current_sprite]
        
        # fit the image to the screen
        self.rect = self.image.get_rect()
        self.rect.topleft = [slash_start_x, slash_start_y]
        
    def attack(self):
        self.attack_animation = True
        
    def update(self, speed):
        if self.attack_animation == True:
            self.current_sprite += speed
            if int(self.current_sprite) >= len(self.sprites):
                self.current_sprite = 0
                self.attack_animation = False
                
        self.image = self.sprites[int(self.current_sprite)]

# name == main => run the code
# if __name__ == "__main__":

# General setup
pygame.init()
clock = pygame.time.Clock()

# Potision
screen_width = 1080
screen_height = 600
floor_player_width = 200
floor_player_height = 400
floor_slash_width = 300
floor_slash_height = 250

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
moving_player_sprites = pygame.sprite.Group()
player = Player(shinobi_frames, floor_player_width, floor_player_height)  # Pass the list of frames
moving_player_sprites.add(player)

moving_slash_sprites = pygame.sprite.Group()
list_slash = ListImageSlash(link_slash, 5)
slash = Slash(list_slash.list_image, floor_slash_width, floor_slash_height)
moving_slash_sprites.add(slash)


while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
        if event.type == pygame.KEYDOWN:
            player.attack()
            slash.attack()

    screen.fill((0, 0, 0))
    # draw background with blit and fit it to the screen
    # screen.blit(pygame.transform.scale(background, (1080, 600)), (0, 0))
    screen.blit(text_surface, (500, 100))

    # draw the sprites
    moving_player_sprites.draw(screen)
    moving_player_sprites.update(0.25)
    
    moving_slash_sprites.draw(screen)
    moving_slash_sprites.update(0.25)

    pygame.display.update()
    clock.tick(60)
