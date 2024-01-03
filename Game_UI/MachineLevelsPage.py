import pygame_gui
import pygame

class MachineLevelsPage:
    def __init__(self, width, height, manager, show_home_page, show_game_play_machine, player_not_available):
        self.width, self.height = width, height
        self.manager = manager
        self.show_home_page = show_home_page
        self.show_game_play_machine = show_game_play_machine
        self.level_buttons = {}
        self.create_levels()
        player_not_available()
        

    def create_levels(self):
        self.back_button = pygame_gui.elements.UIButton(
                relative_rect=pygame.Rect(20, 20, 100, 40),
                text='Back',
                manager=self.manager
            )
        # Example: Create buttons for different levels
        level_x = 100
        level_y = 100
        level_width = 200
        level_height = 50
        level_spacing = 60
        levels_per_line = 2

        for level in range(1, 11):
            button = pygame_gui.elements.UIButton(
                relative_rect=pygame.Rect(level_x, level_y, level_width, level_height),
                text=f'Level {level}',
                manager=self.manager
            )
            self.level_buttons[level] = button 

            level_x += level_width + level_spacing

            if level % levels_per_line == 0:
                level_x = 100
                level_y += level_height + level_spacing

    def handle_events(self, event):
        if event.type == pygame.USEREVENT and event.user_type == pygame_gui.UI_BUTTON_PRESSED:
            if event.ui_element == self.back_button:
                self.show_home_page()
            else:
                for level, button in self.level_buttons.items():
                    if event.ui_element == button:
                        print(f"Level {level} button pressed, let's play the game")
                        self.show_game_play_machine(level)
                        break