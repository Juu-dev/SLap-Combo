import pygame
from PIL import Image
from constant import SLASH_IMG_CROP_INFO

class ListCropedImageSlash():
    def __init__(self, link, n, right = True):
        self.list_image = []
        n = n - 1
        start_x = SLASH_IMG_CROP_INFO[n]["dimension"][0]
        start_y = SLASH_IMG_CROP_INFO[n]["dimension"][1]
        width = SLASH_IMG_CROP_INFO[n]["dimension"][2]
        height = SLASH_IMG_CROP_INFO[n]["dimension"][3]
        n = SLASH_IMG_CROP_INFO[n]["n"]
        
        for i in range(n):
            image = Image.open(link + str(i+1) + ".png")
            image = image.crop((start_x, start_y, start_x + width, start_y + height))
            # convert pillow image to pygame image
            image = pygame.image.fromstring(image.tobytes(), image.size, image.mode)
            # direction of slash
            if (right):
                image_reverse = pygame.transform.flip(image, True, False)
                self.list_image.append(image_reverse)
            else:
                self.list_image.append(image)
    
class Slash(pygame.sprite.Sprite):
    def __init__(self, speed: int, link_slash: str, n:int, slash_start_x:int, slash_start_y:int, direction: bool):
        super().__init__()
        self.attack_animation = False
        
        self.sprites = ListCropedImageSlash(link_slash, n, direction == 'right').list_image
        
        self.current_sprite = 0
        self.image = self.sprites[int(self.current_sprite)]
        
        self.rect = self.image.get_rect(topleft = [slash_start_x, slash_start_y])
        self.speed = speed
        self.direction = direction
        
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
        if self.direction == 'right':
            self.rect.x -= 10
        else:
            self.rect.x += 10
        self.attack_animation_update()