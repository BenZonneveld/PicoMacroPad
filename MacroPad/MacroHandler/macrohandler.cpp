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

void CMacro::init()
{
    SoftButton(0, NULL);
    SoftButton(1, NULL);
    SoftButton(2, NULL);
    SoftButton(3, NULL);
}

void CMacro::PrevPage()
{
    if (page > 0)
    {
        page--;
        getMacroPage(page);
    }
}

void CMacro::NextPage()
{
    if (page + 1 < max_page)
    {
        page++;
        getMacroPage(page);
    }
}

void CMacro::getCurrentPage()
{
    getMacroPage(page);
}

void CMacro::getMacroPage(uint8_t pageNmbr)
{
    int16_t  x1, y1;
    uint16_t w, h;
    FIL fp;
    mount_card();
    uint8_t res = f_open(&fp, (char*)"macro.json", FA_READ);
    printf("res: %i\r\n", res);

    char macrobuf[65535] = { 0 };
    UINT bread = 0;
    if (res == 0)
    {
        f_read(&fp, macrobuf, 65535, &bread);
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
        max_page = pages.Size();

        printf("max_page %i\r\n", max_page);

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
        tft.fillRect(0, 0, tft.width(), h + 4 , DARKGREY);
        tft.setCursor((tft.width() / 2) - (w / 2) - 2, h);
        
        tft.print(pagename);

        if (max_page > 1 && (page+1) < max_page) // Indicate next pages available
        {
//            tft.fillTriangle(320, h / 2, 300, 0, 300, h, GREEN);
            SoftButton(3, (char*)"Page +");
        } else{
            SoftButton(3, NULL);
        }
        if (max_page > 1 && page > 0) // Indicate previous pages available
        {
            SoftButton(0, (char*)"Page -");
//            tft.fillTriangle(0, h / 2, 20, 0, 20, h, GREEN);
        }
        else {
            SoftButton(0, NULL);
        }

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
    SoftButton(1);
    SoftButton(2);
    return;// buttons;
}

void CMacro::CreateShortCut(uint8_t pos, const char* icon, const char* name)
{
    uint16_t ypos = YOFFSET;
    uint16_t xpos = XOFFSET + ((BUTSIZE + XSPACING) * ( pos % 3));
    
    if ( pos/3 > 0)
    {
        ypos = ypos + BUTSIZE + YSPACING;
//        xpos = XOFFSET + (pos % 3);
    }
    ImageReader reader;

    uint32_t mytime = to_ms_since_boot(get_absolute_time());
    uint8_t state = reader.drawBMP(icon, tft, xpos, ypos + 16);
    mytime = to_ms_since_boot(get_absolute_time()) - mytime;

    printf("drawBMP took %" PRIu32 " milliseconds.\r\n", mytime);
    tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 6, BLACK);
    tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 4, BLACK);
    tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 2, BLACK);
    tft.drawRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, BLACK);

    tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 8, WHITE);
    tft.setFont(&FreeSans9pt7b);
    int16_t  x1, y1;
    uint16_t w, h;
    tft.setTextColor(WHITE);
    tft.getTextBounds(name, 0, 20, &x1, &y1, &w, &h);
    tft.setCursor(xpos + (BUTSIZE / 2) - (w / 2) - 2, ypos+10 );
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
//            printf("I: %i\r\n", pos);
//            printf("Keys size %i\r\n", keys.Size());
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
                            snprintf(key, sizeof(key), "%s", keys[i][m].GetString());
                            Keyboard.print(key);
                            printf("Keystring: %s\r\n", key);
                        }
                        else {
                            printf("metakey %s\r\n", key);
                            Keyboard.press(metakeys[key]);

                        }
                    }
                    else if (key[0] != '\0') {
                        tolower(key[0]);
                        Keyboard.press(key[0]);
                        printf("Key %s\r\n", key);
                    }
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

int8_t CMacro::hitSoftButton( uint16_t xpoint, uint16_t ypoint)
{
//    printf("x: %i, y: %i\r\n", xpoint, ypoint);
//    printf("x mod 80 : %i\r\n", xpoint / 80);
    if (strlen(sbnames[xpoint / 80]) == 0 || sbnames[xpoint / 80][0] == '\0')
        return -1;

    if ((xpoint >= ((xpoint / 80) * 80) && xpoint <= ((xpoint / 80) * 80 + 80)) && (ypoint >= 212 && ypoint <= 240))
    {
        return xpoint / 80;
    }
    return -1;
}
void CMacro::SoftButton(uint8_t pos)
{
    SoftButton(pos, sbnames[pos]);
}

void CMacro::SoftButton(uint8_t pos, char* sbname)
{
    int16_t  x1, y1;
    uint16_t w, h;
//    tft.setCursor(pos, 200);
    tft.setFont(&FreeSans12pt7b);
    tft.setTextColor(BLACK);
    if (sbname == NULL || strlen(sbname) == 0)
    {
        sbnames[pos][0] = '\0';
        tft.fillRoundRect(1 + pos * 80, 212, 78, 32, 4, DARKERGREY);
    }
    else {
        strncpy(sbnames[pos], sbname, sizeof(sbnames[0]));
        tft.fillRoundRect(1 + pos * 80, 212, 78, 32, 4, LIGHTGREY);
        tft.getTextBounds(sbname, 0, 20, &x1, &y1, &w, &h);
        tft.setCursor(40 + (pos * 80) - (w / 2) - 2, 233);
        tft.print(sbname);
    }
}