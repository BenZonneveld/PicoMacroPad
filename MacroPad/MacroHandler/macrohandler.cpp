#include <stdlib.h>
#include <stdio.h>
#include <tusb.h>
#include <unordered_map>

#include "MacroPad.h"
#include "macrohandler.h"
#include "ImageReader/ImageReader.h"

//#include "reader.h" // JSON Reader
//#include "filereadstream.h"
#include "document.h"
#include "TinyUSB_Mouse_and_Keyboard.h"

CMacro::CMacro()
{
    buttons = 0;
    Keyboard.begin();
//    metakeys["LEFT_CTRL"] = LEFT_CTRL;

    metakeys = {
        {"LEFT_CTRL",0x80},
        {"LEFT_SHIFT",0x81},
        {"LEFT_ALT",0x82},
        {"LEFT_GUI",0x83},
        {"CTRL", 0x80}, // Assume left ctrl
        {"SHIFT", 0x81}, // left shift
        {"ALT", 0x82}, // left alt
        {"GUI", 0x83},
        {"RIGHT_CTRL",0x84},
        {"RIGHT_SHIFT",0x85 },
        {"RIGHT_ALT",0x86 },
        {"RIGHT_GUI",0x87 },
        {"UP_ARROW",0xDA },
        {"DOWN_ARROW",0xD9 },
        {"LEFT_ARROW",0xD8 },
        {"RIGHT_ARROW",0xD7 },
        {"BACKSPACE",0xB2 },
        {"TAB",0xB3 },
        {"RETURN",0xB0 },
        {"ESC",0xB1 },
        {"INSERT",0xD1 },
        {"DELETE",0xD4 },
        {"PAGE_UP",0xD3},
        {"PAGE_DOWN",0xD6},
        {"HOME",0xD2},
        {"END",0xD5},
        {"CAPS_LOCK",0xC1},
        {"F1",0xC2},
        {"F2",0xC3},
        {"F3",0xC4},
        {"F4",0xC5},
        {"F5",0xC6},
        {"F6",0xC7},
        {"F7",0xC8},
        {"F8",0xC9},
        {"F9",0xCA},
        {"F10",0xCB},
        {"F11",0xCC},
        {"F12",0xCD},
        {"F13",0xF0},
        {"F14",0xF1},
        {"F15",0xF2},
        {"F16",0xF3},
        {"F17",0xF4},
        {"F18",0xF5},
        {"F19",0xF6},
        {"F20",0xF7},
        {"F21",0xF8},
        {"F22",0xF9},
        {"F23",0xFA},
        {"F24",0xFB}
    };
}
void CMacro::getMacroPage(uint8_t pageNmbr)
{
    int16_t  x1, y1;
    uint16_t w, h;
    FIL fp;
    char pagename[64] = {0};
    mount_card();
    uint8_t res = f_open(&fp, (char*)"macro.json", FA_READ);
    printf("res: %i\r\n", res);

    char macrobuf[65535] = { 0 };
    UINT bread = 0;
    if (res == 0)
    {
        f_read(&fp, macrobuf, 65535, &bread);
        //   printf("Bytes read: %i\r\n", bread);
        //   printf("Macrobuf: %s\r\n", macrobuf);
    }
    else {
        umount_card();
        return;
    }
    f_close(&fp);
    tft.fillScreen(BLACK);
    ParseResult ok = doc.Parse(macrobuf);

    printf("Parsed macro.json\r\n");
    if (ok)
    {
        assert(doc.HasMember("page"));
        const Value& pages = doc["page"];
        if (pageNmbr >= pages.Size())
        {
            return;
        }
        page = pageNmbr;

        if (doc["page"][page].HasMember("name"))
        {
            sprintf(pagename, "%s", doc["page"][page]["name"].GetString());
        }
        else {
            sprintf(pagename, "Page %i", page + 1);
        }
        tft.setFont(&FreeSans12pt7b);
        tft.setTextColor(WHITE);
        tft.getTextBounds(pagename, 0, 20, &x1, &y1, &w, &h);
        tft.setCursor((tft.width() / 2) - (w / 2) - 2, h);
        tft.print(pagename);

        Value& page = doc["page"][pageNmbr];
        assert(page.HasMember("button"));
        buttons = page["button"];
        assert(buttons.IsArray());
        for (SizeType i = 0; i < buttons.Size(); i++) // Uses SizeType instead of size_t
        {
            const Value& but = buttons[i];
            assert(but.IsArray());
            assert(but.HasMember["icon"]);
            assert(but["icon"].IsString());
            assert(but.HasMember["name"]);
            assert(but["name"].IsString());
            assert(but.HasMember["keys"]);
            assert(but["keys"].IsArray());

            //        printf("Pos: %i\r\n", but["pos"].GetInt());
            //        printf("Icon: %s\r\n", but["icon"].GetString());
    //        printf("name: %s\r\n", but["name"].GetString());
            //        printf("keys: %s\r\n", but["keys"].GetArray());

            CreateShortCut(i, but["icon"].GetString(), but["name"].GetString());
        }
    }
    else {
        tft.fillScreen(WHITE);
        tft.setCursor(0, 100);
        tft.setTextSize(1);
        tft.setFont(&FreeSans24pt7b);
        tft.setTextColor(RED);
        tft.println("Error in");
        tft.println("macro.json.");
        printf("error in json\r\n");
    }
    return;// buttons;
}

