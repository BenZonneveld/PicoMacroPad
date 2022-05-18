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

class CMacro {
public:
	CMacro();
	void init();
	//	void getCurrentPage();
	static void NextPage();
	static void PrevPage();
	bool loadJSON();
	static void getPage(uint8_t pageNum);
	bool checkHit(uint16_t xpos, uint16_t ypos);
	char* getPageName() { return pagename; }
	static void SoftButton(uint8_t pos, char* sbname, void(*action)());
	static void SoftButton(uint8_t pos);
	int8_t hitSoftButton(uint16_t xpoint, uint16_t ypoint);
private:
	static bool isInside(uint16_t x, uint16_t y, uint16_t sz, uint16_t xpoint, uint16_t ypoint);
	static void CreateShortCut(uint8_t pos, const char* icon, const char* name);
	static void Upper(char* temp);
	static Document doc;
	static ParseResult prOk;
	static Value buttons;
	static uint8_t page;
	static uint8_t max_page;
	static char pagename[64];
	static char sbnames[4][10];
	std::unordered_map<std::string, std::uint8_t> metakeys;

	static std::array<void(*)(), 4>mButtonCallback;
	//	void ( * mButtonCallback[4]()) = {nullptr};
	//	void* mButton1Callback = nullptr;
	//	void* mButton2Callback = nullptr;
	//	void* mButton3Callback = nullptr;
};


