#include <stdlib.h>
#include <stdio.h>
#include "macrohandler.h"
#include "ImageReader/ImageReader.h"

//#include "reader.h" // JSON Reader
//#include "filereadstream.h"
#include "document.h"
#include "MacroPad.h"

void CMacro::getMacroPage(uint8_t pageNmbr)
{
    int16_t  x1, y1;
    uint16_t w, h;
    FIL fp;
    char pagename[64] = {0};
    uint8_t res = device.mmc.f_open(&fp, (char*)"macro.jsn", FA_READ);
    printf("res: %i\r\n", res);

    char macrobuf[65535] = { 0 };
    UINT bread = 0;
    if (res == 0)
    {
        device.mmc.f_read(&fp, macrobuf, 65535, &bread);
        //   printf("Bytes read: %i\r\n", bread);
        //   printf("Macrobuf: %s\r\n", macrobuf);
    }
    else {
        return;
    }
    device.mmc.f_close(&fp);
    device.tft.fillScreen(BLACK);
    doc.Parse(macrobuf);

    printf("Parsed macro.json\r\n");

    assert(doc.HasMember("page"));
    const Value& pages = doc["page"];
    if (pageNmbr >= pages.Size())
    {
        return;
    }
    page = pageNmbr;

    sprintf(pagename, "Page %i", page + 1);
    device.tft.setFont(&FreeSans12pt7b);
    device.tft.setTextColor(WHITE);
    device.tft.getTextBounds(pagename, 0, 20, &x1, &y1, &w, &h);
    device.tft.setCursor((device.tft.width()/2) - (w / 2) - 2, h);
    device.tft.print(pagename);

    Value& page = doc["page"][pageNmbr];
    assert(page.HasMember("button"));
    buttons = page["button"];
    assert(buttons.IsArray());
    for (SizeType i = 0; i < buttons.Size(); i++) // Uses SizeType instead of size_t
    {
        const Value& but = buttons[i];
        assert(but.IsArray());
        assert(but.HasMember["pos"]);
        assert(but["pos".IsInt()]);
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
    ImageReader reader(device.mmc);

    uint8_t state = reader.drawBMP(icon, device.tft, xpos, ypos + 16);
    device.tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 6, BLACK);
    device.tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 4, BLACK);
    device.tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 2, BLACK);
    device.tft.drawRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, BLACK);

    device.tft.drawRoundRect(xpos, ypos + 16, BUTSIZE, BUTSIZE, 8, WHITE);
    device.tft.setFont(&FreeSans9pt7b);
    int16_t  x1, y1;
    uint16_t w, h;
    device.tft.getTextBounds(name, 0, 20, &x1, &y1, &w, &h);
    device.tft.setCursor(xpos + (BUTSIZE / 2) - (w / 2) - 2, ypos + h);
    device.tft.print(name);
}

bool CMacro::checkHit(uint16_t xpoint, uint16_t ypoint)
{
    bool Pressed = false;
    for (uint8_t pos = 0; pos < buttons.Size(); pos++)
    {
        const Value& but = buttons[pos];
        assert(but.IsArray());
        assert(but.HasMember["name"]);
        assert(but["name"].IsString());
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
            for (SizeType i = 0; i < keys.Size(); i++) // Uses SizeType instead of size_t
            {
                const char* key = keys[i].GetString();
                printf("Key %s\r\n", key);
            }
        }
    }
    return Pressed;
}

bool CMacro::isInside(uint16_t x, uint16_t y, uint16_t sz, uint16_t xpoint, uint16_t ypoint)
{
    if ((xpoint >= x && xpoint <= (x + sz)) && (ypoint >= y && ypoint <= (y + sz)))
        return 1;
    return 0;
}
