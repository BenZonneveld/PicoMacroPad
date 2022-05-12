//#include "reader.h" // JSON Reader
//#include "filereadstream.h"
#include "document.h"
#include <unordered_map>
using namespace rapidjson;

#define BUTSIZE 64
#define XOFFSET 32
#define YOFFSET 32
#define XSPACING 32
#define YSPACING 26

class CMacro{
public:
	CMacro();
	void init();
	void getCurrentPage();
	void NextPage();
	void PrevPage();
	void getMacroPage(uint8_t pageNum);
	bool checkHit(uint16_t xpos, uint16_t ypos);
	char* getPageName() { return pagename; }
	void SoftButton(uint8_t pos, char* sbname);
	void SoftButton(uint8_t pos);
	int8_t hitSoftButton(uint16_t xpoint, uint16_t ypoint);
private:
	bool isInside(uint16_t x, uint16_t y, uint16_t sz, uint16_t xpoint, uint16_t ypoint);
	void CreateShortCut(uint8_t pos, const char* icon, const char* name);
	void Upper(char* temp);
	Document doc;
	Value buttons;
	uint8_t page;
	uint8_t max_page;
	char pagename[64];
	char sbnames[4][10];
	std::unordered_map<std::string, std::uint8_t> metakeys;
};