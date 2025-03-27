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

static lv_obj_t *    s_roller;
static lv_display_t* s_display;
static pthread_t     s_tickThread;
static uint8_t*      s_buf;
static lv_indev_t*   s_indev;
static lv_group_t *  s_group;
static bool          s_isQuit = false;

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
    memcpy(LCD_getFrameBuffer(), px_map, LCD_FRAME_BUFFER_SIZE);
    LCD_displayFrame();
    
    /* IMPORTANT!!!
     * Inform LVGL that flushing is complete so buffer can be modified again. */
    lv_display_flush_ready(s_display);
}

static void rollerClickedHandler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);

    // LV_ENENT_CLICKED
    int curIdx = lv_roller_get_selected(obj);
    int lastIdx = lv_roller_get_option_count(obj) - 1;

    if (curIdx == lastIdx) {
        lv_obj_del(obj);
        lv_refr_now(s_display);
        s_isQuit = true;
        return;
    }

    // Select a game
    const char* dir;
    const char* executableName;
    const char* argv0;

    // pvrTriangle
    if (curIdx == 0) {
        dir = "../pvrTriangle";
        executableName = "./pvrTriangle";
        argv0 = "pvrTriangle";

    // pvrGnomeToy
    } else if (curIdx == 1) {
        dir = "../pvrGnomeToy";
        executableName = "./pvrGnomeToy";
        argv0 = "pvrGnomeToy";

    // breakout
    } else if (curIdx == 2) {
        dir = "../breakout";
        executableName = "./breakout";
        argv0 = "breakout";
    // pong
    } else if (curIdx == 3) {
        dir = "../pong";
        executableName = "./pong";
        argv0 = "pong";
    }

    // Deinit HAL before fork
    Joystick_deinit();
    LCD_deinit(); 
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    // Child process
    if (pid == 0) {
        // Change to the target directory
        if (chdir(dir) != 0) {
            perror("chdir failed");
            exit(EXIT_FAILURE);
        }

        execl(executableName, argv0, (char *)NULL);
        
        // If execl returns, it must have failed
        perror("execl");
        exit(EXIT_FAILURE);

    // Parent process
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("Child process return\n");

        // Reinitizlize HAL
        LCD_init();
        uint8_t lcdParam = 0x68;
        LCD_command(0x36, &lcdParam, 1); // Flip the screen

        Joystick_init();
        lv_group_set_editing(s_group, true);

        // lv_refr_now(s_display);
        if (WIFEXITED(status)) {
            printf("Child exited with status %d.\n", WEXITSTATUS(status));
        } else {
            printf("Child terminated abnormally.\n");
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
                          "pong\n"
                          "Quit",
                          LV_ROLLER_MODE_INFINITE);
    lv_obj_add_style(s_roller, &style_sel, LV_PART_MAIN);
    lv_roller_set_visible_row_count(s_roller, 3);
    lv_obj_center(s_roller);
    lv_obj_add_event_cb(s_roller, rollerClickedHandler, LV_EVENT_CLICKED, NULL);
}


void joystickIndevRead(lv_indev_t * indev, lv_indev_data_t * data){
    (void) indev;

    // Prevent multiple triggers
    static bool was_up = false;
    static bool was_down = false;

    JoystickReading reading = Joystick_read();

    // UP
    if (reading.y > 80) {
        if (!was_up) {
            data->enc_diff--;
            was_up = true;
        }
    } else {
        was_up = false;
    }

    // DOWN
    if (reading.y < 20) {
        if (!was_down) {
            data->enc_diff++;
            was_down = true;
        }
    } else {
        was_down = false;
    }

    // Update press state
    if(Joystick_isPressed()) {
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

}

int main()
{
    // Initialze
    LCD_init();
    uint8_t lcdParam = 0x68;
    LCD_command(0x36, &lcdParam, 1); // Flip the screen vertically
    Joystick_init();
    lv_init();
    
    // Create background thread to update lvgl tick
    if (pthread_create(&s_tickThread, NULL, tickThread, NULL) != 0) {
        perror("Failed to create LVGL tick thread");
    }

    // Create display
    s_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_color_format(s_display, LV_COLOR_FORMAT_RGB565);

    // Set up frame buffer and flush call back
    s_buf = (uint8_t*)malloc(LCD_FRAME_BUFFER_SIZE);
    lv_display_set_buffers(s_display, s_buf, NULL, LCD_FRAME_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(s_display, flushFrameBufferCallback);

    // Create input device
    s_indev = lv_indev_create();
    lv_indev_set_type(s_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(s_indev, joystickIndevRead);
    
    // Create UI elements
    createRoller();

    // Link input device to UI elements
    s_group = lv_group_create();
    lv_group_add_obj(s_group, s_roller);
    lv_indev_set_group(s_indev, s_group);
    lv_group_set_editing(s_group, true);

    // Main update loop
    while (!s_isQuit) {
        int delay = lv_timer_handler();
        if (delay < 1) delay = 1;
        usleep(delay * 1000);
    }

    // Clean up
    lv_deinit();
    Joystick_deinit();
    LCD_deinit();
    return 0;  
}
