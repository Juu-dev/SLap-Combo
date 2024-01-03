import pygame_gui
import pygame
from SocketClient import SocketClient as Socket
from SocketP2P import SocketServer
from PopUp import PopUp

class PlayersListPage:
    def __init__(self, screen, width, height, manager, show_home_page, socket: Socket, show_game_play, socket_server: SocketServer, game_state, player_not_available):
        self.width, self.height = width, height
        self.manager = manager
        self.show_home_page = show_home_page
        self.socket = socket
        self.screen = screen
        self.pop_up = None
        self.show_game_play = show_game_play
        self.socket_server = socket_server
        self.game_state = game_state
        self.player_not_available = player_not_available
        self.player_ui_elements = []

        self.create_menu()
        self.create_players_list()

    def update_list_player(self):
        self.clear_player_ui_elements()
        self.list_player = self.socket.get_list_user()
        if self.list_player is None or self.list_player == "NULL":
            self.list_player = []
        else:
            self.list_player = self.list_player
            # remove the last character '|'
            self.list_player = self.list_player[:-1]
            # split list_player to array object
            self.list_player = self.list_player.split(';')
            # remove the last : and split each object to array object
            for i in range(len(self.list_player)):
                self.list_player[i] = {"username": self.list_player[i].split(':')[1], "health": 1234, "level": 5}
            
            print(f"list_player: {self.list_player}")

            # remove player with username = self.game_state.me.name
            for i in range(len(self.list_player)):
                if self.list_player[i]['username'] == self.game_state.me.name:
                    self.list_player.pop(i)
                    break

    def clear_player_ui_elements(self):
        # Clear old player UI elements
        for element in self.player_ui_elements:
            element.kill()  # Removes the element from UIManager
        self.player_ui_elements = []  # Reset the list

    def create_menu(self):
        self.back_button = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(20, 20, 100, 40),
            text='Back',
            manager=self.manager
        )

        self.refresh_button = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(150, 20, 100, 40),
            text='Refresh',
            manager=self.manager
        )

    def create_popup(self, message):
        screen_size = self.screen.get_size()
        self.pop_up = PopUp(message, self.manager, screen_size)

    def create_players_list(self):
        # Create a table to display player information
        # Table setup
        table_start_x, table_start_y = 100, 100
        row_height = 50
        column_widths = [100, 100, 50, 150]  # Widths for username, health, level, challenge button

        self.update_list_player()

        for index, player in enumerate(self.list_player):
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

            self.player_ui_elements.extend([username_label, health_label, level_label, challenge_button])

            setattr(self, f'{index}_challenge_button', challenge_button)

        pygame.display.flip()
        self.player_not_available()


    def handle_events(self, event):
        # Handle challenge button events
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.back_button:
                    self.show_home_page()
                elif event.ui_element == self.refresh_button:
                    self.create_players_list()
                # Check which player button was pressed
                for index, player in enumerate(self.list_player):
                    button = getattr(self, f'{index}_challenge_button')
                    if event.ui_element == button:
                        print(f"Challenge button pressed for player: {player['username']}")
                        message = self.socket.send_invite(player['username'])
                        # if message include "OK" then wait for response from player
                        if message and message[0] == "O" and message[1] == "K":
                            print("Da gui loi moi thanh cong", str(message))
                            # messafe: OK:ip_target:port_target
                            ip_target = message.split(':')[1]
                            port_target = int(message.split(':')[2])
                            self.game_state.set_you(player['username'], 'right')
                            self.game_state.update_me('left')
                            self.show_game_play(self.socket.port_random, True, port_target, ip_target)
                        else:
                            print("Challenge failed")
                            # message = self.game_state.you.name + ""
                            self.create_popup(message)
                        return
                if self.pop_up is not None:
                    if event.ui_element == self.pop_up.accept_button or event.ui_element == self.pop_up.decline_button:
                        self.pop_up.kill()
                        self.pop_up = None

