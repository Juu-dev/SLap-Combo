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

# class ListImageSlash():
#     def __init__(self, link, n):
#         self.list_image = []
#         for i in range(n):
#             image = pygame.image.load(link + str(i+1) + ".png").convert_alpha()
#             self.list_image.append(image)
            # self.list_image.append(pygame.transform.scale(image, (300, 400)))
            
class ListCropedImageSlash():
    def __init__(self, link, n):
        self.list_image = []
        n = n - 1
        start_x = slash_img_crop_info[n]["dimension"][0]
        start_y = slash_img_crop_info[n]["dimension"][1]
        width = slash_img_crop_info[n]["dimension"][2]
        height = slash_img_crop_info[n]["dimension"][3]
        n = slash_img_crop_info[n]["n"]
        
        for i in range(n):
            image = Image.open(link + str(i+1) + ".png")
            image = image.crop((start_x, start_y, start_x + width, start_y + height))
            # convert pillow image to pygame image
            image = pygame.image.fromstring(image.tobytes(), image.size, image.mode)
            self.list_image.append(image)
    
class Slash(pygame.sprite.Sprite):
    def __init__(self, speed: int, link_slash: str, n:int, slash_start_x:int, slash_start_y:int):
        super().__init__()
        self.attack_animation = False
        
        self.sprites = ListCropedImageSlash(link_slash, n).list_image
        
        self.current_sprite = 0
        self.image = self.sprites[int(self.current_sprite)]
        
        self.rect = self.image.get_rect(topleft = [slash_start_x, slash_start_y])
        self.speed = speed
        
    def attack(self):
        self.attack_animation = True
        
    def attack_animation_update(self):
        if self.attack_animation == True:
            self.current_sprite += self.speed
            if int(self.current_sprite) >= len(self.sprites):
                self.current_sprite = 0
                self.attack_animation = False
                
        self.image = self.sprites[int(self.current_sprite)]
        
    def update(self):
        self.attack_animation_update()