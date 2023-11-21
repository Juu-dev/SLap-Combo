import pygame
import pygame_gui
import sys

FAKE_PLAYERS = [
    {"username": "Player1", "health": 100, "level": 5},
    {"username": "Player2", "health": 90, "level": 4},
    {"username": "Player3", "health": 110, "level": 6},
    {"username": "Player4", "health": 110, "level": 6},
    {"username": "Player5", "health": 110, "level": 6},
    {"username": "Player6", "health": 110, "level": 6},
    {"username": "Player7", "health": 110, "level": 6},
    # Add more fake players as needed
]

class LevelsPage:
    def __init__(self, width, height, manager, on_back):
        self.width, self.height = width, height
        self.manager = manager
        self.on_back = on_back

        self.create_levels()

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
            level_button = pygame_gui.elements.UIButton(
                relative_rect=pygame.Rect(level_x, level_y, level_width, level_height),
                text=f'Level {level}',
                manager=self.manager
            )

            level_x += level_width + level_spacing

            if level % levels_per_line == 0:
                level_x = 100
                level_y += level_height + level_spacing

    def handle_events(self, event):
        # Handle level selection events
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.back_button:
                    self.on_back()
                # Check which level button was pressed

class PlayersPage:
    def __init__(self, width, height, manager, on_back):
        self.width, self.height = width, height
        self.manager = manager
        self.on_back = on_back

        self.create_players_list()

    def create_players_list(self):
        self.back_button = pygame_gui.elements.UIButton(
                relative_rect=pygame.Rect(20, 20, 100, 40),
                text='Back',
                manager=self.manager
            )
        # Example: Create a list of player buttons
        self.player1_button = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(100, 100, 200, 50),
            text='Player 1: Challenge',
            manager=self.manager
        )
        
    def handle_events(self, event):
        # Handle level selection events
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.back_button:
                    self.on_back()
                # Check which level button was pressed

class PlayersPage:
    def __init__(self, width, height, manager, on_back):
        self.width, self.height = width, height
        self.manager = manager
        self.on_back = on_back

        self.create_players_list()

    def create_players_list(self):
        self.back_button = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(20, 20, 100, 40),
            text='Back',
            manager=self.manager
        )

        # Create a table to display player information
        # Table setup
        table_start_x, table_start_y = 100, 100
        row_height = 50
        column_widths = [100, 100, 50, 150]  # Widths for username, health, level, challenge button

        for index, player in enumerate(FAKE_PLAYERS):
            # Positioning for each row
            row_y = table_start_y + index * row_height

            # Create labels and buttons for each player
            username_label = pygame_gui.elements.UILabel(
                relative_rect=pygame.Rect(table_start_x, row_y, column_widths[0], row_height),
                text=player["username"],
                manager=self.manager
            )

            health_label = pygame_gui.elements.UILabel(
                relative_rect=pygame.Rect(table_start_x + column_widths[0], row_y, column_widths[1], row_height),
                text=str(player["health"]),
                manager=self.manager
            )

            level_label = pygame_gui.elements.UILabel(
                relative_rect=pygame.Rect(table_start_x + column_widths[0] + column_widths[1], row_y, column_widths[2], row_height),
                text=str(player["level"]),
                manager=self.manager
            )

            challenge_button = pygame_gui.elements.UIButton(
                relative_rect=pygame.Rect(table_start_x + sum(column_widths[:-1]), row_y, column_widths[3], row_height),
                text='Challenge',
                manager=self.manager
            )

    def handle_events(self, event):
        # Handle challenge button events
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.back_button:
                    self.on_back()
                # Check which player button was pressed

class HomePage:
    def __init__(self, width, height, manager, on_levels, on_players):
        self.width, self.height = width, height
        self.manager = manager
        self.on_levels = on_levels
        self.on_players = on_players

        self.create_home_page()

    def create_home_page(self):
        # Create buttons
        self.create_buttons()

        # Create character visual and properties display
        self.create_character_visual()

    def create_buttons(self):
        # Example button: Start with Machine
        self.start_with_machine = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(20, 50, 200, 40),
            text='Start with Machine',
            manager=self.manager
        )

        self.start_with_player = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(20, 100, 200, 40),
            text='Start with Player',
            manager=self.manager
        )

        # More buttons can be added similarly...

    def create_character_visual(self):
        # Placeholder for character visual and properties
        # You might want to use an image or draw something to represent the character
        self.character_visual_rect = pygame.Rect(300, 50, 200, 200)
        self.character_properties_label = pygame_gui.elements.UILabel(
            relative_rect=pygame.Rect(300, 260, 200, 100),
            text="Character Properties",
            manager=self.manager
        )

        # Add more elements for properties and upgrades...

    def handle_events(self, event):
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.start_with_machine:
                    print("Start with Machine button pressed")
                    self.on_levels()

                elif event.ui_element == self.start_with_player:
                    # Transition to PlayersPage
                    print("Start with Player button pressed")
                    self.on_players()

