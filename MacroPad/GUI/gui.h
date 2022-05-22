#ifndef _CLASS_GUI_H
#define _CLASS_GUI_H

class cGUI {
public:
	cGUI() { Xpoint0 = 0; Ypoint0 = 0xffff; };
	bool touchPosition();
	uint16_t getXpos() { return Xpoint0; }
	uint16_t getYpos() { return Ypoint0; }
	void brightness();
	void CtrlXY(uint16_t x, uint16_t y,
		uint16_t w, uint16_t h,
		uint16_t xmin, uint16_t xmax,
		uint16_t ymin, uint16_t ymax,
		uint8_t xtype, uint16_t xctrl,
		uint8_t ytype, uint16_t yctrl,
		const char *xname, const char *yname);

	//	uint16_t getSlideCtrlValue();
	class SlideHCtrl {
	public:
		SlideHCtrl() { x = 0; y = 0; w = 0; h = 0; min = 0; max = 0; value = -1; }
		uint16_t getValue() { return value; }
		void createSlider(uint16_t x, uint16_t y,
			uint16_t w, uint16_t h,
			uint16_t min, uint16_t max,
			char* name
		);
		//	void destroySlideCtrl();
		void updateSlider(uint16_t posix, uint16_t posiy);
		void setValue(uint16_t val);
	private:
		uint16_t x, y, w, h, min, max, value, oldwidth;
	};
	class SlideVCtrl {
	public:
		SlideVCtrl() { x = 0; y = 0; w = 0; h = 0; min = 0; max = 0; value = -1; }
		uint16_t getValue() { return value; }
		void createSlider(uint16_t x, uint16_t y,
			uint16_t w, uint16_t h,
			uint16_t min, uint16_t max,
			char* name
		);
		//	void destroySlideCtrl();
		void updateSlider(uint16_t posix, uint16_t posiy);
		void setValue(uint16_t val);
	private:
		uint16_t x, y, w, h, min, max, value, oldheight;
	};
	class XYCtrl {
	public:
		XYCtrl() { x = 0; y = 0; w = 0; h = 0; minx = 0; maxx = 0; miny = 0; maxy = 0; }
		void createXY(uint16_t xpos, uint16_t ypos,
			uint16_t tw, uint16_t th,
			uint16_t minx, uint16_t maxx,
			uint16_t miny, uint16_t maxy,
			const char *xname, const char *yname);
		bool updateXY(uint16_t xpos, uint16_t ypos);
		void setValue(uint16_t xval, uint16_t yval);
		void getXVal();
		void geyYval();
		uint8_t type[2];
		uint8_t ctrl[2];
	protected:
		uint16_t x, y, w, h, minx, maxx, miny, maxy, xval, yval;
		const char* xname;
		const char* yname;
	};
protected:
	uint16_t Xpoint0, Ypoint0;
};

#endif