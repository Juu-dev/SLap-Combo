import pygame
from Slash import Slash, ListImageSlash, ListCropedImageSlash
from sys import exit

# result crop image
slash_img_crop_info = [
{
    "folder": "1",
    "n": 10,
    "dimension": [160, 60, 300, 300]
},
{
    "folder": "2",
    "n": 5,
    "dimension": [0, 210, 510, 100]
},
{
    "folder": "3",
    "n": 10,
    "dimension": [110, 70, 330, 370]
},
{
    "folder": "4",
    "n": 8,
    "dimension": [0, 200, 550, 200]
},
{
    "folder": "5",
    "n": 8,
    "dimension": [100, 150, 300, 300]
},
{
    "folder": "6",
    "n": 10,
    "dimension": [0, 0, 500, 500]
},
{
    "folder": "7",
    "n": 10,
    "dimension": [0, 0, 520, 400]
},
{
    "folder": "8",
    "n": 10,
    "dimension": [160, 50, 330, 380]
},
{
    "folder": "9",
    "n": 8,
    "dimension": [130, 30, 380, 300]
},
{
    "folder": "10",
    "n": 8,
    "dimension": [150, 80, 280, 350]
}]


# link_stask = 1 -> 5
img_slash = "../slash-sprite-cartoon-effects/PNG/10/"

# General setup
pygame.init()
clock = pygame.time.Clock()

# Potision
screen_width = 1080
screen_height = 600

slash_width = 50
slash_height = 50

# color
border_color = (255, 0, 0)

# Game Screen
screen = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption("Slap Combo")

# Background and Name
text_surface = pygame.font.Font(None, 50).render("My game!", False, 'Green')

# Creating sprites
moving_slash_sprites = pygame.sprite.Group()
# list_slash = ListImageSlash(img_slash, 5)
list_slash = ListCropedImageSlash(img_slash, 8, 150, 80, 280, 350)
slash = Slash(list_slash.list_image, slash_width, slash_height)
moving_slash_sprites.add(slash)

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
        if event.type == pygame.KEYDOWN:
            slash.attack()
    
    screen.fill((0, 0, 0))

    # draw background with blit and fit it to the screen
    screen.blit(text_surface, (500, 100))
    
    # draw sprites with red border
    moving_slash_sprites.draw(screen)
    moving_slash_sprites.update(0.25)
    pygame.draw.rect(screen, border_color, slash.rect, 2)
    
    # 

    pygame.display.update()
    clock.tick(60)
