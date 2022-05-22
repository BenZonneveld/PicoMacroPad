// TFT_ Display.cpp : Defines the entry point for the application.
//

#include <stdlib.h>
#include <stdio.h>
#include "pico/binary_info.h"

#include <hardware/pwm.h>
#include <hardware/irq.h>
#include <hardware/watchdog.h>
//#include <pico/multicore.h>
//#include <hardware/spi.h>

/* FAT FS */
#include "f_util.h"
#include "ff.h"
#include "hw_config.h"

#include <bsp/board.h>
#include <tusb.h>
#include "usb_descriptors.h"
//#include "reader.h" // JSON Reader
//#include "filereadstream.h"
#include "document.h"

/* Screen Handling */
#include "GFX-lib/Adafruit_GFX.h"
#include "GFX-lib/Adafruit_SPITFT.h"

#include "gpio_pins.h"
#include "Touch.h"
//#include "debounce.h"
#include "MacroHandler/macrohandler.h"
#include "MacroPad.h"

#define PROGMEM
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans18pt7b.h"
#include "fonts/FreeSans24pt7b.h"
//Adafruit_SPITFT tft = Adafruit_SPITFT(240,320);
//TFTSDTouch device = TFTSDTouch();

#include "msc_disk.h"
#include "GUI/gui.h"

cGUI gui;
cTouch TP = cTouch();
Adafruit_SPITFT tft = Adafruit_SPITFT();
CMacro h_macro = CMacro();
Debounce debouncer;
static uint8_t sb_pressed = 0;
static int fade;
static uint32_t btnTimeStamp = 0;
uint8_t sbuttons = 0;
bool buttons_read = false;
bool sleep_mode = false;
struct repeating_timer timer;

using namespace rapidjson;
uint32_t millis()
{
    return to_ms_since_boot(get_absolute_time());
}

void loadJSON()
{
    if (! h_macro.loadJSON())
    {
        tft.fillScreen(WHITE);
        tft.setCursor(0, 100);
        tft.setTextSize(1);
        tft.setFont(&FreeSans24pt7b);
        tft.setTextColor(RED);
        tft.println("Error in");
        tft.println("macro.json.");
        printf("error in json\r\n");
    }
}

bool autofade(struct repeating_timer* t) {
    static bool going_up = true;
    if (fade > MAXFADE)
        going_up = false;
    // Clear the interrupt flag that brought us here
//    pwm_clear_irq(pwm_gpio_to_slice_num(LCD_BKL_PIN));
    if (going_up) {
        ++fade;
        if (fade > MAXFADE) {
            fade = MAXFADE;
            going_up = false;
        }
    }
    else {
        if (fade > MAXFADE)
        {
            fade -= 2;
        }
        else
        {
            --fade;
        }
        if (fade < 16) {
            fade = 16;
            going_up = true;
        }
    }
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(LCD_BKL_PIN, fade * fade);
    return true;
}

