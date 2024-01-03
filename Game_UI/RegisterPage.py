import pygame_gui
import pygame
from SocketClient import SocketClient as Socket

class RegisterPage:
    def __init__(self, width, height, manager, on_cancel_register, on_register_success, socket: Socket):
        pygame.display.set_caption('Register Page')
        self.width, self.height = width, height
        self.manager = manager
        self.socket = socket
        self.on_register_success = on_register_success
        self.on_cancel_register = on_cancel_register

        self.create_register_form()

    def create_register_form(self):
        title_rect = pygame.Rect((self.width - 400) // 2, 50, 400, 70)
        title_label = pygame_gui.elements.UILabel(relative_rect=title_rect, text="Register Form", manager=self.manager, object_id='#title-label')

        # description_rect = pygame.Rect((self.width - 400) // 2, 120, 400, 50)
        # description_label = pygame_gui.elements.UILabel(relative_rect=description_rect, text="Please enter your username and password:", manager=self.manager)

        username_rect = pygame.Rect((self.width - 300) // 2, 180, 300, 40)
        password_rect = pygame.Rect((self.width - 300) // 2, 230, 300, 40)
        confirm_password_rect = pygame.Rect((self.width - 300) // 2, 280, 300, 40)

        username_rect_label = pygame.Rect((self.width - 600) // 2, 180, 150, 40)
        password_rect_label = pygame.Rect((self.width - 600) // 2, 230, 150, 40)
        confirm_password_rect_label = pygame.Rect((self.width - 600) // 2, 280, 150, 40)

        # label for input
        self.username_label = pygame_gui.elements.UILabel(relative_rect=username_rect_label, text="Username:", manager=self.manager)
        self.password_label = pygame_gui.elements.UILabel(relative_rect=password_rect_label, text="Password:", manager=self.manager)
        self.confirm_password_label = pygame_gui.elements.UILabel(relative_rect=confirm_password_rect_label, text="Confirm Password:", manager=self.manager)

        self.username_input = pygame_gui.elements.UITextEntryLine(relative_rect=username_rect, manager=self.manager)
        self.password_input = pygame_gui.elements.UITextEntryLine(relative_rect=password_rect, manager=self.manager)
        self.confirm_password_input = pygame_gui.elements.UITextEntryLine(relative_rect=confirm_password_rect, manager=self.manager)

        submit_button_rect = pygame.Rect((self.width - 300) // 2, 370, 150, 40)
        self.submit_button = pygame_gui.elements.UIButton(relative_rect=submit_button_rect, text="Submit", manager=self.manager)

        cancel_button_rect = pygame.Rect((self.width) // 2, 370, 150, 40)
        self.cancel_button = pygame_gui.elements.UIButton(relative_rect=cancel_button_rect,text="Cancel",manager=self.manager)

    def submit_action(self):
        username = self.username_input.get_text()
        password = self.password_input.get_text()
        confirm_password = self.confirm_password_input.get_text()
        print(f"Submitting register with username: {username}, password: {password}, confirm_password: {confirm_password}")
        is_register_successful = self.socket.register(username, password, confirm_password)
        
        if is_register_successful == 0:
            print(f"Register successful for username: {username}")
            self.on_register_success()
        else:
            print("Register failed")

    def cancel_action(self):
        self.on_cancel_register()

    def handle_event_focus(self, event):
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_TAB:
                if self.username_input.is_focused:
                    self.username_input.unfocus()
                    self.password_input.focus()
                elif self.password_input.is_focused:
                    self.password_input.unfocus()
                    self.confirm_password_input.focus()

    def handle_event_button(self, event):
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.submit_button:
                    self.submit_action()
                elif event.ui_element == self.cancel_button:
                    self.cancel_action()

    def handle_events(self, event):
        self.handle_event_focus(event)
        self.handle_event_button(event)
