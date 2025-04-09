#ifndef _LED_STRIP_H_
#define _LED_STRIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void led_strip_init(void);
void led_strip_cleanup(void);

void check_hit(void);
void player_scored_increment(void);
void opponent_scored_increment(void);
void led_strip_stop(void);

#ifdef __cplusplus
}
#endif

#endif