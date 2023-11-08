import pygame
import os
from sys import exit
from Player import Player

BACKGROUND_IMG = "../image/summer-pixel-art-backgrounds/PNG/summer 1/Summer1.png"
LINK_SHINOBI = "../image/shinobi-sprites-pixel-art/Shinobi/Attack_1.png"
# link_stask = 1 -> 5
SLASH_INDEX = 2
LINK_SLASH = "../image/slash-sprite-cartoon-effects/PNG/" + str(SLASH_INDEX) + "/"
# Potision
SCREEN_WIDTH = 1080
SCREEN_HEIGHT = 600
FLOOR_PLAYER_WIDTH = 200
FLOOR_PLAYER_HEIGHT = 400

FPS = 60

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
FRAME_WIDTH = 64
FRAME_HEIGHT = 64
ANIMATION_SPEED = 0.15

class Shinobi(pygame.sprite.Sprite):
    def __init__(self):
        super().__init__()
        self.actions = {}
        self.load_sprites()
        self.action = 'idle'
        self.frame_index = 0
        self.image = self.actions[self.action][self.frame_index]
        self.rect = self.image.get_rect()
        self.last_update = pygame.time.get_ticks()

    def load_sprites(self):
        for action in KEY_ACTION_MAPPING.values():
            sprite_sheet = pygame.image.load(os.path.join('image/shinobi-sprites-pixel-art/Shinobi', f'{action}.png')).convert_alpha()
            num_frames = sprite_sheet.get_width() 
            frames = [sprite_sheet.subsurface(pygame.Rect(i * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT)) for i in range(num_frames)]
            self.actions[action] = frames

    def update(self):
        now = pygame.time.get_ticks()
        if now - self.last_update > ANIMATION_SPEED * 1000:
            self.frame_index = (self.frame_index + 1) % len(self.actions[self.action])
            self.image = self.actions[self.action][self.frame_index]
            self.last_update = now

    def change_action(self, action):
        if self.action != action:
            self.action = action
            self.frame_index = 0

class Game:
    def __init__(self):
        self.player = pygame.sprite.GroupSingle() 
        player_ins = Player(LINK_SHINOBI, FLOOR_PLAYER_WIDTH, FLOOR_PLAYER_HEIGHT, 0.2, LINK_SLASH, SLASH_INDEX, 0.2)
        self.player.add(player_ins)
        
    def run(self):
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                if event.key in KEY_ACTION_MAPPING:
                    Player.change_action(KEY_ACTION_MAPPING[event.key])
        
        self.player.sprite.update()
        
        # draw
        self.player.draw(screen)
        self.player.sprite.slash.draw(screen)
        
        # draw border
        # pygame.draw.rect(screen, (255, 0, 0), self.player.sprite.rect, 2)

# name == main => run the code
if __name__ == "__main__":
    # General setup
    pygame.init()
    clock = pygame.time.Clock()
    game = Game()

    # Game Screen
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Slap Combo")

    # Background and Name
    background = pygame.image.load().convert_alpha()
    text_surface = pygame.font.Font(None, 50).render("My game!", False, 'Green')
    
    shinobi = Shinobi()
    all_sprites = pygame.sprite.Group(shinobi)

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
                

        screen.fill((0, 0, 0))
        # draw background with blit and fit it to the screen
        # screen.blit(pygame.transform.scale(background, (1080, 600)), (0, 0))
        screen.blit(text_surface, (500, 100))

        game.run()
        
        pygame.display.flip()
        clock.tick(FPS)

# if __name__ == '__main__':
#     main()
