import threading
import time

import pygame
from sys import exit

import pygame_gui
from GameState import GameState
from SocketP2P import SocketServer
from Player import Player
from constant import *

MACHINE_ATTACK_EVENT = pygame.USEREVENT + 1

class Game_Play_Machine:
    # def __init__(self, screen, port_random, my_turn, game_state, port_target = None, socket_server = None):
    def __init__(self, screen, manager, socket, my_turn, game_state: GameState, show_home_page, update_history, level_machine = 1):
        self.count = 1
        self.screen = screen
        self.manager = manager
        self.socket = socket
        self.my_turn = my_turn
        self.game_state = game_state
        self.update_history = update_history
        self.damage = 0
        self.is_game_over = False
        self.show_home_page = show_home_page

        self.indicator_visible = self.game_state.me.position == 'left'

        name_machine = "Machine" + str(level_machine)
        self.game_state.set_you(name_machine, 'right')
        self.level_machine = level_machine
        self.damage_machine = 10 * level_machine

        self.indicator_moving = True
        self.indicator_position = 50  # Vị trí bắt đầu của indicator
        self.indicator_max = 550  # Vị trí kết thúc của indicator
        self.indicator_min = 50  # Vị trí bắt đầu của indicator
        self.indicator_speed = 5 * level_machine  # Tốc độ di chuyển của indicator
        self.indicator_direction = 1  # 1 nghĩa là di chuyển sang phải, -1 là sang trái
        track_start = 50
        track_end = 600
        section_width = 50 
        levels = ["weak", "medium", "strong", "medium", "weak", "medium", "strong", "medium", "weak", "strong", "weak"] 
        self.DAMAGE_RANGES = self.generate_damage_ranges(track_start, track_end, section_width, levels)

        if my_turn:
            print("Connect to target")

        pygame.init()

        pygame.display.set_caption("Slap Combo")

        # Load background and text surface
        self.background = pygame.image.load(BACKGROUND_IMG).convert_alpha()
        # self.text_surface = pygame.font.Font(None, 50).render("My game!", False, 'Green')

        # Initialize sprite groups
        self.player_left = pygame.sprite.GroupSingle()
        self.player_right = pygame.sprite.GroupSingle()

        # Initialize left player
        self.player_left_ins = Player(LINK_SHINOBI_LEFT, FLOOR_PLAYER_LEFT_WIDTH, FLOOR_PLAYER_LEFT_HEIGHT, 0.15, LINK_SLASH, SLASH_INDEX, 0.2, 'left')
        self.player_left.add(self.player_left_ins)
        # Initialize a number in above of head player
        # self.render_number(self.game_state.me.hp)

        # Initialize right player
        self.player_right_ins = Player(LINK_SHINOBI_LEFT, FLOOR_PLAYER_RIGHT_WIDTH, FLOOR_PLAYER_RIGHT_HEIGHT, 0.15, LINK_SLASH, SLASH_INDEX, 0.2, 'right')
        self.player_right.add(self.player_right_ins)

        self.quit_button = pygame_gui.elements.UIButton(
            relative_rect=pygame.Rect(SCREEN_WIDTH // 2 - 75, SCREEN_HEIGHT // 2 + 100, 150, 50),
            text='Back to Home',
            manager=self.manager,
        )
        self.quit_button.hide()

    def draw_name(self):
        font = pygame.font.Font(None, 36) 
        if self.game_state.me.position == 'left':
            name = self.game_state.me.name
            name_surface = font.render(name, True, (0, 0, 255)) 
            self.screen.blit(name_surface, (self.player_left_ins.rect.x + self.player_left_ins.rect.width // 2 - name_surface.get_width() // 2, self.player_left_ins.rect.y - name_surface.get_height() - 10))
            name = self.game_state.you.name
            name_surface = font.render(name, True, (255, 0, 0)) 
            self.screen.blit(name_surface, (self.player_right_ins.rect.x + self.player_right_ins.rect.width // 2 - name_surface.get_width() // 2, self.player_right_ins.rect.y - name_surface.get_height() - 10))
        else:
            name = self.game_state.you.name
            name_surface = font.render(name, True, (0, 0, 255)) 
            self.screen.blit(name_surface, (self.player_left_ins.rect.x + self.player_left_ins.rect.width // 2 - name_surface.get_width() // 2, self.player_left_ins.rect.y - name_surface.get_height() - 10))
            name = self.game_state.me.name
            name_surface = font.render(name, True, (255, 0, 0))
            self.screen.blit(name_surface, (self.player_right_ins.rect.x + self.player_right_ins.rect.width // 2 - name_surface.get_width() // 2, self.player_right_ins.rect.y - name_surface.get_height() - 10))
    
    def render_number(self, number, num_pos):
        font = pygame.font.Font(None, 36) 
        num_surface = font.render(str(number), True, (255, 0, 0)) 
        self.screen.blit(num_surface, num_pos)

    def generate_damage_ranges(self, start, end, section_width, levels):
        ranges = []
        current = start

        # Loop through the track and assign levels in order
        level_index = 0  # Start with the first level
        while current + section_width <= end:
            # Calculate the end of this section
            section_end = current + section_width

            # Append the range with the current level
            ranges.append((current, section_end, levels[level_index]))

            # Move to the next section and cycle through the levels
            current = section_end
            level_index = (level_index + 1) % len(levels)

        return ranges

    def handle_left_attack(self, event):
        # mess = "L:" + str(self.damage)
        # self.send_attack_left(mess)
        self.player_left_ins.change_animation('Attack_1')
        self.hidden_indicator()
        pygame.time.set_timer(MACHINE_ATTACK_EVENT, 3000, 1)

    def handle_right_attack_machine(self):
        # set damage for machine
        self.damage = self.damage_machine
        self.player_right_ins.change_animation('Attack_1')
        self.display_indicator()

    def check_game_over(self):
        if self.game_state.me.hp <= 0 or self.game_state.you.hp <= 0:
            self.hidden_indicator()
            self.is_game_over = True
            print("Game over")
            self.quit_button.show()
            return True
        return False

    def update_your_hp(self):
        self.game_state.you.hp = self.game_state.you.hp - self.damage
        self.check_game_over()

    def update_my_hp(self):
        self.game_state.me.hp = self.game_state.me.hp - self.damage
        self.check_game_over()

    def calculate_damage(self):
        for start, end, level in self.DAMAGE_RANGES:
            if start <= self.indicator_position <= end:
                if level == "strong":
                    self.damage = 30
                elif level == "medium":
                    self.damage = 20
                else:
                    self.damage = 10
                break

    def hidden_indicator(self):
        self.indicator_visible = False
        self.indicator_moving = False

    def display_indicator(self):
        self.indicator_visible = True
        self.indicator_moving = True

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.socket.logout()
                pygame.quit()
                exit()
            self.manager.process_events(event)
            if event.type == pygame.USEREVENT:
                if event.user_type == pygame_gui.UI_BUTTON_PRESSED:
                    if event.ui_element == self.quit_button:
                        print("Back to home button pressed")
                        self.running = False
                        if self.game_state.me.hp <= 0:
                            print("You lose")
                            self.game_state.update_winner_loser(self.game_state.you.name, self.game_state.me.name, time.time())
                        else:
                            print("You win")
                            self.game_state.update_winner_loser(self.game_state.me.name, self.game_state.you.name, time.time())
                        self.update_history()
                        self.show_home_page()
                        break
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_SPACE and self.indicator_visible:
                    # Tính toán lực đánh dựa trên vị trí của indicator
                    self.indicator_moving = not self.indicator_moving
                    if self.indicator_moving:
                        print("Indicator resumed.")
                    else:
                        print("Indicator stopped.")
                        self.calculate_damage()
                    if self.game_state.me.position == 'left':
                        self.handle_left_attack(event)
            elif event.type == MACHINE_ATTACK_EVENT:  # Xử lý sự kiện MACHINE_ATTACK_EVENT
                self.handle_right_attack_machine()

    def update_indicator_position(self):
        # Cập nhật vị trí của indicator
        if self.indicator_moving:
            self.indicator_position += self.indicator_direction * self.indicator_speed
            if self.indicator_position > self.indicator_max or self.indicator_position < self.indicator_min:
                self.indicator_direction *= -1

    def update(self):
        self.clock = pygame.time.Clock()
        self.player_left_ins.update()
        self.player_right_ins.update()

        # update position indicator
        self.update_indicator_position()

        time_delta = self.clock.tick(60) / 1000.0
        self.manager.update(time_delta)
        self.manager.draw_ui(self.screen)
        pygame.display.flip()

    def draw_line_slash(self, player, direction):
        slash = player.slash.sprite
        if direction == 'right':
            rect = slash.image.get_rect(topright = [slash.slash_start_x, slash.slash_start_y])
        else:
            rect = slash.image.get_rect(topleft = [slash.slash_start_x, slash.slash_start_y])
        border_color = (255, 0, 0)
        pygame.draw.rect(self.screen, border_color, rect, 2)

    def draw_background(self):
        # Vẽ nền màn hình
        background = pygame.transform.scale(self.background, (SCREEN_WIDTH, SCREEN_HEIGHT))
        self.screen.blit(background, (0, 0))

    def draw(self):
        # Draw background fit to screen
        self.draw_background()
        self.draw_name()

        # load GAMEOVER_IMG png in center of window
        if self.is_game_over:
            # gameover_img = pygame.image.load(GAMEOVER_IMG).convert_alpha()
            self.image_over_game = WIN_IMG
            if self.game_state.me.hp <= 0:
                self.image_over_game = LOSE_IMG
            gameover_img = pygame.image.load(self.image_over_game).convert_alpha()
            gameover_img = pygame.transform.scale(gameover_img, (SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2))
            self.screen.blit(gameover_img, (SCREEN_WIDTH // 2 - gameover_img.get_width() // 2, SCREEN_HEIGHT // 2 - gameover_img.get_height() // 2))
            # # create button to back home page

        # Draw text surface
        # self.screen.blit(self.text_surface, (500, 100))
        # Draw player left
        self.player_left.draw(self.screen)
        # Draw player right
        self.player_right.draw(self.screen)

        # Enhanced Indicator drawing
        indicator_y = 30  # Y-position for the indicator
        indicator_height = 10  # Height of the indicator

        
        if self.indicator_visible:
            # Draw the gradient track
            for start, end, level in self.DAMAGE_RANGES:
                start_color = LEVEL_COLORS[level]  # Get start color from LEVEL_COLORS
                # end color = the next level color
                end_color = LEVEL_COLORS[self.DAMAGE_RANGES[(self.DAMAGE_RANGES.index((start, end, level)) + 1) % len(self.DAMAGE_RANGES)][2]]
                # Draw gradient for this range
                for x in range(start, end):
                    interp = (x - start) / (end - start)
                    color = [int(interp * end_color[i] + (1 - interp) * start_color[i]) for i in range(3)]
                    pygame.draw.line(self.screen, color, (x, indicator_y), (x, indicator_y + indicator_height), 1)

            # Moving indicator with image or improved graphics
            # Load an image or create a fancy shape for your moving indicator
            # For simplicity, here's an enhanced rectangle with border
            pygame.draw.rect(self.screen, (255, 255, 0), (self.indicator_position, indicator_y, 10, indicator_height))  # Yellow moving indicator
            pygame.draw.rect(self.screen, (0, 0, 0), (self.indicator_position, indicator_y, 10, indicator_height), 2)  # Black border

            # Adding text or icons at specific positions (Weak, Strong, etc.)
            font = pygame.font.Font(None, 24)
            self.screen.blit(font.render("Weak", True, (80, 80, 80)), (self.indicator_min, indicator_y - 20))
            self.screen.blit(font.render("Strong", True, (80, 80, 80)), (self.indicator_max - 50, indicator_y - 20))

        if self.game_state.me.position == 'left':
            right_hp = self.game_state.you.hp
            left_hp = self.game_state.me.hp
        else:  
            right_hp = self.game_state.me.hp
            left_hp = self.game_state.you.hp

        self.player_right_ins.draw_health_bar(right_hp, self.screen)
        self.player_left_ins.draw_health_bar(left_hp, self.screen)

        # Check if the player_left has a slash before trying to draw it
        if self.player_left_ins.slash:
            self.player_left_ins.slash.draw(self.screen)
            # self.draw_line_slash(self.player_left_ins, 'left')
        if self.player_right_ins.slash:
            self.player_right_ins.slash.draw(self.screen)
            # self.draw_line_slash(self.player_right_ins, 'right')


    def check_collision(self):
        if self.player_left_ins.slash:
            slash_hitbox = self.player_left_ins.slash.sprite.rect
            if self.player_right_ins.hitbox.colliderect(slash_hitbox):
                # print("check collision", self.count)
                # self.count = self.count + 1
                # print("Collision")
                self.player_right_ins.change_animation('Hurt')
                if self.game_state.me.position == 'right':
                    self.update_my_hp()
                else:
                    self.update_your_hp()
                self.damage = 0
                # self.player_right_ins.create_slash()
        if self.player_right_ins.slash:
            slash_hitbox = self.player_right_ins.slash.sprite.rect
            if self.player_left_ins.hitbox.colliderect(slash_hitbox):
                # print("check collision", self.count)
                # self.count = self.count + 1
                # print("Collision")
                self.player_left_ins.change_animation('Hurt')
                if self.game_state.me.position == 'left':
                    self.update_my_hp()
                else:
                    self.update_your_hp()
                self.damage = 0

    def run(self):
        self.running = True
        while self.running:
            self.handle_events()
            self.update()
            self.draw()
            self.check_collision()
            self.clock.tick(FPS)