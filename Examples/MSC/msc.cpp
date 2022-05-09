// TFT_ Display.cpp : Defines the entry point for the application.
//

#include <stdlib.h>
#include <stdio.h>
#include <hardware/spi.h>
#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include "Adafruit_GFX.h"    // Core graphics library
#include "Adafruit_SPITFT.h" // Hardware-specific library for ST7789
#include "msc.h"
//#include "TFTSDTouch.h"
#include "ImageReader/ImageReader.h"

//Adafruit_SPITFT tft = Adafruit_SPITFT(240,320);
TFTSDTouch device = TFTSDTouch();

#define BUTSIZE 64
float p = 3.1415926;

uint32_t millis()
{
    return to_ms_since_boot(get_absolute_time());
}

bool isInside(uint16_t x, uint16_t y, uint16_t sz, uint16_t xpoint, uint16_t ypoint)
{
    if ((xpoint >= x && xpoint <= (x + sz)) && (ypoint >= y && ypoint <= (y + sz)))
        return 1;
    return 0;
}

int main(void) {

    stdio_init_all();
//    Serial.begin(9600);
//   sleep_ms(5000);
    printf("Hello! ST77xx TFT Test\r\n");
    tusb_init();
    device.tft.init(240, 320);           // Init ST7789 320x240

    device.mmc.Init();

    device.tft.setRotation(L2R_D2U);
    device.tft.fillScreen(BLACK);

    device.TP.Init(&device.tft);
    ImageReader reader(device.mmc);

    device.TP.GetAdFac(); 
//    device.TP.Adjust();
    device.tft.setCursor(0, 9);
    device.tft.setTextSize(2);
    device.tft.println("Trying to read from SD");

    int32_t w, h = 0;

    uint8_t state = reader.drawBMP("cubase.bmp", device.tft, 14, 14);

    printf("Initialized\r\n");
    device.tft.setRotation(L2R_D2U);

    uint16_t x = 10, y = 10;
    device.tft.drawRoundRect(x, y, BUTSIZE, BUTSIZE,8, WHITE);
    x = 250, y = 10;
    device.tft.drawRoundRect(x, y, BUTSIZE, BUTSIZE, 8, WHITE);

    while(1)
    {
        tud_task();
        hid_task();
        device.TP.Scan();
        if (device.TP.status().chStatus & TP_PRESS_DOWN)
        {
            uint16_t xpoint = device.TP.DrawPoint().Xpoint;
            uint16_t ypoint = device.TP.DrawPoint().Ypoint;
            if (isInside(10, 10, BUTSIZE, xpoint, ypoint))
            {
                device.tft.invertDisplay(true);
            }
            if (isInside(320-BUTSIZE-10, 10, BUTSIZE, xpoint, ypoint))
            {
                device.tft.invertDisplay(false);
            }
        }
        ;
    }
    return 0;
}


//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
//    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
//    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
//    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
//    blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = board_button_read();
    if (tud_suspended() && btn)
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }
    else
    {
        // keyboard interface
        if (tud_hid_n_ready(ITF_NUM_KEYBOARD))
        {
            // used to avoid send multiple consecutive zero report for keyboard
            static bool has_keyboard_key = false;

            uint8_t const report_id = 0;
            uint8_t const modifier = 0;

            if (btn)
            {
                uint8_t keycode[6] = { 0 };
                keycode[0] = HID_KEY_ARROW_RIGHT;

                tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
                has_keyboard_key = true;
            }
            else
            {
                // send empty key report if previously has key pressed
                if (has_keyboard_key) tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, NULL);
                has_keyboard_key = false;
            }
        }

        // mouse interface
        if (tud_hid_n_ready(ITF_NUM_MOUSE))
        {
            if (btn)
            {
                uint8_t const report_id = 0;
                uint8_t const button_mask = 0;
                uint8_t const veritical = 0;
                uint8_t const horizontal = 0;
                int8_t  const delta = 5;

                tud_hid_n_mouse_report(ITF_NUM_MOUSE, report_id, button_mask, delta, delta, veritical, horizontal);
            }
        }
    }
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol)
{
    (void)instance;
    (void)protocol;

    // nothing to do since we use the same compatible boot report for both Boot and Report mode.
    // TOOD set a indicator for user
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
    (void)instance;
    (void)report;
    (void)len;

    // nothing to do
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void)report_id;

    // keyboard interface
    if (instance == ITF_NUM_KEYBOARD)
    {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_type == HID_REPORT_TYPE_OUTPUT)
        {
            // bufsize should be (at least) 1
            if (bufsize < 1) return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
            {
                // Capslock On: disable blink, turn led on
  //              blink_interval_ms = 0;
  //              board_led_write(true);
            }
            else
            {
                // Caplocks Off: back to normal blink
//                board_led_write(false);
//                blink_interval_ms = BLINK_MOUNTED;
            }
        }
    }
}
