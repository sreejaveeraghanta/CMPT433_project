#include "lvgl.h"
#include "src/osal/lv_os.h"
#include "hal/lcd.h"
#include "hal/joystick.h"

#include <stdio.h>
#include <cstring>
#include <unistd.h>  // usleep
#include <pthread.h> // pthread_create
#include <stdlib.h> 
#include <time.h>
#include <sys/wait.h>

static lv_obj_t* s_btn;
static lv_obj_t * s_roller;
static lv_display_t* s_display;
static pthread_t s_tickThread;
static uint8_t* s_buf;
static bool s_isQuit = false;

void *tickThread(void *arg)
{
    (void)arg;
    while(1) {
        lv_tick_inc(5);  // Increment LVGL tick by 5 ms
        usleep(5000);    // Sleep for 5 ms
    }
    return NULL;
}

void flushFrameBufferCallback(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    (void) display;
    (void) area;
    // Get the framebuffer address
    memcpy(LCD_getFrameBufferAddress(), px_map, LCD_FRAME_BUFFER_SIZE);
    LCD_displayFrame();
    
    /* IMPORTANT!!!
     * Inform LVGL that flushing is complete so buffer can be modified again. */
    lv_display_flush_ready(s_display);
}

static void joystickUpEvent()
{
    lv_lock();
    int diff = -1;
    lv_obj_send_event(s_roller, LV_EVENT_VALUE_CHANGED, &diff);
    lv_unlock();
}

static void joystickDownEvent()
{
    lv_lock();
    int diff = +1;
    lv_obj_send_event(s_roller, LV_EVENT_VALUE_CHANGED, &diff);
    lv_unlock();
}

static void joystickPressedEvent()
{
    lv_lock();
    lv_obj_send_event(s_roller, LV_EVENT_CLICKED, NULL);
    lv_unlock();
}

static void rollerClickedHandler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);

    int optionCount = lv_roller_get_option_count(obj);
    if(code == LV_EVENT_VALUE_CHANGED) {
        int diff = *(int*)lv_event_get_param(e);
        int newSelectedId = lv_roller_get_selected(obj) + diff;
        int lastIndex = optionCount - 1;
        if (newSelectedId < 0) newSelectedId = lastIndex;
        else if (newSelectedId > lastIndex) newSelectedId = 0;
        lv_roller_set_selected(obj, newSelectedId, LV_ANIM_ON);

    } else if (code == LV_EVENT_CLICKED) {
        if (lv_roller_get_selected(obj) == optionCount - 1) {
            lv_obj_del(obj);
            lv_refr_now(s_display);
            s_isQuit = true;
            return;
        }

        const char* dir;
        const char* executableName;
        const char* argv0;
        int curId = lv_roller_get_selected(obj);

        // pvrTriangle
        if (curId == 0) {
            dir = "../pvrTriangle";
            executableName = "./pvrTriangle";
            argv0 = "pvrTriangle";

        // pvrGnomeToy
        } else if (curId == 1) {
            dir = "../pvrGnomeToy";
            executableName = "./pvrGnomeToy";
            argv0 = "pvrGnomeToy";

        // breakout
        } else if (curId == 2) {
            dir = "../breakout";
            executableName = "./breakout";
            argv0 = "breakout";
        }
        // Joystick_deinit();
        // LCD_deinit(); // has to before fork
        pid_t pid = fork();

        if (pid < 0) {
            // Fork failed
            perror("fork");
            exit(EXIT_FAILURE);
        }
        // Child process
        if (pid == 0) {
            // Change to the target directory
            if (chdir(dir) != 0) {
                perror("chdir");
                exit(EXIT_FAILURE);
            }
            usleep(50000);
            // Execute the program
            execl(executableName, argv0, (char *)NULL);
            
            // If execl returns, it must have failed
            perror("execl");
            exit(EXIT_FAILURE);
        // Parent process
        } else {
            printf("cleaning up\n");
            // Joystick_deinit();
            // LCD_deinit();
            lv_deinit();
            exit(EXIT_SUCCESS);

            // int status;
            // waitpid(pid, &status, 0);
            // printf("Child process return\n");
            // LCD_init();
            // auto NOP = []{};
            // Joystick_init(NOP, NOP, joystickDownEvent, joystickUpEvent, joystickPressedEvent);
            // uint8_t lcdParam = 0x68;
            // LCD_command(0x36, &lcdParam, 1); // Flip the screen
            // lv_refr_now(s_display);
            // if (WIFEXITED(status)) {
            //     printf("Child exited with status %d.\n", WEXITSTATUS(status));
            // } else {
            //     printf("Child terminated abnormally.\n");
            // }
        }

    }
}

void createRoller(void)
{
    static lv_style_t style_sel;
    lv_style_init(&style_sel);
    lv_style_set_text_font(&style_sel, &lv_font_montserrat_22);
    lv_style_set_border_width(&style_sel, 2);

    s_roller = lv_roller_create(lv_screen_active());
    lv_roller_set_options(s_roller,
                          "pvrTriangle\n"
                          "pvrGnomeToy\n"
                          "breakout\n"
                          "Quit",
                          LV_ROLLER_MODE_INFINITE);
    lv_obj_add_style(s_roller, &style_sel, LV_PART_MAIN);
    lv_roller_set_visible_row_count(s_roller, 3);
    lv_obj_center(s_roller);
    lv_obj_add_event_cb(s_roller, rollerClickedHandler, LV_EVENT_ALL, NULL);
}

int main()
{
    LCD_init();
    uint8_t lcdParam = 0x68;
    LCD_command(0x36, &lcdParam, 1); // Flip the screen

    auto NOP = []{};
    Joystick_init(NOP, NOP, joystickDownEvent, joystickUpEvent, joystickPressedEvent);
    lv_init();

    if (pthread_create(&s_tickThread, NULL, tickThread, NULL) != 0) {
        perror("Failed to create LVGL tick thread");
    }

    s_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_color_format(s_display, LV_COLOR_FORMAT_RGB565);

    s_buf = (uint8_t*)malloc(LCD_FRAME_BUFFER_SIZE);
    lv_display_set_buffers(s_display, s_buf, NULL, LCD_FRAME_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(s_display, flushFrameBufferCallback);


    createRoller();

    while (!s_isQuit) {
        int delay = lv_timer_handler();
        if (delay < 1) delay = 1;
        usleep(delay * 1000);
    }
    Joystick_deinit();
    LCD_deinit();
    lv_deinit();
    return 0;  
}
