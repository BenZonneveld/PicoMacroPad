class cGUI {
public:
	cGUI() { Xpoint0 = 0; Ypoint0 = 0xffff; };
	bool touchPosition();
	uint16_t getXpos() { return Xpoint0; }
	uint16_t getYpos() { return Ypoint0; }
	void brightness();
	void test();
	//	uint16_t getSlideCtrlValue();
	class SlideCtrl {
	public:
		SlideCtrl() { x = 0; y = 0; w = 0; h = 0; min = 0; max = 0; value = -1; }
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
	class XYCtrl {
	public:
		XYCtrl() { x = 0; y = 0; w = 0; h = 0; minx = 0; maxx = 0; miny = 0; maxy = 0; }
		void createXY(uint16_t xpos, uint16_t ypos,
			uint16_t tw, uint16_t th,
			uint16_t minx, uint16_t maxx,
			uint16_t miny, uint16_t maxy,
			char *xname, char *yname);
		void updateXY(uint16_t xpos, uint16_t ypos);
		void setValue(uint16_t xval, uint16_t yval);
		void getXVal();
		void geyYval();
	protected:
		uint16_t x, y, w, h, minx, maxx, miny, maxy, xval, yval;
	};
protected:
	uint16_t Xpoint0, Ypoint0;
};