//#include "reader.h" // JSON Reader
//#include "filereadstream.h"
#include "document.h"
#include <unordered_map>
using namespace rapidjson;

#define BUTSIZE 64
#define XOFFSET 32
#define YOFFSET 32
#define XSPACING 32
#define YSPACING 56

class CMacro{
public:
	CMacro();
	void getMacroPage(uint8_t pageNum);
	bool checkHit(uint16_t xpos, uint16_t ypos);
private:
	bool isInside(uint16_t x, uint16_t y, uint16_t sz, uint16_t xpoint, uint16_t ypoint);
	void CreateShortCut(uint8_t pos, const char* icon, const char* name);
	void Upper(char* temp);
	Document doc;
	Value buttons;
	uint8_t page;
	std::unordered_map<std::string, std::uint8_t> metakeys;
};