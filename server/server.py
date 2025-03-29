import pygame
from socket import *
from helper import *
import random

beagley_ai_ip = '192.168.7.2'
beagley_ai_port = 12345

server_address =(beagley_ai_ip, beagley_ai_port)
client_socket = socket(AF_INET, SOCK_DGRAM)

pygame.init() 

SCREEN_WIDTH = 500 
SCREEN_HEIGHT = 450

SCREEN_MID = SCREEN_WIDTH/2

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("PONG")

player1 = player1_x = 10
player1_y = SCREEN_HEIGHT/2-50

player2_x = SCREEN_WIDTH-20
player2_y = SCREEN_HEIGHT/2-50

player1 = pygame.Rect(player1_x, player1_y, 10, 100)
player2 = pygame.Rect(player2_x, player2_y, 10, 100)

# This is the center position of the ball
ball_x = SCREEN_WIDTH/2
ball_y = SCREEN_HEIGHT/2
ball_x_speed, ball_y_speed = random.choice([0.5, -0.5]), random.choice([0.5, -0.5])

speed = 2

running = True

# to keep track of which player initially (switches between players same controller)
player_turn = True

while running:
    screen.fill((0,0,0))
    # switches players based on which side the ball is on
    if (ball_x > SCREEN_MID): 
        player1_turn = False
    else: 
        player1_turn = True

    player1_surface = pygame.Surface((10, 100))
    player1_surface.fill((255, 255, 255))

    player2_surface = pygame.Surface((10, 100))
    player2_surface.fill((255, 255, 255))

    screen.blit(player1_surface, player1.topleft)
    screen.blit(player2_surface, player2.topleft)

    ball = pygame.draw.circle(screen, (255, 0, 0), (ball_x, ball_y), 7)

    client_socket.sendto("requesting message".encode(), server_address)

    message, server_address = client_socket.recvfrom(2048)

    # print(message.decode())

    for event in pygame.event.get(): 
        
        if event.type == pygame.QUIT: 
            # client_socket.sendto("stop".encode(), server_address)
            running = False


    ## Keyboard movement for testing
    keys = pygame.key.get_pressed()
    if player1_turn:
        # Doesn't work, cannot rotate
        if keys[pygame.K_r]:
            #rotate paddle
            player1_surface = pygame.transform.rotate(player1_surface, 25)
            player1 = player1_surface.get_rect(center=player1.center)

        if keys[pygame.K_UP] or (message.decode() == "UP"): 
            player1.y -= speed
        if keys[pygame.K_DOWN] or (message.decode() == "DOWN"): 
            player1.y +=speed
        if keys[pygame.K_LEFT] or (message.decode() == "LEFT"): 
            player1.x -= speed
        if keys[pygame.K_RIGHT] or (message.decode() == "RIGHT"): 
            player1.x +=speed

    else:
        if keys[pygame.K_UP] or (message.decode() == "UP"): 
            player2.y -= speed
        if keys[pygame.K_DOWN] or (message.decode() == "DOWN"): 
            player2.y +=speed
        if keys[pygame.K_LEFT] or (message.decode() == "LEFT"): 
            player2.x -= speed
        if keys[pygame.K_RIGHT] or (message.decode() == "RIGHT"): 
            player2.x +=speed

    if detect_collision(player1.x, player1.y, ball_x, ball_y): 
        ball_x_speed = -ball_x_speed

    if detect_collision(player2.x, player2.y, ball_x, ball_y): 
        ball_x_speed = -ball_x_speed
    
    if ball_x < 0: 
        print("player 2 scored")
        ball_x = SCREEN_MID
        ball_y = SCREEN_HEIGHT/2
        ball_x_speed, ball_y_speed = random.choice([0.5, -0.5]), random.choice([0.5, -0.5])

    if ball_x > SCREEN_WIDTH: 
        print("player 1 scored")
        ball_x = SCREEN_MID
        ball_y = SCREEN_HEIGHT/2
        ball_x_speed, ball_y_speed = random.choice([0.5, -0.5]), random.choice([0.5, -0.5])
    
    if ball_y <= 0: 
        ball_y_speed = 0.5
    if ball_y >= SCREEN_HEIGHT: 
        ball_y_speed = -0.5

    ball_x += ball_x_speed
    ball_y += ball_y_speed

    # Bound checks
    if player1_turn:
        player1.y = player_bounds_y_check(player1.y, SCREEN_HEIGHT, 100)
        player1.x = player_bounds_x_check(player1.x, 0, SCREEN_MID, 10)
    else:
        player2.y = player_bounds_y_check(player2.y, SCREEN_HEIGHT, 100)
        player2.x = player_bounds_x_check(player2.x, SCREEN_MID, SCREEN_WIDTH, 10)
        

    pygame.display.update()

pygame.quit()
client_socket.close()