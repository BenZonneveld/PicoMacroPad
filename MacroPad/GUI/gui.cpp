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

void cGUI::SlideCtrl::createSlider(uint16_t xp, uint16_t yp,
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

void cGUI::SlideCtrl::updateSlider(uint16_t posix, uint16_t posiy)
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

void cGUI::SlideCtrl::setValue(uint16_t val)
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

void cGUI::brightness()
{
	SlideCtrl slider;
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
	char *xname, char *yname)
{
	x = xpos;
	y = ypos;
	w = tw;
	h = th;
	minx = xmin;
	maxx = xmax;
	miny = ymin;
	maxy = ymax;
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
		tft.setRotation(rot + 3);
		tft.getTextBounds(yname, 0, 20, &x1, &y1, &w1, &h1);
		tft.setCursor((y + (h / 2)) + (w1/2), x + 4 + h1);
		tft.print(yname);
		tft.setRotation(rot);
	}
}

void cGUI::XYCtrl::updateXY(uint16_t posix, uint16_t posiy)
{
	// Boundary checking
	if (posiy < y || posiy > y + h || posix < x || posix >(x + w))
		return;

	int16_t width = 5;

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
		tft.fillCircle(x + oldx, y + oldy, width, BLACK);
		tft.drawFastHLine(x, y + oldy, w, BLACK);
		tft.drawFastVLine(x + oldx, y, h, BLACK);

		tft.drawFastHLine(x, y + ypos, w, ORANGE);
		tft.drawFastVLine(x + xpos, y, h, CYAN);
		tft.fillCircle(x + xpos, y + ypos, width, WHITE);

		tft.drawRect(x, y, w, h, WHITE);
		xval = posix;
		yval = posiy;
	}
}

void cGUI::test()
{
	XYCtrl cXY;
	int8_t sb = -1;
	tft.fillScreen(BLACK);
	h_macro.SoftButton(0, (char*)"Cancel", NULL);
	h_macro.SoftButton(1, NULL, NULL);
	h_macro.SoftButton(2, NULL, NULL);
	h_macro.SoftButton(3, (char*)"OK", NULL);

	cXY.createXY(20, 10, 270, 200, 0, 127, 0, 127, (char*)"X", (char*)"Y");

	while (true)
	{
		if ( touchPosition())
		{
			cXY.updateXY(getXpos(), getYpos());
			sb = h_macro.hitSoftButton(getXpos(), getYpos());			
		}
		if (!debouncer.read(PIN_SB0) || sb == 0)
		{
			break;
		}
		if (!debouncer.read(PIN_SB3) || sb == 3)
			break;
	}
}