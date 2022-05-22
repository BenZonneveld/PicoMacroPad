#include <stdlib.h>
#include <stdio.h>
#include <bsp/board.h>

//#include "debounce.h"
#include "gui.h"
#include "tools.h"

#include "Adafruit_SPITFT.h"
#include "MacroPad.h"

bool cGUI::touchPosition()
{
//	uint16_t Xpoint0 = 0, Ypoint0 = 0xffff;
	TP.Scan();
	if (TP.status().chStatus & TP_PRESS_DOWN)
	{
		uint16_t xpoint = TP.DrawPoint().Xpoint;
		uint16_t ypoint = TP.DrawPoint().Ypoint;
		if (xpoint == Xpoint0 && ypoint == Ypoint0)
			return false;
		Xpoint0 = xpoint;
		Ypoint0 = ypoint;
		return true;
	}
	return false;
}

void cGUI::SlideHCtrl::createSlider(uint16_t xp, uint16_t yp,
	uint16_t tw, uint16_t th,
	uint16_t tmin, uint16_t tmax,
	char* name)
{
	x = xp;
	y = yp;
	w = tw;
	h = th;
	min = tmin;
	max = tmax;
	if (y + h > 210)
	{
		h = 210 - y;
	}
	if (x + w >= 320)
	{
		w = 320 - x;
	}
	tft.drawRect(x, y, w, h, WHITE);
	if (strlen(name) > 0)
	{
		tft.setFont(&FreeSans9pt7b);
		int16_t  x1, y1;
		uint16_t w1, h1;
		tft.setTextColor(WHITE);
		tft.getTextBounds(name, 0, 20, &x1, &y1, &w1, &h1);
		tft.setCursor(x + (w / 2) - (w1 / 2) - 2, y - h1);
		tft.print(name);
	}
}

void cGUI::SlideHCtrl::updateSlider(uint16_t posix, uint16_t posiy)
{
	// Boundary checking
	if (posiy < y || posiy > y + h || posix < x || posix > (x + w))
		return;

	posix = map(posix, x, x + w, min, max);
	if (value >= 0 && posix != value)
	{
		setValue(posix);
	}

}

void cGUI::SlideHCtrl::setValue(uint16_t val)
{
	int16_t width = w / (max - min);
	if (width < 10) width = 10;

	uint16_t xpos = map(val, min, max, 0, w) - width/2;
	uint16_t oldx = map(value, min, max, 0, w) - width / 2;

	tft.writeFillRect(x + oldx, y + 1, oldwidth, h - 2, BLACK);
	tft.writeFillRect(x + xpos, y + 1, width, h - 2, LIGHTGREY);
	tft.drawRect(x, y, w, h, WHITE);
	tft.drawFastVLine(x + xpos, y - 1, h + 2, BLACK);
	oldwidth = width;
	value = val;
}

void cGUI::SlideVCtrl::createSlider(uint16_t xp, uint16_t yp,
	uint16_t tw, uint16_t th,
	uint16_t tmin, uint16_t tmax,
	char* name)
{
	x = xp;
	y = yp;
	w = tw;
	h = th;
	min = tmin;
	max = tmax;
	if (y + h > 210)
	{
		h = 210 - y;
	}
	if (x + w >= 320)
	{
		w = 320 - x;
	}
	tft.drawRect(x, y, w, h, WHITE);
	if (strlen(name) > 0)
	{
		tft.setFont(&FreeSans9pt7b);
		int16_t  x1, y1;
		uint16_t w1, h1;
		tft.setTextColor(WHITE);
		tft.getTextBounds(name, 0, 20, &x1, &y1, &w1, &h1);
		tft.setCursor(x + (w / 2) - (w1 / 2) - 2, y - h1);
		tft.print(name);
	}
}

void cGUI::SlideVCtrl::updateSlider(uint16_t posix, uint16_t posiy)
{
	// Boundary checking
	if (posiy < y || posiy > y + h || posix < x || posix >(x + w))
		return;

	posiy = map(posiy, y + h , y, min, max);
	if (value >= 0 && posiy != value)
	{
		setValue(posiy);
	}

}

