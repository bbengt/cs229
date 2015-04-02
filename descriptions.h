#ifndef DESCRIPTIONS_H
# define DESCRIPTIONS_H

# include <stdint.h>

typedef struct dungeon dungeon_t;

# ifdef __cplusplus

# include <string>
# include "dice.h"

extern "C" {
# endif

uint32_t parse_descriptions(dungeon_t *d);
uint32_t print_descriptions(dungeon_t *d);
uint32_t destroy_descriptions(dungeon_t *d);

# ifdef __cplusplus
} /* extern "C" */

class monster_description {
 private:
  std::string name, description;
  char symbol;
  uint32_t color, abilities;
  dice speed, hitpoints, damage;
 public:
  monster_description() : name(),       description(), symbol(0),   color(0),
                          abilities(0), speed(),       hitpoints(), damage()
  {
  }
  void set(const std::string &name,
           const std::string &description,
           const char symbol,
           const uint32_t color,
           const dice &speed,
           const uint32_t abilities,
           const dice &hitpoints,
           const dice &damage);
  std::ostream &print(std::ostream &o);
};

# endif

#endif
