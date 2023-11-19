import pygame

KEY_ACTION_LEFT_MAPPING = {
    pygame.K_a: 'Attack_1',
    pygame.K_d: 'Dead',
    pygame.K_h: 'Hurt',
    pygame.K_i: 'Idle',
    pygame.K_r: 'Run',
    pygame.K_j: 'Jump',
    pygame.K_s: 'Shield',
    pygame.K_w: 'Walk'
}

KEY_ACTION_RIGHT_MAPPING = {
    pygame.K_1: 'Attack_1',
    pygame.K_2: 'Dead',
    pygame.K_3: 'Hurt',
    pygame.K_4: 'Idle',
    pygame.K_5: 'Run',
    pygame.K_6: 'Jump',
    pygame.K_7: 'Shield',
    pygame.K_8: 'Walk'
}

# LINK IMG
BACKGROUND_IMG = "./image/background/summer 1/Summer1.png"
SLASH_INDEX = 2
LINK_SLASH = "./image/Slash/" + str(SLASH_INDEX) +"/left/"
LINK_SHINOBI_LEFT = './image/Player/Shinobi_Left'

# SCREEN
SCREEN_WIDTH = 1080
SCREEN_HEIGHT = 600

# Potision
FLOOR_PLAYER_LEFT_WIDTH = 300
FLOOR_PLAYER_LEFT_HEIGHT = 200

FLOOR_PLAYER_RIGHT_WIDTH = 750
FLOOR_PLAYER_RIGHT_HEIGHT = 200

# FPS
FPS = 60

# DIMENSION SCROP IMG
SLASH_IMG_CROP_INFO = [
{
    "folder": "1",
    "n": 10,
    "dimension": [160, 60, 300, 300]
},
{
    "folder": "2",
    "n": 5,
    "dimension": [[0, 210, 180, 100],
                  [20, 210, 250, 100],
                  [0, 210, 350, 100],
                  [0, 210, 400, 100],
                  [0, 210, 510, 100],
                  ]
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
