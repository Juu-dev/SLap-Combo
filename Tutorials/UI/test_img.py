import pygame
from PIL import Image
from sys import exit

# result crop image
SLASH_IMG_CROP_INFO = [
{
    "folder": "2",
    "n": 5,
    "dimension": [[0, 210, 180, 100],
                  [20, 210, 250, 100],
                  [0, 210, 350, 100],
                  [0, 210, 400, 100],
                  [0, 210, 510, 100],
                  ]
}]

class ListCropedImageSlash():
    def __init__(self, link, n, right):
        self.list_image = []
        n = n - 1
        
        for i in range(n):
            image = Image.open(link + str(i+1) + ".png")
            start_x = SLASH_IMG_CROP_INFO[0]["dimension"][i][0]
            start_y = SLASH_IMG_CROP_INFO[0]["dimension"][i][1]
            width = SLASH_IMG_CROP_INFO[0]["dimension"][i][2]
            height = SLASH_IMG_CROP_INFO[0]["dimension"][i][3]
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
    def __init__(self, speed: int, link_slash: str, n:int, slash_start_x:int, slash_start_y:int, direction: str):
        super().__init__()
        self.attack_animation = False
        
        self.sprites = ListCropedImageSlash(link_slash, n, direction == 'right').list_image
        
        self.current_sprite = 0
        self.image = self.sprites[int(self.current_sprite)]
        
        if (direction == 'right'):
            print("right")
            self.rect = self.image.get_rect(topright = [slash_start_x, slash_start_y])
        else:
            self.rect = self.image.get_rect(topleft = [slash_start_x, slash_start_y])
        self.speed = speed
        self.direction = direction

        self.slash_start_x = slash_start_x
        self.slash_start_y = slash_start_y
        
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
        # if self.direction == 'right':
        #     self.rect.x -= 10
        # else:
        #     self.rect.x += 10
        self.attack_animation_update()
        self.rect = self.image.get_rect(topright = [self.slash_start_x, self.slash_start_y])
        rect = self.image.get_rect()
        if self.direction == 'right':
            rect = self.image.get_rect(topright = [self.slash_start_x, self.slash_start_y])
        else:
            rect = self.image.get_rect(topleft = [self.slash_start_x, self.slash_start_y])
        pygame.draw.rect(screen, border_color, rect, 2)


# link_stask = 1 -> 5
img_slash = "./image/Slash/2/left/"

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
# slash = Slash(0.25, img_slash, 6, 200, 200, 'left')
slash = Slash(0.25, img_slash, 6, 600, 200, 'right')
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
    moving_slash_sprites.update()

    pygame.display.update()
    clock.tick(60)