void cGUI::SlideVCtrl::setValue(uint16_t val)
{
	int16_t height = h / (max - min);
	if (height < 10) height = 10;

	uint16_t ypos = map(val, min, max, 0, h) - height / 2;
	uint16_t oldy = map(value, min, max, 0, h) - height / 2;

	tft.writeFillRect(x + 1, y + oldy, oldheight, h - 2, BLACK);
	tft.writeFillRect(x + 1, y + ypos, height, h - 2, LIGHTGREY);
	tft.drawRect(x, y, w, h, WHITE);
	tft.drawFastVLine(x, y + ypos, h + 2, BLACK);
	oldheight = height;
	value = val;
}
void cGUI::brightness()
{
	SlideHCtrl slider;
	int8_t sb = -1;
	int8_t but0 = debouncer.read(PIN_SB0);
	printf("but0: %i\r\n", but0);

	tft.fillScreen(BLACK);
	uint8_t current = tft.getBacklight();
	slider.createSlider(40, 100, 240, 40,128, 255, (char *)"Brightness");
	slider.setValue(current);
	h_macro.SoftButton(0, (char *)"Cancel", NULL);
	h_macro.SoftButton(1, NULL, NULL);
	h_macro.SoftButton(2, NULL, NULL);
	h_macro.SoftButton(3, (char *)"OK", NULL);
	while (1)
	{
		if ( touchPosition() )
//		if (TP.status().chStatus & TP_PRESS_DOWN)
		{
			slider.updateSlider(getXpos(), getYpos());
			tft.setBacklight(slider.getValue());
			sb = h_macro.hitSoftButton(getXpos(), getYpos());
		}

		if (but0 != debouncer.read(PIN_SB0) && but0 > 0)
		{
			printf("but0 : %i\t read %i\r\n", but0, debouncer.read(PIN_SB0));
			but0 = -1;
		}

		if ( (but0 == -1 && !debouncer.read(PIN_SB0)) || sb == 0)
		{
			tft.setBacklight(current);
			printf("cancel\r\n");
			break;
		}
		if (!debouncer.read(PIN_SB3) || sb == 3 )
			break;
	}
}

void cGUI::XYCtrl::createXY(uint16_t xpos, uint16_t ypos,
	uint16_t tw, uint16_t th,
	uint16_t xmin, uint16_t xmax,
	uint16_t ymin, uint16_t ymax,
	const char *namex, const char *namey)
{
	x = xpos;
	y = ypos;
	w = tw;
	h = th;
	minx = xmin;
	maxx = xmax;
	miny = ymin;
	maxy = ymax;
	xname = namex;
	yname = namey;
	int16_t  x1, y1;
	uint16_t w1, h1;
	if (y + h > 210)
	{
		h = 210 - y;
	}
	if (x + w >= 320)
	{
		w = 320 - x;
	}
	tft.drawRect(x, y, w, h, WHITE);
	tft.setFont(&FreeSans9pt7b);
	tft.setTextColor(WHITE);
	if (strlen(xname) > 0)
	{
		tft.getTextBounds(xname, 0, 20, &x1, &y1, &w1, &h1);
		tft.setCursor(x + (w / 2) - (w1 / 2) - 2, y + h - 4);
		tft.print(xname);
	}
	if (strlen(yname) > 0)
	{
		uint8_t rot = tft.getRotation();
		tft.getTextBounds(yname, 0, 0, &x1, &y1, &w1, &h1);
		tft.setRotation(rot + 3); 
		tft.setCursor((240 - ((y + h)/2)) - (w1/2) - 2, x + 2 + h1);
		tft.print(yname);
		tft.setRotation(rot);
	}
}

