import pygame
from socket import *
from helper import *
import random

beagley_ai_ip = '192.168.7.2'
beagley_ai_port = 12345

server_address =(beagley_ai_ip, beagley_ai_port)
client_socket = socket(AF_INET, SOCK_DGRAM)

pygame.init() 

SCREEN_WIDTH = 240 
SCREEN_HEIGHT = 240 

SCREEN_MID = SCREEN_WIDTH/2

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("PONG")
paddle_size = (7.5, 50.0)
ball_radius = 21
background = pygame.image.load("./assets/background.jpg")
player1 = pygame.image.load("./assets/paddle.png")
player1 = pygame.transform.scale(player1, (paddle_size[0], paddle_size[1]))
player2= pygame.image.load("./assets/paddle.png")
player2 = pygame.transform.scale(player2, (paddle_size[0], paddle_size[1]))
ball = pygame.image.load("./assets/ball.png")
ball = pygame.transform.scale(ball, (ball_radius, ball_radius))


running = True

# to keep track of which player initially (switches between players same controller)
player_turn = True

while running:
    screen.fill((0,0,0))
    # switches players based on which side the ball is on

    client_socket.sendto("requesting message".encode(), server_address)

    message, server_address = client_socket.recvfrom(2048)

    message = message.decode()
    if message == "quit": 
        running = False

    positions = message.split(",")

    screen.blit(background, (0, 0))
    screen.blit(player1, (float(positions[0]), float(positions[1])))
    screen.blit(player2, (float(positions[4]), float(positions[5])))
    screen.blit(ball, (float(positions[2]), float(positions[3])))


    for event in pygame.event.get(): 
        
        if event.type == pygame.QUIT: 
            client_socket.sendto("stop".encode(), server_address)
            running = False


    ##TODO must update since it sends to client continuously
    ## keyboard controls player2!
    keys = pygame.key.get_pressed()
    if keys[pygame.K_UP]: 
        client_socket.sendto("up".encode(), server_address)
    if keys[pygame.K_DOWN]: 
        client_socket.sendto("down".encode(), server_address)


    pygame.display.update()

pygame.quit()
# client_socket.close()