int main(void) {
    set_sys_clock_khz(133000, true);

    stdio_init_all();
    printf("Hello! MacroPad debug.\r\n");
    tusb_init();
//    multicore_launch_core1(setup);
    // Setup Softbuttons
    gpio_init(PIN_SB0);
    gpio_init(PIN_SB1);
    gpio_init(PIN_SB2);
    gpio_init(PIN_SB3);
    gpio_set_dir(PIN_SB0, GPIO_IN);
    gpio_set_dir(PIN_SB1, GPIO_IN);
    gpio_set_dir(PIN_SB2, GPIO_IN);
    gpio_set_dir(PIN_SB3, GPIO_IN);
    gpio_pull_up(PIN_SB0);
    gpio_pull_up(PIN_SB1);
    gpio_pull_up(PIN_SB2);
    gpio_pull_up(PIN_SB3);

    mount_card();
    debouncer.debounce_gpio(PIN_SB0);
    debouncer.debounce_gpio(PIN_SB1);
    debouncer.debounce_gpio(PIN_SB2);
    debouncer.debounce_gpio(PIN_SB3);

    gpio_set_irq_enabled_with_callback(PIN_SB0, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,true, &gpio_callback);
    gpio_set_irq_enabled(PIN_SB1, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(PIN_SB2, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(PIN_SB3, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
 //   gpio_set_irq_enabled(TP_IRQ_PIN, GPIO_IRQ_LEVEL_LOW, true);
    
    tft.init(240, 320);           // Init ST7789 320x240
//    device.mmc.Init();
    tft.setRotation(L2R_D2U);
    TP.Init(&tft);
    bi_decl(bi_3pins_with_func(LCD_MISO_PIN, LCD_MOSI_PIN, LCD_CLK_PIN, GPIO_FUNC_SPI));
    bi_decl(bi_1pin_with_name(LCD_RST_PIN, "LCD Reset"));
    bi_decl(bi_1pin_with_name(LCD_DC_PIN, "LCD DC"));
    bi_decl(bi_1pin_with_name(LCD_CS_PIN, "LCD CS"));
    bi_decl(bi_1pin_with_name(LCD_BKL_PIN, "LCD BKL"));
    bi_decl(bi_1pin_with_name(TP_CS_PIN, "TP CS"));
    bi_decl(bi_1pin_with_name(TP_IRQ_PIN, "TP IRQ (IN)"));
    bi_decl(bi_1pin_with_name(SD_CS_PIN, "TP CS"));
    bi_decl(bi_1pin_with_name(PIN_SB0, "SButton 0"));
    bi_decl(bi_1pin_with_name(PIN_SB1, "SButton 1"));
    bi_decl(bi_1pin_with_name(PIN_SB2, "SButton 2"));
    bi_decl(bi_1pin_with_name(PIN_SB3, "SButton 3"));


    //, LCD_CD_PIN, LCD_CS_PIN, GPIO_OUT));
    /*for (*/ uint8_t blue = (uint8_t)((BLUE & 0x001F) << 3);// blue > 0; blue--)
    //{
    tft.fillScreen(tft.color565(0, 0, BLACK));
    //}
    TP.GetAdFac();
    tft.setCursor(100, 100);
    tft.setTextSize(1);
    tft.setFont(&FreeSans24pt7b);
    tft.println("INIT");

    h_macro.init();
    h_macro.showPage(0);

    //    pwm_clear_irq(pwm_gpio_to_slice_num(LCD_BKL_PIN));
    //    pwm_set_irq_enabled(pwm_gpio_to_slice_num(LCD_BKL_PIN), true);
    //    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    //    irq_set_enabled(PWM_IRQ_WRAP, false);

    printf("Initialized\r\n");
    taskloop();
}
//    us_time = to_us_since_boot(get_absolute_time());

    //    gpio_set_irq_enabled_with_callback(TP_IRQ_PIN, GPIO_IRQ_EDGE_FALL, true, &inter_test);
void taskloop()
{
    bool Pressed = false;
    uint16_t Xpoint0 = 0, Ypoint0 = 0xffff;
    uint32_t idle_time;
    uint32_t us_time = to_ms_since_boot(get_absolute_time());
    int8_t sb = -1;

    while (1)
    {
        tud_task();
        idle_time = to_ms_since_boot(get_absolute_time()) - us_time;
        doSoftButtons();

        if (sbuttons != 0 && !buttons_read)
        {
            buttons_read = true;
            idle_time = 0; 
            us_time = to_ms_since_boot(get_absolute_time());
            uint8_t count = 0;
            
            for (uint8_t i = 0; i < 8; i++)
            {
                count += (sbuttons >> i) & 1;
            }
            disableSleep();
            switch (count)
            {
            case 1:  // Single button
                for (uint8_t i = 0; i < 4; i++)
                {
                    if ((sbuttons >> i) & 1)
                    {
                        h_macro.forceCallback(i);
                    }
                }
                break;
            case 2: // 2 Button chord
                switch (sbuttons)
                {
                case 0x3: // SB0 & SB1
                    gui.brightness();
                    h_macro.showPage(h_macro.getPage());
                    break;
                case 0x6: // SB1 && SB2
 //                   gui.test();
 //                   h_macro.showPage(h_macro.getPage());
                    break;
                case 0xC: // SB2 && SB3
                    TP.Adjust();
                    h_macro.showPage(h_macro.getPage());
                    break;
                default: 
                    break;
                }
                if ( sbuttons == 0x3 ) // SB0 & SB1
                if ( sbuttons)
                break;
            case 3: // 3 button chord
                break;
            case 4: // all buttons!
                h_macro.loadJSON(); // reload json
                h_macro.showPage(0);
//                watchdog_enable(1, 1);
//                while (1);
                break;
            default: // how the hell did we get here?
                break;
            }
        }

        TP.Scan();
        if (TP.status().chStatus & TP_PRESS_DOWN)
        {
            uint16_t xpoint = TP.DrawPoint().Xpoint;
            uint16_t ypoint = TP.DrawPoint().Ypoint;
            if (xpoint == Xpoint0 && ypoint == Ypoint0)
                continue;
            Xpoint0 = xpoint;
            Ypoint0 = ypoint;
            if (!Pressed)
                Pressed = h_macro.checkHit(xpoint, ypoint);
            if (!Pressed)
            {
                sb = h_macro.hitSoftButton(xpoint, ypoint);
                if (sb >= 0)
                {
                    Pressed = true;
                }
            }
            us_time = to_ms_since_boot(get_absolute_time());
        }
        else {
            Pressed = false;
        }

        /*
         Dim the backlight when the unit is not used for MAXIDLE seconds
        */
        if (Pressed)
            idle_time = 0;

        if ((idle_time /1000) > MAXIDLE)
        {
            if (!sleep_mode)
            {
                fade = tft.getBacklight();
                add_repeating_timer_ms(6554/MAXFADE, autofade, NULL, &timer);
                sleep_mode = true;
            }
        }
        else {
            disableSleep();
        }
    }
}

void doSoftButtons()
{
    if (!debouncer.read(PIN_SB0) && (sb_pressed & 0x1) == false)
    {
        sb_pressed |= SB0;
    }
    if (debouncer.read(PIN_SB0)) {
        sb_pressed &= ~(SB0);
    }

    if (!debouncer.read(PIN_SB1) && (sb_pressed & 0x2) == false)
    {
        sb_pressed |= SB1;
    }
    if (debouncer.read(PIN_SB1)) {
        sb_pressed &= ~(SB1);
    }

    if (!debouncer.read(PIN_SB2) && (sb_pressed & 0x4) == false)
    {
        sb_pressed |= SB2;
    }
    if (debouncer.read(PIN_SB2)) {
        sb_pressed &= ~(SB2);
    }

    if (!debouncer.read(PIN_SB3) && (sb_pressed & 0x8) == false)
    {
        sb_pressed |= SB3;
    }
    if (debouncer.read(PIN_SB3)) {
        sb_pressed &= ~(SB3);
    }

    if ( sb_pressed != 0x0 && to_ms_since_boot(get_absolute_time()) - btnTimeStamp > 50)
    {
        sbuttons = sb_pressed;
    }
    else {
        if (sb_pressed == 0 && btnTimeStamp != 0)
        {
            btnTimeStamp = 0;
            sbuttons = 0;
        }
    }
}

void gpio_callback(uint gpio, uint32_t events)
{
    buttons_read = false;
    btnTimeStamp = to_ms_since_boot(get_absolute_time()); // Create timestamp
}

void disableSleep()
{
    if (sleep_mode)
    {
        cancel_repeating_timer(&timer);
        tft.setBacklight(tft.getBacklight());
        sleep_mode = false;
    }
}