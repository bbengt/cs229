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

# define ITEM_WEAPON       0x00000001
# define ITEM_OFFHAND      0x00000002
# define ITEM_RANGED       0x00000004
# define ITEM_ARMOR        0x00000008
# define ITEM_HELMET       0x00000010
# define ITEM_CLOAK        0x00000020
# define ITEM_GLOVES       0x00000040
# define ITEM_BOOTS        0x00000080
# define ITEM_RING         0x00000100
# define ITEM_AMULET       0x00000200
# define ITEM_LIGHT        0x00000400
# define ITEM_SCROLL       0x00000800
# define ITEM_BOOK         0x00001000
# define ITEM_FLASK        0x00002000
# define ITEM_GOLD         0x00004000
# define ITEM_AMMO         0x00008000
# define ITEM_FOOD         0x00010000
# define ITEM_WAND         0x00020000
# define ITEM_CONTAINER    0x00040000
# define ITEM_BIT19        0x00080000
# define ITEM_BIT20        0x00100000
# define ITEM_BIT21        0x00200000
# define ITEM_BIT22        0x00400000
# define ITEM_BIT23        0x00800000
# define ITEM_BIT24        0x01000000
# define ITEM_BIT25        0x02000000
# define ITEM_BIT26        0x04000000
# define ITEM_BIT27        0x08000000
# define ITEM_BIT28        0x10000000
# define ITEM_BIT29        0x20000000
# define ITEM_BIT30        0x40000000
# define ITEM_BIT31        0x80000000

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

class item_description {
 private:
  std::string name, description;
  uint32_t type, color;
  dice attributes, speed, hit, damage, weight, dodge, def, val;
 public:
  item_description() : name(),       description(), type(0),   color(0),
                          attributes(), speed(),       hit(), damage(), weight(),
                          dodge(), def(), val()
  {
  }
  void set(const std::string &name,
           const std::string &description,
           const uint32_t &type,
           const uint32_t color,
           const dice &attributes,
           const dice &speed,
           const dice &hit,
           const dice &damage,
           const dice &weight,
           const dice &dodge,
           const dice &def,
           const dice &val);
  std::ostream &print(std::ostream &o);
};

# endif

#endif
