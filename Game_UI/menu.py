import time
import pygame
import pygame_gui
import sys
import threading

from Game_Play import Game_Play
from Game_Play_Machine import Game_Play_Machine
from Player import Player
from SocketClient import SocketClient as Socket
from SocketP2P import SocketServer
from MachineLevelsPage import MachineLevelsPage
from PlayersListPage import PlayersListPage
from LoginPage import LoginPage
from RegisterPage import RegisterPage
from HomePage import HomePage
from MenuGame import MenuGame
from GameState import GameState

from constant import *

clock = pygame.time.Clock()

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

class Game:
    def __init__(self):
        pygame.init()

        print("Initializing socket")
        self.socket = Socket()
        self.socket.connect()
        self.socket_server = None
        self.socket_game = None
        self.home_page = None
        self.game_play_machine = None

        self.width, self.height = 1080, 600
        # screen is the surface representing the window
        self.screen = pygame.display.set_mode((self.width, self.height))
        pygame.display.set_caption('Menu Game')

        # manager is the UI manager
        self.manager = pygame_gui.UIManager((self.width, self.height), 'theme/default.json')
        self.manager.get_theme().load_theme('theme/label.json')
        self.manager.get_theme().load_theme('theme/button.json')
        self.manager.get_theme().load_theme('theme/text_entry_line.json')
        self.manager.get_theme().load_theme('theme/panel.json')

        self.reset_page()

        self.menu_game = MenuGame(self.screen, self.manager, self.socket)

        # game state
        self.game_state = GameState()
        self.set_background(LOGIN_IMG)
        
    # ===============================SET BACKGROUND==========================
    def set_background(self, background_link):
        background = pygame.image.load(background_link)
        self.background = pygame.transform.scale(background, (self.width, self.height))

    def draw_background(self):
        self.screen.blit(self.background, (0, 0))

    # ===============================HISTORY GAME==========================
    def update_history(self):
        self.socket.add_history(self.socket.username, self.game_state.winner, self.game_state.loser, self.game_state.time)

    def get_history(self):
        return self.socket.get_history(self.socket.username)

    # ===============================SOCKET==========================
    def player_not_available(self):
        self.socket.update_state("buzy")

    def player_available(self):
        self.socket.update_state("available")

    # ===============================CHANGE SCREEN==========================
    def update_special(self):
        time_delta = clock.tick(60) / 1000.0
        self.manager.update(time_delta)

        self.screen.fill((255, 255, 255))

        self.draw_background()  # Draw the background before drawing UI

        if (self.home_page):
            self.home_page.update()

        self.manager.draw_ui(self.screen)

        pygame.display.flip()

    def reset_page(self):
        self.menu_game = None
        self.home_page = None
        self.levels_page = None
        self.players_page = None
        self.login_page = None
        self.register_page = None
        self.game_play = None
        self.game_play_machine = None

    def before_change_page(self):
        # self.screen = pygame.display.set_mode((self.width, self.height))
        self.manager.clear_and_reset()
        self.reset_page()

    def show_menu_game(self):
        self.before_change_page()
        self.menu_game = MenuGame(self.screen, self.manager, self.socket)

    # def show_home_page(self, socket_server: SocketServer = None):
    def show_home_page(self):
        if not self.socket_server:
            self.socket_server = SocketServer(self.socket.port_random)
        self.before_change_page()
        self.home_page = HomePage(self.screen, self.width, self.height, self.manager, self.show_levels_page, self.show_players_page, self.show_menu_game, self.socket, self.show_game_play, self.socket_server, self.game_state, self.player_available, self.get_history)
        
    def show_levels_page(self):
        self.before_change_page()
        self.levels_page = MachineLevelsPage(self.width, self.height, self.manager, self.show_home_page, self.show_game_play_machine, self.player_not_available)

    def show_players_page(self):
        self.before_change_page()
        self.players_page = PlayersListPage(self.screen, self.width, self.height, self.manager, self.show_home_page, self.socket, self.show_game_play, self.socket_server, self.game_state, self.player_not_available)

    def show_register_page(self):
        self.before_change_page()
        self.register_page = RegisterPage(self.width, self.height, self.manager, self.show_menu_game, self.show_login_page, self.socket)
    
    def show_login_page(self):
        self.before_change_page()
        self.login_page = LoginPage(self.screen, self.width, self.height, self.manager, self.show_menu_game, self.show_home_page, self.socket, self.game_state)

    def show_game_play(self, port_random, my_turn, port_target = None):
        self.before_change_page()
        self.update_special()
        self.game_play = Game_Play(self.screen, self.manager, self.socket, port_random, my_turn, self.game_state, self.show_home_page, self.update_history, port_target, self.socket_server)
        self.game_play.run()

    def show_game_play_machine(self, level_machine):
        self.before_change_page()
        self.update_special()
        self.game_state.set_me(self.socket.username, 'left')
        self.game_play_machine = Game_Play_Machine(self.screen, self.manager, self.socket, True, self.game_state, self.show_home_page, self.update_history, level_machine)
        self.game_play_machine.run()

    def run(self):
        is_running = True

        while is_running:
            time_delta = clock.tick(60) / 1000.0

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.socket.logout()
                    is_running = False
                self.manager.process_events(event)
                if (self.menu_game):
                    self.menu_game.handle_events(event)
                elif (self.login_page):
                    self.login_page.handle_events(event)
                elif (self.register_page):
                    self.register_page.handle_events(event)
                elif (self.home_page):
                    self.home_page.handle_events(event)
                elif (self.levels_page):
                    self.levels_page.handle_events(event)
                elif (self.players_page):
                    self.players_page.handle_events(event)

            self.manager.update(time_delta)

            self.screen.fill((255, 255, 255))

            self.draw_background()  # Draw the background before drawing UI

            if (self.home_page):
                self.home_page.update()

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