class LoginPage:
    def __init__(self, width, height, manager, on_login_success):
        pygame.display.set_caption('Login Page')
        self.width, self.height = width, height
        self.manager = manager
        self.on_login_success = on_login_success

        self.create_login_form()

    def create_login_form(self):
        title_rect = pygame.Rect((self.width - 300) // 2, 50, 300, 30)
        title_label = pygame_gui.elements.UILabel(relative_rect=title_rect, text="Login Form", manager=self.manager)

        description_rect = pygame.Rect((self.width - 400) // 2, 100, 400, 50)
        description_label = pygame_gui.elements.UILabel(relative_rect=description_rect, text="Please enter your username and password:", manager=self.manager)

        username_rect = pygame.Rect((self.width - 300) // 2, 180, 300, 30)
        password_rect = pygame.Rect((self.width - 300) // 2, 230, 300, 30)

        username_rect_label = pygame.Rect((self.width - 800) // 2, 180, 300, 30)
        password_rect_label = pygame.Rect((self.width - 800) // 2, 230, 300, 30)

        self.username_label = pygame_gui.elements.UILabel(relative_rect=username_rect_label, text="Username:", manager=self.manager)
        self.password_label = pygame_gui.elements.UILabel(relative_rect=password_rect_label, text="Password:", manager=self.manager)

        self.username_input = pygame_gui.elements.UITextEntryLine(relative_rect=username_rect, manager=self.manager)
        self.password_input = pygame_gui.elements.UITextEntryLine(relative_rect=password_rect, manager=self.manager)

        submit_button_rect = pygame.Rect((self.width - 300) // 2, 280, 150, 40)
        self.submit_button = pygame_gui.elements.UIButton(relative_rect=submit_button_rect, text="Submit", manager=self.manager)
        
        cancel_button_rect = pygame.Rect((self.width) // 2, 280, 150, 40)
        self.cancel_button = pygame_gui.elements.UIButton(relative_rect=cancel_button_rect,text="Cancel",manager=self.manager)

    def submit_action(self):
        username = self.username_input.get_text()
        password = self.password_input.get_text()
        print(f"Submitting login with username: {username}, password: {password}")
        is_login_successful = True  # Replace with actual login verification logic
        if is_login_successful:
            print(f"Login successful for username: {username}")
            self.on_login_success()
        else:
            print("Login failed")

    def cancel_action(self, menu_game):
        self.manager.clear_and_reset()
        menu_game.create_menu_buttons()

    def handle_event_focus(self, event):
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_TAB:
                if self.username_input.is_focused:
                    self.username_input.unfocus()
                    self.password_input.focus()

    def handle_event_button(self, event, menu_game):
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.submit_button:
                    self.submit_action()
                elif event.ui_element == self.cancel_button:
                    self.cancel_action(menu_game)


    def handle_events(self, event, menu_game):
        self.handle_event_focus(event)
        self.handle_event_button(event, menu_game)
        
class RegisterPage:
    def __init__(self, width, height, manager):
        pygame.display.set_caption('Register Page')
        self.width, self.height = width, height
        self.manager = manager

        self.create_register_form()

    def create_register_form(self):
        title_rect = pygame.Rect((self.width - 300) // 2, 50, 300, 30)
        title_label = pygame_gui.elements.UILabel(relative_rect=title_rect, text="Register Form", manager=self.manager)

        description_rect = pygame.Rect((self.width - 400) // 2, 100, 400, 50)
        description_label = pygame_gui.elements.UILabel(relative_rect=description_rect, text="Please enter your username and password:", manager=self.manager)

        username_rect = pygame.Rect((self.width - 300) // 2, 180, 300, 30)
        password_rect = pygame.Rect((self.width - 300) // 2, 230, 300, 30)
        confirm_password_rect = pygame.Rect((self.width - 300) // 2, 280, 300, 30)

        username_rect_label = pygame.Rect((self.width - 800) // 2, 180, 300, 30)
        password_rect_label = pygame.Rect((self.width - 800) // 2, 230, 300, 30)
        confirm_password_rect_label = pygame.Rect((self.width - 800) // 2, 280, 300, 30)

        # label for input
        self.username_label = pygame_gui.elements.UILabel(relative_rect=username_rect_label, text="Username:", manager=self.manager)
        self.password_label = pygame_gui.elements.UILabel(relative_rect=password_rect_label, text="Password:", manager=self.manager)
        self.confirm_password_label = pygame_gui.elements.UILabel(relative_rect=confirm_password_rect_label, text="Confirm Password:", manager=self.manager)

        self.username_input = pygame_gui.elements.UITextEntryLine(relative_rect=username_rect, manager=self.manager)
        self.password_input = pygame_gui.elements.UITextEntryLine(relative_rect=password_rect, manager=self.manager)
        self.confirm_password_input = pygame_gui.elements.UITextEntryLine(relative_rect=confirm_password_rect, manager=self.manager)

        submit_button_rect = pygame.Rect((self.width - 300) // 2, 330, 150, 40)
        self.submit_button = pygame_gui.elements.UIButton(relative_rect=submit_button_rect, text="Submit", manager=self.manager)

        cancel_button_rect = pygame.Rect((self.width) // 2, 330, 150, 40)
        self.cancel_button = pygame_gui.elements.UIButton(relative_rect=cancel_button_rect,text="Cancel",manager=self.manager)

    def submit_action(self):
        username = self.username_input.get_text()
        password = self.password_input.get_text()
        confirm_password = self.confirm_password_input.get_text()
        print(f"Submitting register with username: {username}, password: {password}, confirm_password: {confirm_password}")

    def cancel_action(self, menu_game):
        self.manager.clear_and_reset()
        menu_game.create_menu_buttons()

    def handle_event_focus(self, event):
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_TAB:
                if self.username_input.is_focused:
                    self.username_input.unfocus()
                    self.password_input.focus()
                elif self.password_input.is_focused:
                    self.password_input.unfocus()
                    self.confirm_password_input.focus()

    def handle_event_button(self, event, menu_game):
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.submit_button:
                    self.submit_action()
                elif event.ui_element == self.cancel_button:
                    self.cancel_action(menu_game)

    def handle_events(self, event, menu_game):
        self.handle_event_focus(event)
        self.handle_event_button(event, menu_game)

class MenuGame:
    def __init__(self, manager):
        pygame.display.set_caption('Menu Game')
        self.width, self.height = 800, 600

        self.manager = manager

        self.menu_options = ['LOGIN', 'REGISTER', 'SETTING', 'EXIT']
        self.current_option = None

        self.create_menu_buttons()

    def create_menu_buttons(self):
        button_width, button_height = 200, 50
        button_start_y = 200
        space_between_buttons = 20

        # Create buttons for each option
        for option in self.menu_options:
            button_rect = pygame.Rect(
                (self.width - button_width) // 2,
                button_start_y,
                button_width,
                button_height
            )

            button = pygame_gui.elements.UIButton(
                relative_rect=button_rect,
                text=option,
                manager=self.manager
            )

            button_start_y += button_height + space_between_buttons

            # setattr example: setattr(x, 'foobar', 123) is equivalent to x.foobar = 123
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

class Game:
    def __init__(self):
        pygame.init()

        self.width, self.height = 800, 600
        # screen is the surface representing the window
        self.screen = pygame.display.set_mode((self.width, self.height))
        pygame.display.set_caption('Menu Game')

        # manager is the UI manager
        self.manager = pygame_gui.UIManager((self.width, self.height))

        self.reset_page()

        self.menu_game = MenuGame(self.manager)
        

    def reset_page(self):
        self.menu_game = None
        self.home_page = None
        self.levels_page = None
        self.players_page = None
        self.login_page = None
        self.register_page = None

    def before_change_page(self):
        self.manager.clear_and_reset()
        self.reset_page()
    
    def show_home_page(self):
        self.before_change_page()
        self.home_page = HomePage(self.width, self.height, self.manager, self.show_levels_page, self.show_players_page)

    def show_levels_page(self):
        self.before_change_page()
        self.levels_page = LevelsPage(self.width, self.height, self.manager, self.show_home_page)

    def show_players_page(self):
        self.before_change_page()
        self.players_page = PlayersPage(self.width, self.height, self.manager, self.show_home_page)

    def show_register_page(self):
        self.before_change_page()
        self.register_page = RegisterPage(self.width, self.height, self.manager)
    
    def show_login_page(self):
        self.before_change_page()
        self.login_page = LoginPage(self.width, self.height, self.manager, self.show_home_page)

    def run(self):
        clock = pygame.time.Clock()
        is_running = True

        while is_running:
            time_delta = clock.tick(60) / 1000.0

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    is_running = False
                self.manager.process_events(event)
                if (self.menu_game):
                    self.menu_game.handle_events(event)
                elif (self.login_page):
                    self.login_page.handle_events(event, self.menu_game)
                elif (self.register_page):
                    self.register_page.handle_events(event, self.menu_game)
                elif (self.home_page):
                    self.home_page.handle_events(event)
                elif (self.levels_page):
                    self.levels_page.handle_events(event)
                elif (self.players_page):
                    self.players_page.handle_events(event)


            self.manager.update(time_delta)

            self.screen.fill((255, 255, 255))

            self.manager.draw_ui(self.screen)

            pygame.display.flip()

            if self.menu_game:
                if self.menu_game.current_option:
                    # Redirect to the login page
                    if self.menu_game.current_option == 'LOGIN':
                        self.menu_game.current_option = None
                        self.show_login_page()
                    # Redirect to the register page
                    elif self.menu_game.current_option == 'REGISTER':
                        self.menu_game.current_option = None
                        self.show_register_page()
                    else:
                        # Perform action based on the selected option
                        getattr(self.menu_game, f'{self.menu_game.current_option.lower()}_action')()
                        self.menu_game.current_option = None

if __name__ == "__main__":
    game = Game()
    game.run()