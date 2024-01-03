import random
import threading
import time as Time
import pygame_gui
import pygame
from Player import Player
from SocketClient import SocketClient as Socket
from SocketP2P import SocketServer
from PopUp import PopUp
from constant import *

class StoppableThread(threading.Thread):
    def __init__(self, port, create_popup, socket_server: SocketServer):
        super().__init__()
        self.stop_requested = False
        self.port = port
        self.create_popup = create_popup
        print("===StoppableThread===: ", socket_server)
        if socket_server is None:
            self.socket_server = SocketServer(self.port)
        else:
            self.socket_server = socket_server

    def run(self):
        self.socket_server.connect()
        while not self.stop_requested:
           # Check for new connections
            self.socket_server.check_for_new_connections()
            # Check for incoming messages
            message = self.socket_server.check_for_incoming_messages()
            # if have message then show popup invite class PopUp and wait for accept or decline
            if message:
                print("Message HomePage: ", message)
                # message = "INVITE_FROM:admin_1:port"
                self.socket_server.target_client_port = int(message.split(':')[2])
                self.socket_server.your_name = message.split(':')[1]

                message = f"Invite from {message.split(':')[1]}"
            # Cập nhật trạng thái để xử lý trong vòng lặp chính
                self.create_popup(message)

        print("Thread terminating")

    def stop(self):
        self.stop_requested = True
        # self.socket_server.close_connection()

