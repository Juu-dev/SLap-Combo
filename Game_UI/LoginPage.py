import pygame_gui
import pygame
from SocketClient import SocketClient as Socket

text_kwargs = {
    'html_text': '<b>Login Form</b>'  # Sử dụng HTML-like markup để định dạng chữ đỏ và bold
}

class LoginPage:
    def __init__(self, screen, width, height, manager, on_cancel_login, on_login_success, socket: Socket, game_state):
        pygame.display.set_caption('Login Page')
        self.width, self.height = width, height
        self.manager = manager
        self.on_login_success = on_login_success
        self.on_cancel_login = on_cancel_login
        self.game_state = game_state
        self.screen = screen

        self.socket = socket

        self.create_login_form()

    def create_login_form(self):
        title_rect = pygame.Rect((self.width - 300) // 2, 50, 300, 70)
        title_label = pygame_gui.elements.UILabel(relative_rect=title_rect, text="Login Form", manager=self.manager, text_kwargs=text_kwargs, object_id='#title-label')

        # description_rect = pygame.Rect((self.width - 400) // 2, 120, 400, 50)
        # description_label = pygame_gui.elements.UILabel(relative_rect=description_rect, text="Please enter your username and password:", manager=self.manager)

        username_rect = pygame.Rect((self.width - 300) // 2, 180, 300, 40)
        password_rect = pygame.Rect((self.width - 300) // 2, 230, 300, 40)

        username_rect_label = pygame.Rect((self.width - 600) // 2, 180, 150, 40)
        password_rect_label = pygame.Rect((self.width - 600) // 2, 230, 150, 40)

        self.username_label = pygame_gui.elements.UILabel(relative_rect=username_rect_label, text="Username:", manager=self.manager)
        self.password_label = pygame_gui.elements.UILabel(relative_rect=password_rect_label, text="Password:", manager=self.manager)

        self.username_input = pygame_gui.elements.UITextEntryLine(relative_rect=username_rect, manager=self.manager)
        self.password_input = pygame_gui.elements.UITextEntryLine(relative_rect=password_rect, manager=self.manager)

        submit_button_rect = pygame.Rect((self.width - 300) // 2, 320, 150, 40)
        self.submit_button = pygame_gui.elements.UIButton(relative_rect=submit_button_rect, text="Submit", manager=self.manager)
        
        cancel_button_rect = pygame.Rect((self.width) // 2, 320, 150, 40)
        self.cancel_button = pygame_gui.elements.UIButton(relative_rect=cancel_button_rect,text="Cancel",manager=self.manager)

    def submit_action(self):
        username = self.username_input.get_text()
        password = self.password_input.get_text()
        print(f"Submitting login with username: {username}, password: {password}")
        is_login_successful = self.socket.login(username, password)
        
        if is_login_successful == 0:
            self.game_state.set_me(username, 'none')
            print(f"Login successful for username: {username}")
            self.on_login_success()
        else:
            print("Login failed")

    def cancel_action(self):
        self.on_cancel_login()

    def handle_event_button(self, event):
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.submit_button:
                    self.submit_action()
                elif event.ui_element == self.cancel_button:
                    self.cancel_action()


    def handle_events(self, event):
        self.handle_event_button(event)
        