bool cGUI::XYCtrl::updateXY(uint16_t posix, uint16_t posiy)
{
	int16_t  x1, y1;
	uint16_t w1, h1;
	// Boundary checking
	if (posiy < y || posiy > y + h || posix < x || posix >(x + w))
		return false;

	int16_t width = 5;
	printf("minx: %i\tmaxx%i\r\n", minx, maxx);
	posix = map(posix, x, x + w, minx, maxx);
	posiy = map(posiy, y, y + h, miny, maxy);
	//		printf("pos: %i\tx: %i\tx+w: %i\tmin: %i\tmax: %i\txpos: %i\r\n",
	//			pos, x, x + w, min, max, map(pos,min,max,0,w));

	uint16_t xpos = map(posix, minx, maxx, 0, w) - width / 2;
	uint16_t oldx = map(xval, minx, maxx, 0, w) - width / 2;
	uint16_t ypos = map(posiy, miny, maxy, 0, h) - width / 2;
	uint16_t oldy = map(yval, miny, maxy, 0, h) - width / 2;

	if ((xval >= 0 && posix != xval) || (yval >= 0 && posiy != yval))
	{
//		printf("Xpos: %i\tYpos: %i\r\n", posix, maxy - posiy);
		uint16_t pos[2] = { posix, (uint16_t)maxy-posiy };
		uint16_t minp[2] = { minx, miny };
		uint16_t maxp[2] = { maxx, maxy };
		for (uint8_t i = 0; i < 2; i++)
		{
			if (type[i] == 0)
			{
				if (pos[i] > 0x7F)
				{
					pos[i] = map(pos[i], minp[i], maxp[i], 0, 127);
				}
				printf("%i MIDI CC: %i value %i\r\n", i, ctrl[i], pos[i]);
			}
			if (type[i] == 1)
			{
				printf("%i MIDI NRPN: %i value %i\r\n", i, ctrl[i], pos[i]);
			}
		}

		tft.fillCircle(x + oldx, y + oldy, width, BLACK);
		tft.drawFastHLine(x, y + oldy, w, BLACK);
		tft.drawFastVLine(x + oldx, y, h, BLACK);

		tft.drawFastHLine(x, y + ypos, w, ORANGE);
		tft.drawFastVLine(x + xpos, y, h, CYAN);
		tft.fillCircle(x + xpos, y + ypos, width, WHITE);

		tft.drawRect(x, y, w, h, WHITE);
		xval = posix;
		yval = posiy;
		if (strlen(xname) > 0)
		{
			tft.getTextBounds(xname, 0, 20, &x1, &y1, &w1, &h1);
			tft.setCursor(x + (w / 2) - (w1 / 2) - 2, y + h - 4);
			tft.print(xname);
		}
		if (strlen(yname) > 0)
		{
			uint8_t rot = tft.getRotation();
			tft.getTextBounds(yname, 0, 0, &x1, &y1, &w1, &h1);
			tft.setRotation(rot + 3);
			tft.setCursor((240 - ((y + h) / 2)) - (w1 / 2) - 2, x + 2 + h1);
			tft.print(yname);
			tft.setRotation(rot);
		}
		return true;
	}
	return false;
}

void cGUI::CtrlXY(uint16_t x, uint16_t y,
	uint16_t w, uint16_t h, 
	uint16_t xmin, uint16_t xmax, 
	uint16_t ymin, uint16_t ymax,
	uint8_t xtype, uint16_t xctrl,
	uint8_t ytype, uint16_t yctrl,
	const char *xname, const char *yname)
{
//	prinft("xctrl: %i\tyctrl: %i\r\n", xctrl, yctrl);
	XYCtrl cXY;
	cXY.type[0] = xtype;
	cXY.type[1] = ytype;
	cXY.ctrl[0] = xctrl;
	cXY.ctrl[1] = yctrl;

	int8_t sb = -1;
	tft.fillScreen(BLACK);
	h_macro.SoftButton(0, NULL, NULL);
	h_macro.SoftButton(1, NULL, NULL);
	h_macro.SoftButton(2, NULL, NULL);
	h_macro.SoftButton(3, (char*)"Exit", NULL);

	cXY.createXY(x, y, w, h, xmin, xmax, ymin, ymax, xname, yname);

	while (true)
	{
		tud_task();

		doSoftButtons();
		if ( touchPosition())
		{
//			sb = h_macro.hitSoftButton(getXpos(), getYpos());
			cXY.updateXY(getXpos(), getYpos());
		}
		//if (!debouncer.read(PIN_SB0) || sb == 0)
		//{
		//	break;
		//}
		if (!debouncer.read(PIN_SB3) /* || sb == 3*/) // disable screenbutton on xy controls
			break;
	}
}