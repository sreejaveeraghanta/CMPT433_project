import pygame
from socket import *

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
## load all images 
player1 = pygame.image.load("./assets/paddle.png")
player1 = pygame.transform.scale(player1, (paddle_size[0], paddle_size[1]))

player2= pygame.image.load("./assets/paddle.png")
player2 = pygame.transform.scale(player2, (paddle_size[0], paddle_size[1]))

ball = pygame.image.load("./assets/ball.png")
ball = pygame.transform.scale(ball, (ball_radius, ball_radius))

font = pygame.font.SysFont(pygame.font.get_default_font(), 30)

up_message_sent = False
down_message_sent = False
running = True


while running:
    screen.fill((0,0,0))
    client_socket.sendto("ready".encode(), server_address)

    message, server_address = client_socket.recvfrom(2048)

    message = message.decode()
    if message == "quit": 
        running = False

    positions = message.split(",")

    screen.blit(background, (0, 0))
    screen.blit(player1, (float(positions[0]), float(positions[1])))
    screen.blit(player2, (float(positions[4]), float(positions[5])))
    screen.blit(ball, (float(positions[2]), float(positions[3])))

    player1_score = font.render(str(positions[6]), True, (255, 255, 255))
    player2_score = font.render(str(positions[7]), True, (255, 255, 255))
    screen.blit(player1_score, (20, 10))
    screen.blit(player2_score, (SCREEN_WIDTH-40, 10))


    for event in pygame.event.get(): 
        
        if event.type == pygame.QUIT: 
            client_socket.sendto("stop".encode(), server_address)
            running = False

        if event.type == pygame.KEYUP: 
            if event.key == pygame.K_UP: 
                if up_message_sent: 
                    client_socket.sendto("idle".encode(), server_address)
                up_message_sent = False
            if event.key == pygame.K_DOWN: 
                if  down_message_sent: 
                    client_socket.sendto("idle".encode(), server_address)
                down_message_sent = False

    ## keyboard controls player2!
    keys = pygame.key.get_pressed()
    if keys[pygame.K_UP]: 
        if not up_message_sent: 
            client_socket.sendto("up".encode(), server_address)
            up_message_sent = True
    if keys[pygame.K_DOWN]: 
        if not down_message_sent: 
            client_socket.sendto("down".encode(), server_address)
            down_message_sent = True

    pygame.display.update()

pygame.quit()
client_socket.close()