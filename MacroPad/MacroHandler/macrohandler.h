//#include "reader.h" // JSON Reader
//#include "filereadstream.h"
#include "document.h"

using namespace rapidjson;

#define BUTSIZE 64
#define XOFFSET 32
#define YOFFSET 32
#define XSPACING 32
#define YSPACING 56

class CMacro{
public:
	CMacro() { buttons = 0; }
	void getMacroPage(uint8_t pageNum);
	bool checkHit(uint16_t xpos, uint16_t ypos);
//	void getKeyMacro(uint8_t pos);
private:
	bool isInside(uint16_t x, uint16_t y, uint16_t sz, uint16_t xpoint, uint16_t ypoint);
	void CreateShortCut(uint8_t pos, const char* icon, const char* name);
	Document doc;
	Value buttons;
	uint8_t page;
};