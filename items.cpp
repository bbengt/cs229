#include <string>
#include <cstring>
using namespace std;

#include "descriptions.h"
#include "dungeon.h"

class Item {
	public:
		item_description item_desc;
		uint32_t x, y;
		uint32_t color;
};