class HomePage:
    def __init__(self, screen, width, height, manager, on_levels, show_players_page, on_logout, socket: Socket, show_game_play, socket_server: SocketServer, game_state, player_available, get_history):
        self.width, self.height = width, height
        self.manager = manager
        self.on_levels = on_levels
        self.show_players_page = show_players_page
        self.on_logout = on_logout
        self.socket = socket
        self.message = ""
        self.pop_up = None
        self.show_game_play = show_game_play
        self.socket_server = socket_server
        self.game_state = game_state
        self.player_available = player_available
        self.screen = screen
        self.get_history = get_history
        self.history_game = []
        self.data_history = self.get_history()

        self.notification_thread = StoppableThread(self.socket.port_random, self.create_popup, self.socket_server)
        self.notification_thread.start()

        self.avail = True

        self.create_home_page()
        # self.socket.wait_invite()

    def create_popup(self, message):
        screen_size = self.screen.get_size()
        self.pop_up = PopUp(message, self.manager, screen_size)
    
    def create_home_page(self):
        # Create buttons
        self.create_buttons()

        # Create character visual and properties display
        self.create_character_visual()

        self.create_info_player()

        self.create_history()

        self.player_available()

    def create_info_player(self):
        info_panel_rect = pygame.Rect(SCREEN_WIDTH - 300, 20, 250, 250)

        # Create a panel to contain player information
        self.info_panel = pygame_gui.elements.UIPanel(
            relative_rect=info_panel_rect,
            starting_height=1,
            manager=self.manager
        )

        # Create labels for player information
        self.username_label = pygame_gui.elements.UILabel(
            relative_rect=pygame.Rect(10, 10, 180, 20),
            text=f"Username: {self.socket.username}",
            manager=self.manager,
            container=self.info_panel
        )

        self.damage_label = pygame_gui.elements.UILabel(
            relative_rect=pygame.Rect(10, 40, 180, 20),
            text=f"Damage: 200",
            manager=self.manager,
            container=self.info_panel
        )

        self.health_label = pygame_gui.elements.UILabel(
            relative_rect=pygame.Rect(10, 70, 180, 20),
            text=f"Health: 200",
            manager=self.manager,
            container=self.info_panel
        )

        self.score_label = pygame_gui.elements.UILabel(
            relative_rect=pygame.Rect(10, 100, 180, 20),
            text=f"Score: 200",
            manager=self.manager,
            container=self.info_panel
        )

        self.achievements_label = pygame_gui.elements.UILabel(
            relative_rect=pygame.Rect(10, 130, 190, 20),
            text=f"Achievements: Solo King",
            manager=self.manager,
            container=self.info_panel
        )

        self.level_label = pygame_gui.elements.UILabel(
            relative_rect=pygame.Rect(10, 160, 180, 20),
            text=f"Level: 1",
            manager=self.manager,
            container=self.info_panel
        )

    def create_history(self):
        info_panel_rect = pygame.Rect(SCREEN_WIDTH - 300, 300, 250, 300)

        # Create a panel to contain player information
        self.info_panel = pygame_gui.elements.UIPanel(
            relative_rect=info_panel_rect,
            starting_height=1,
            manager=self.manager
        )

        # Create a scrolling container for historical data
        scroll_container_rect = pygame.Rect(10, 10, 230, 280)
        self.history_scroll_container = pygame_gui.elements.UIScrollingContainer(
            relative_rect=scroll_container_rect,
            manager=self.manager,
            container=self.info_panel
        )

        print("===data_history===: ", self.data_history)

        # Format and split your historical data
        if self.data_history == '' or self.data_history == 'NULL' or self.data_history is None:
            return

        data_history = self.data_history.split(';')
        for index, data in enumerate(data_history):
            if data != '':
                history_data = data.split(':')

                time = Time.strftime('%Y-%m-%d %H:%M:%S', Time.localtime(float(history_data[4])))
                time_label = pygame_gui.elements.UILabel(
                    relative_rect=pygame.Rect(0, index * 45 + index * 10, 200, 20),  # Adjust width as needed
                    text=f"Time: {time}",
                    manager=self.manager,
                    container=self.history_scroll_container
                )

                # Create labels for each historical entry
                winner_label = pygame_gui.elements.UILabel(
                    relative_rect=pygame.Rect(0, index * 45 + index * 10 + 15, 200, 20),  # Adjust width as needed
                    text=f"Winner: {history_data[2]}",
                    manager=self.manager,
                    container=self.history_scroll_container
                )

                loser_label = pygame_gui.elements.UILabel(
                    relative_rect=pygame.Rect(0, index * 45 + index * 10 + 30, 200, 20),  # Adjust width as needed
                    text=f"Loser: {history_data[3]}",
                    manager=self.manager,
                    container=self.history_scroll_container
                )

                # Add the horizontal layout to the list of game layouts
        # Calculate the total height needed for data_history
        total_height = len(data_history) * 50

        # Set the scrolling container's height to fit all entries
        self.history_scroll_container.set_scrollable_area_dimensions((300, total_height))

    # def create_history(self):
    #     info_panel_rect = pygame.Rect(SCREEN_WIDTH - 300, 300, 250, 300)

    #     # Create a panel to contain player information
    #     self.info_panel = pygame_gui.elements.UIPanel(
    #         relative_rect=info_panel_rect,
    #         starting_height=1,
    #         manager=self.manager
    #     )

    #     # format data_history: 1:admin_1:Machine10:admin_1:1704219469.5586808;2:admin_1:Machine10:admin_1:1704219658.4801276;
    #     # convert time
    #     time = Time.strftime('%Y-%m-%d %H:%M:%S', Time.localtime(float('1704219469.5586808')))
    #     # split to auth data
    #     data_history = self.data_history.split(';')
    #     for data in data_history:
    #         if data != '':
    #             self.history_game.append(data.split(':'))


    #     # Create labels for player information
    #     self.winner_label = pygame_gui.elements.UILabel(
    #         relative_rect=pygame.Rect(10, 10, 180, 20),
    #         text=f"Winner: {self.history_game[0][2]}",
    #         manager=self.manager,
    #         container=self.info_panel
    #     )

    #     self.loser_label = pygame_gui.elements.UILabel(
    #         relative_rect=pygame.Rect(10, 40, 180, 20),
    #         text=f"Loser: {self.history_game[0][3]}",
    #         manager=self.manager,
    #         container=self.info_panel
    #     )

    #     self.time_label = pygame_gui.elements.UILabel(
    #         relative_rect=pygame.Rect(10, 70, 200, 20),
    #         text=f"Time: {time}",
    #         manager=self.manager,
    #         container=self.info_panel
    #     )

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

        self.logout = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(20, 150, 200, 40),
            text='Logout',
            manager=self.manager
        )
    
    def create_character_visual(self):
        # Placeholder for character visual and properties
        # You might want to use an image or draw something to represent the character
        self.character_visual_rect = pygame.Rect(300, 50, 200, 200)
        self.player_left = pygame.sprite.GroupSingle()
        self.player_left_ins = Player(LINK_SHINOBI_LEFT, PLAYER_HOMEPAGE_WIDTH, PLAYER_HOMEPAGE_HEIGHT, 0.5, LINK_SLASH, SLASH_INDEX, 0.2, 'left', 4)
        self.player_left.add(self.player_left_ins)

        # Add more elements for properties and upgrades...

    def update(self):
        self.player_left.draw(self.screen)
        self.player_left.update()

        action = ['Attack_1', 'Hurt', 'Idle', 'Walk']
        index = random.randint(0, 3)
        self.player_left_ins.change_animation(action[index])

    def handle_events(self, event):
        if event.type == pygame.QUIT:
            self.notification_thread.stop()
        if event.type == pygame.USEREVENT:
            if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                if event.ui_element == self.start_with_machine:
                    self.notification_thread.stop()
                    print("Start with Machine button pressed")
                    self.avail = False
                    self.on_levels()

                elif event.ui_element == self.start_with_player:
                    self.notification_thread.stop()
                    # Transition to PlayersListPage
                    print("Start with Player button pressed")
                    self.avail = False
                    self.show_players_page()

                elif event.ui_element == self.logout:
                    self.notification_thread.stop()
                    # Transition to MenuGame
                    print("Logout button pressed")
                    self.avail = False
                    self.socket.logout()
                    self.on_logout()

                elif event.ui_element == self.pop_up.accept_button:
                    self.notification_thread.socket_server.send_message("OK")
                    # remove popup
                    self.pop_up = None
                    # show game play
                    self.notification_thread.stop()

                    self.game_state.set_you(self.notification_thread.socket_server.your_name, 'left')
                    self.game_state.update_me('right')
                    self.show_game_play(self.socket.port_random, False, self.notification_thread.socket_server.target_client_port)


                elif event.ui_element == self.pop_up.decline_button:
                    self.notification_thread.socket_server.send_message("NO")
                    # remove popup
                    self.pop_up.kill()
                    self.pop_up = None
