#ifndef ITEMS_H
#define ITEMS_H

#include "descriptions.h"
#include "dungeon.h"

# ifdef __cplusplus

#include <string>

extern "C" {
# endif

uint32_t get_attr(dungeon_t *d, int x, int y);
uint32_t get_color(dungeon_t *d, int x, int y);
uint32_t set_color(dungeon_t *d, int x, int y, uint32_t color);

# ifdef __cplusplus
} /* extern "C" */

class Item {

private:
	uint32_t x, y; 

public:
  uint32_t cpp_get_attr();
  uint32_t cpp_get_color();
  item_description item_desc;

};

  Item to_item(item_t item_struct);

# endif
#endif