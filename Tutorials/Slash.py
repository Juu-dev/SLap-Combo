import pygame
from PIL import Image

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

class ListImageSlash():
    def __init__(self, link, n):
        self.list_image = []
        for i in range(n):
            image = pygame.image.load(link + str(i+1) + ".png").convert_alpha()
            self.list_image.append(image)
            # self.list_image.append(pygame.transform.scale(image, (300, 400)))
            
class ListCropedImageSlash():
    def __init__(self, link, n, start_x, start_y, width, height):
        self.list_image = []
        for i in range(n):
            image = Image.open(link + str(i+1) + ".png")
            image = image.crop((start_x, start_y, start_x + width, start_y + height))
            # convert pillow image to pygame image
            image = pygame.image.fromstring(image.tobytes(), image.size, image.mode)
            self.list_image.append(image)
    
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