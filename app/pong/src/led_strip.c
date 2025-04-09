#include "led_strip.h"
#include "sharedDataLayout.h"

#include <pthread.h>

#define ZERO 0
#define IN_NS_25ms 25000000

void led_strip_init(void);
void led_strip_cleanup(void);

void check_hit(void);
void player_scored_increment(void);
void opponent_scored_increment(void);
void led_strip_stop(void);

static pthread_t ledStripThreadId;

static struct timespec reqDelay = {ZERO, IN_NS_25ms};

static bool stopping = false;
static bool doing_animation = false;
static int player_scored = 0;
static int opponent_scored = 0;

static int animation_count = 0;

static void* led_strip_thread(void* _arg)
{
    (void)_arg;
    while(!stopping){
        if(doing_animation > 0){
            if(player_scored){
                hit_animation();
                player_scored--;
            }
            else if(opponent_scored > 0){
                miss_animation();
                opponent_scored--;
            }

            if(animation_count == 1){
                doing_animation = false;
            }
            animation_count--;
        }
        nanosleep(&reqDelay, (struct timespec *) NULL);
    }
    return NULL;
}

void led_strip_stop(void)
{
    stopping = true;
}

void check_hit(void)
{
    animation_count++;
    doing_animation = true;

    if(animation_count > 1)
    {
        set_cut_animation();

        // wait until animation cuts
        while(animation_count > 1){
            nanosleep(&reqDelay, NULL);
        }
        clear_cut_animation();
    }

}

void player_scored_increment(void)
{
    player_scored++;
}

void opponent_scored_increment(void)
{
    opponent_scored++;
}


void led_strip_init(void)
{
    pthread_create(&ledStripThreadId, NULL, led_strip_thread, NULL);
}


void led_strip_cleanup(void)
{
    led_strip_stop();
    pthread_join(ledStripThreadId, NULL);
}