#ifndef OBJECT_H
# define OBJECT_H

# ifdef __cplusplus

# include <string>
# include "descriptions.h"
# include "dims.h"

extern "C" {
# endif

typedef struct object_t {
} object_t;

typedef struct dungeon dungeon_t;

void gen_objects(dungeon_t *d, uint32_t numobj);
char get_symbol(object_t *o);
uint32_t get_color(object_t *o);
void destroy_objects(dungeon_t *d);
void delete_object(dungeon_t *d, int x, int y);
pair_t *get_coords(object_t *o);
uint32_t roll_dice_damage(object_t *o);

# ifdef __cplusplus

} /* extern "C" */

class object {
 public:
  const std::string &name;
  const std::string &description;
  object_type_t type;
  uint32_t color;
  pair_t position;
  const dice &damage;
  int32_t hit, dodge, defence, weight, speed, attribute, value;
  object *next;
  object(const object_description &o, pair_t p, object *next);
  ~object();

  friend char get_symbol(object_t *o);
  friend uint32_t get_color(object_t *o);
  friend pair_t *get_coords(object_t *o);
  friend uint32_t roll_dice_damage(object_t *o);
};

# endif

#endif
