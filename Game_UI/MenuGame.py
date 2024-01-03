import sys
import pygame_gui
import pygame
from SocketClient import SocketClient as Socket
from constant import SCREEN_WIDTH, SCREEN_HEIGHT, LOGIN_IMG

class MenuGame:
    def __init__(self, screen, manager, socket: Socket):
        pygame.display.set_caption('Menu Game')
        self.width, self.height = SCREEN_WIDTH, SCREEN_HEIGHT

        self.manager = manager
        self.socket = socket
        self.screen = screen

        self.menu_options = ['LOGIN', 'REGISTER', 'SETTING', 'EXIT']
        self.current_option = None

        self.create_menu_buttons()

    def create_menu_buttons(self):
        button_width, button_height = 200, 50
        total_height = (button_height + 20) * len(self.menu_options) - 20  # Total height of all buttons including spaces
        button_start_y = (self.height - total_height) // 2  # Adjust start position

        # Create buttons for each option
        for option in self.menu_options:
            button_rect = pygame.Rect(
                (self.width - button_width) // 2,  # Center horizontally
                button_start_y,
                button_width,
                button_height
            )

            button = pygame_gui.elements.UIButton(
                relative_rect=button_rect,
                text=option,
                manager=self.manager,
            )

            button_start_y += button_height + 20  # Move down for the next button

            setattr(self, f'{option}_button', button)

    def login_action(self):
        print("Redirecting to login page")

    def register_action(self):
        print("Redirecting to register page")

    def setting_action(self):
        print("Displaying settings")

    def exit_action(self):
        print("Exiting game")
        pygame.quit()
        self.socket.close()
        sys.exit()

    def handle_events(self, event):
        if event.type == pygame.QUIT:
            self.exit_action()
        elif event.type == pygame.USEREVENT:
            # Handle button clicks
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                # Check which button was clicked
                for option in self.menu_options:
                    # getattr example: getattr(x, 'foobar') is equivalent to x.foobar
                    button = getattr(self, f'{option}_button')
                    if event.ui_element == button:
                        self.current_option = option
                        break