void CMacro::CreateShortCut(uint8_t pos, const char* icon, const char* name)
{
    uint16_t ypos = YOFFSET;
    uint16_t xpos = XOFFSET + ((BUTSIZE + XSPACING) * pos);
    if ((pos % 3) == 0 && pos > 0)
    {
        ypos = ypos + BUTSIZE + YSPACING;
        xpos = XOFFSET;
    }
    ImageReader reader;

    uint8_t state = reader.drawBMP(icon, tft, xpos, ypos + 16);
    tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 6, BLACK);
    tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 4, BLACK);
    tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 2, BLACK);
    tft.drawRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, BLACK);

    tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 8, WHITE);
    tft.setFont(&FreeSans9pt7b);
    int16_t  x1, y1;
    uint16_t w, h;
    tft.getTextBounds(name, 0, 20, &x1, &y1, &w, &h);
    tft.setCursor(xpos + (BUTSIZE / 2) - (w / 2) - 2, ypos + h);
    tft.print(name);
}

bool CMacro::checkHit(uint16_t xpoint, uint16_t ypoint)
{
    bool Pressed = false;
    for (uint8_t pos = 0; pos < buttons.Size(); pos++)
    {
        const Value& but = buttons[pos];
        assert(but.IsArray());
//        assert(but.HasMember["name"]);
//        assert(but["name"].IsString());
        assert(but.HasMember["keys"]);
        assert(but["keys"].IsArray());
        uint16_t ypos = YOFFSET;
        uint16_t xpos = XOFFSET + ((BUTSIZE + XSPACING)*pos);
        if ((pos % 3) == 0 && pos > 0)
        {
            ypos = ypos + BUTSIZE + YSPACING;
            xpos = XOFFSET;
        }
        //                printf("index: %i, xpoint: %i, ypoint: %i\r\n", i, xpoint, ypoint);
        if (isInside(xpos, ypos, BUTSIZE, xpoint, ypoint))
        {
            Pressed = true;
            const Value& keys = but["keys"];
            //                    keys.SetArray();
            assert(keys.IsArray());
            printf("I: %i\r\n", pos);
            printf("Keys size %i\r\n", keys.Size());
            uint8_t keycode[6] = { 0 };
            for (SizeType i = 0; i < keys.Size(); i++) // Uses SizeType instead of size_t
            {
                char key[32];
//                printf("keys[%i].Size() = %i\r\n", i, keys[i].Size());
                for (SizeType m = 0; m < keys[i].Size(); m++)
                {
                    snprintf(key, sizeof(key), "%s", keys[i][m].GetString());
                    if (strlen(key) > 1) // check for special or modifier keys
                    {
                        Upper(key);
                        if (metakeys[key] == 0)
                        {
                            tft.println("Invalid metakey.");
                            tft.print(key);
                            //return Pressed;
                        }
                        else {
                            printf("metakey %s\r\n", key);
                            Keyboard.press(metakeys[key]);
                        }
                    }
                    else {
                        tolower(key[0]);
                        Keyboard.press(key[0]);
                    }
                    printf("Key %s\r\n", key);
                }
                Keyboard.releaseAll();
            }
        }
    }
    return Pressed;
}

void CMacro::Upper(char* temp)
{
    char* s = temp;
    while (*s)
    {
        *s = toupper((unsigned char)*s);
        s++;
    }
}

bool CMacro::isInside(uint16_t x, uint16_t y, uint16_t sz, uint16_t xpoint, uint16_t ypoint)
{
    if ((xpoint >= x && xpoint <= (x + sz)) && (ypoint >= y && ypoint <= (y + sz)))
        return 1;
    return 0;
}
