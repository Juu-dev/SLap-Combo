import pygame_gui
import pygame

class PopUp:
    def __init__(self, message, manager, screen_size):
        self.manager = manager
        self.screen_width, self.screen_height = screen_size  # Unpack screen dimensions
        popup_width, popup_height = 300, 200  # Define popup dimensions
        self.accept_button = None
        self.decline_button = None

        # Calculate center position for the popup
        center_x = (self.screen_width - popup_width) // 2
        center_y = (self.screen_height - popup_height) // 2

        self.window = pygame_gui.elements.UIWindow(
            rect=pygame.Rect(center_x, center_y, popup_width, popup_height),
            manager=self.manager,
            window_display_title="Invitation",
            object_id="#invitation_window"
        )

        self.message_label = pygame_gui.elements.UILabel(
            relative_rect=pygame.Rect(10, 10, 280, 50),
            text=message,
            manager=self.manager,
            container=self.window,
            object_id="#invitation_message"
        )

        self.accept_button = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(50, 100, 100, 30),
            text="Accept",
            manager=self.manager,
            container=self.window
        )

        self.decline_button = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(150, 100, 100, 30),
            text="Decline",
            manager=self.manager,
            container=self.window
        )

    def kill(self):
        self.window.kill()
        self.message_label.kill()
        self.accept_button.kill()
        self.decline_button.kill()
