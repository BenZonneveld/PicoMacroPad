//#include "reader.h" // JSON Reader
//#include "filereadstream.h"
#include "document.h"

using namespace rapidjson;

class CMacro : public Document{
public:
	CMacro();
	Value& GetMacroPage(uint8_t pageNum);
//	void GetKeyMacro(uint8_t pageNum, uint8_t pos);
private:
	Document doc;
};