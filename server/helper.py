import math
def player_bounds_y_check(player_y, screen_height, player_height): 
    if player_y < 0: 
        player_y = 0 
    if player_y > screen_height - player_height: 
        player_y = screen_height - player_height
    return player_y

def player_bounds_x_check(player_x, screen_start, screen_end, player_width): 
    if player_x < screen_start: 
        player_x = screen_start 
    if player_x > screen_end - player_width: 
        player_x = screen_end - player_width
    return player_x

def detect_collision(paddle_x, paddle_y, ball_x, ball_y):
    # calculates the distance between player and block
    if paddle_x - 7 <= ball_x <= paddle_x and ball_y in range(paddle_y-7, paddle_y+100 +7):
        return True
    else: 
        return False