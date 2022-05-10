#include <stdlib.h>
#include <stdio.h>
#include "macrohandler.h"
#include "reader.h" // JSON Reader
#include "filereadstream.h"
#include "document.h"
#include "msc.h"

Value& CMacro::GetMacroPage(uint8_t pageNmbr)
{
    FIL fp;
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
    device.mmc.f_close(&fp);
    device.tft.fillScreen(BLACK);
    doc.Parse(macrobuf);

    assert(doc.HasMember("page"));
    const Value& pages = doc["page"];
    if (pageNmbr >= pages.Size())
    {
        return (Value)0;
    }
    Value& page = doc[pageNmbr];
    assert(page.HasMember("button"));
    &buttons = page["button"];
    assert(buttons.IsArray());
    for (SizeType i = 0; i < buttons.Size(); i++) // Uses SizeType instead of size_t
    {
        const Value& but = buttons[i];
        //        but.SetObject();
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
        printf("name: %s\r\n", but["name"].GetString());
        //        printf("keys: %s\r\n", but["keys"].GetArray());
                //        
        CreateShortCut(&reader, i, but["icon"].GetString(), but["name"].GetString());
    }

    return buttons;
}