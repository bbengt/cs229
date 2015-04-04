#include <string>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <limits.h>
#include <ncurses.h>
#include <vector>
#include <sstream>
#include <cstdlib>

#include "descriptions.h"
#include "dungeon.h"
#include "npc.h"
#include "dice.h"

#define MONSTER_FILE_SEMANTIC          "RLG229 MONSTER DESCRIPTION"
#define MONSTER_FILE_VERSION           1U
#define NUM_MONSTER_DESCRIPTION_FIELDS 8
#define NUM_ITEM_DESCRIPTION_FIELDS 8
#define ITEM_FILE_SEMANTIC             "RLG229 OBJECT DESCRIPTION"
#define ITEM_FILE_VERSION               1

static const struct {
  const char *name;
  const uint32_t value;
} abilities_lookup[] = {
  /* There are only 32 bits available.  So far we're only using four *
   * of them (but even if we were using all 32, that's a very small  *
   * number), so it's much more efficient to do a linear search      *
   * rather than a binary search.  Zeros on the end are sentinals to *
   * stop the search.  Two notes: 1) Performance isn't a big deal    *
   * here, since this is initialization, not gameplay; and           *
   * 2) Alphabetizing these just because.                            */
  { "PASS",   NPC_PASS_WALL },
  { "SMART",  NPC_SMART     },
  { "TELE",   NPC_TELEPATH  },
  { "TUNNEL", NPC_TUNNEL    },
  { 0,        0             }
};

static const struct {
  const char *name;
  const uint32_t value;
} type_lookup[] = {

  { "WEAPON",     ITEM_WEAPON     },
  { "OFFHAND",    ITEM_OFFHAND    },
  { "RANGED",     ITEM_RANGED     },
  { "ARMOR",      ITEM_ARMOR      },
  { "HELMET",     ITEM_HELMET     },
  { "CLOAK",      ITEM_CLOAK      },
  { "GLOVES",     ITEM_GLOVES     },
  { "BOOTS",      ITEM_BOOTS      },
  { "RING",       ITEM_RING       },
  { "AMULET",     ITEM_AMULET     },
  { "LIGHT",      ITEM_LIGHT      },
  { "SCROLL",     ITEM_SCROLL     },
  { "BOOK",       ITEM_BOOK       },
  { "FLASK",      ITEM_FLASK      },
  { "GOLD",       ITEM_GOLD       },
  { "AMMUNITION", ITEM_AMMO       },
  { "FOOD",       ITEM_FOOD       },
  { "WAND",       ITEM_WAND       },
  { "CONTAINER",  ITEM_CONTAINER  },
  { 0,            0               }
}; 

#define color_lu_entry(color) { #color, COLOR_##color }
static const struct {
  const char *name;
  const uint32_t value;
} colors_lookup[] = {
  /* Same deal here as above in abilities_lookup definition. */
  /* We can use this convenient macro here, but we can't use a *
   * similar macro above because of PASS and TELE.             */
  color_lu_entry(BLACK),
  color_lu_entry(BLUE),
  color_lu_entry(CYAN),
  color_lu_entry(GREEN),
  color_lu_entry(MAGENTA),
  color_lu_entry(RED),
  color_lu_entry(WHITE),
  color_lu_entry(YELLOW),
  { 0, 0 }
};

static inline void eat_whitespace(std::ifstream &f)
{
  while (isspace(f.peek())) {
    f.get();
  }  
}

static inline void eat_blankspace(std::ifstream &f)
{
  while (isblank(f.peek())) {
    f.get();
  }  
}

static uint32_t parse_dice(std::ifstream &f,
                           std::string *lookahead,
                           dice *d)
{
  int32_t base;
  uint32_t number, sides;

  eat_blankspace(f);

  if (f.peek() == '\n') {
    return 1;
  }

  f >> *lookahead;

  if (sscanf(lookahead->c_str(), "%d+%ud%u", &base, &number, &sides) != 3) {
    return 1;
  }

  d->set(base, number, sides);

  f >> *lookahead;

  return 0;
}

static uint32_t parse_item_name(std::ifstream &f,
                                std::string *lookahead,
                                std::string *name) 
{

  eat_blankspace(f);

  if(f.peek() == '\n') {
    return 1;
  }

  getline(f, *name);

  f >> *lookahead;

  return 0;
}

static uint32_t parse_item_desc(std::ifstream &f,
                                   std::string *lookahead,
                                   std::string *desc)
{
  /* DESC is special.  Data doesn't follow on the same line *
   * as the keyword, so we want to eat the newline, too.    */
  eat_blankspace(f);

  if (f.peek() != '\n') {
    return 1;
  }

  f.get();

  while (f.peek() != EOF) {
    getline(f, *lookahead);
    if (lookahead->length() > 77) {
      return 1;
    }

    lookahead->push_back('\n');

    if (*lookahead == ".\n") {
      break;
    }

    *desc += *lookahead;
  }

  /* Strip off the trailing newline */
  desc->erase(desc->length() - 1);

  if (*lookahead != ".\n") {
    return 1;
  }

  f >> *lookahead;

  return 0;
}

static uint32_t parse_item_type(std::ifstream &f,
                                std::string *lookahead,
                                uint32_t *type)
{

 uint32_t i;

  *type = 0;

  eat_blankspace(f);

  if (f.peek() == '\n') {
    return 1;
  }

  /* Will not lead to error if an ability is listed multiple times. */
  while (f.peek() != '\n') {
    f >> *lookahead;

    for (i = 0; type_lookup[i].name; i++) {
      if (*lookahead == type_lookup[i].name) {
        *type |= type_lookup[i].value;
        break;
      }
    }

    if (!type_lookup[i].name) {
      return 1;
    }

    eat_blankspace(f);
  }

  f >> *lookahead;

  return 0;
}

static uint32_t parse_item_color(std::ifstream &f,
                                    std::string *lookahead,
                                    uint32_t *color)
{
  uint32_t i;

  *color = UINT_MAX;

  eat_blankspace(f);

  if (f.peek() == '\n') {
    return 1;
  }

  f >> *lookahead;

  for (i = 0; colors_lookup[i].name; i++) {
    if (*lookahead == colors_lookup[i].name) {
      *color = colors_lookup[i].value;
      break;
    }
  }

  if (!colors_lookup[i].name) {
    return 1;
  }

  eat_blankspace(f);
  if (f.peek() != '\n') {
    return 1;
  }

  f >> *lookahead;

  return 0;
}

static uint32_t parse_item_hit(std::ifstream &f,
                               std::string *lookahead,
                               dice *hit)
{

  return parse_dice(f, lookahead, hit);
}

static uint32_t parse_item_dam(std::ifstream &f,
                               std::string *lookahead,
                               dice *dam)
{

  return parse_dice(f, lookahead, dam);
}

static uint32_t parse_item_dodge(std::ifstream &f,
                                 std::string *lookahead,
                                 dice *dodge)
{

  return parse_dice(f, lookahead, dodge);
}

static uint32_t parse_item_def(std::ifstream &f,
                               std::string *lookahead,
                               dice *def)
{

  return parse_dice(f, lookahead, def);
}

static uint32_t parse_item_weight(std::ifstream &f,
                                  std::string *lookahead,
                                  dice *weight)
{

  return parse_dice(f, lookahead, weight);
}

static uint32_t parse_item_speed(std::ifstream &f,
                                 std::string *lookahead,
                                 dice *speed)
{

  return parse_dice(f, lookahead, speed);
}

static uint32_t parse_item_attr(std::ifstream &f,
                                std::string *lookahead,
                                dice *attr)
{

  return parse_dice(f, lookahead, attr);
}

static uint32_t parse_item_val(std::ifstream &f,
                               std::string *lookahead,
                               dice *val)
{

  return parse_dice(f, lookahead, val);
}

static uint32_t parse_item_description(std::ifstream &f,
                                        std::string *lookahead,
                                        std::vector<item_description> *v) 
{

  std::string s;

  bool read_name, read_type, read_color, read_weight, read_hit, read_dam, read_attr, read_val, read_dodge, read_def, read_speed, read_desc;
  std::string name, desc;
  uint32_t color, type;
  dice speed, dam, hit, weight, attr, dodge, def, val;
  item_description i_desc;
  int count;

  read_name = read_type = read_color = read_weight = read_hit = read_dam = read_attr = read_val = read_dodge = read_def = read_speed = read_desc = false;

  if (*lookahead != "BEGIN") {
    std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
              << "Parse error in item description.\n"
              << "Discarding item." << std::endl;
    do {
      f >> *lookahead;
    } while (*lookahead != "BEGIN" && f.peek() != EOF);
  }
  if (f.peek() == EOF) {
    return 1;
  }
  f >> *lookahead;
  if (*lookahead != "OBJECT") {
    return 1;
  }

  for (f >> *lookahead, count = 0;
       count < NUM_ITEM_DESCRIPTION_FIELDS;
       count++) {
    /* This could definitely be more concise. */
    if        (*lookahead == "NAME")  {
      if (read_name || parse_item_name(f, lookahead, &name)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item name.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_name = true;
    } else if (*lookahead == "DESC")  {
      if (read_desc || parse_item_desc(f, lookahead, &desc)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item description.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_desc = true;
    } else if (*lookahead == "TYPE")  {
      if (read_type || parse_item_type(f, lookahead, &type)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item type.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_type = true;
    } else if (*lookahead == "COLOR") {
      if (read_color || parse_item_color(f, lookahead, &color)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item color.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_color = true;
    } else if (*lookahead == "SPEED") {
      if (read_speed || parse_item_speed(f, lookahead, &speed)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item speed.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_speed = true;
    } else if (*lookahead == "WEIGHT")  {
      if (read_weight || parse_item_weight(f, lookahead, &weight)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item weight.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_weight = true;
    } else if (*lookahead == "HIT")    {
      if (read_hit || parse_item_hit(f, lookahead, &hit)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item hitpoints.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_hit = true;
    } else if (*lookahead == "DAM")   {
      if (read_dam || parse_item_dam(f, lookahead, &dam)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item damage.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_dam = true;
    } else if (*lookahead == "ATTR")   {
      if (read_attr || parse_item_attr(f, lookahead, &attr)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item attributes.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_dam = true;
    } else if (*lookahead == "DODGE")   {
      if (read_dodge || parse_item_dodge(f, lookahead, &dodge)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item dodge.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_dodge = true;
    } else if (*lookahead == "DEF")   {
      if (read_def || parse_item_def(f, lookahead, &def)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item defense.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_def = true;
    } else if (*lookahead == "VAL")   {
      if (read_val || parse_item_val(f, lookahead, &val)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in item value.\n"
                  << "Discarding item." << std::endl;
        return 1;
      }
      read_val = true;
   }  else                           {
      std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                << "Parse error in item description.\n"
                << "Discarding item." << std::endl;
      return 1;
    }
  }

  if (*lookahead != "END") {
    return 1;
  }

  eat_blankspace(f);
  if (f.peek() != '\n' && f.peek() != EOF) {
    return 1;
  }
  f >> *lookahead;

  i_desc.set(name, desc, type, color, attr, speed, hit, dam, weight, dodge, def, val);
  v->push_back(i_desc);

  return 0;
}

static uint32_t parse_item_descriptions(std::ifstream &f,
                                           dungeon_t *d,
                                           std::vector<item_description> *v)
{
  std::string s;
  std::stringstream expected;
  std::string lookahead;

  expected << ITEM_FILE_SEMANTIC << " " << ITEM_FILE_VERSION;

  eat_whitespace(f);

  getline(f, s);

  if (s != expected.str()) {
    std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
              << "Parse error in item description file.\nExpected: \""
              << expected.str() << "\"\nRead:     \"" << s << "\"\n\nAborting."
              << std::endl;
    return 1;
  }

  f >> lookahead;
  do {
    parse_item_description(f, &lookahead, v);
  } while (f.peek() != EOF);

  return 0;
}

static uint32_t parse_monster_name(std::ifstream &f,
                                   std::string *lookahead,
                                   std::string *name)
{
  /* Always start by eating the blanks.  If we then find a newline, we *
   * know there's an error in the file.  If we eat all whitespace,     *
   * we'd consume newlines and perhaps miss a restart on the next      *
   * line.                                                             */

  eat_blankspace(f);

  if (f.peek() == '\n') {
    return 1;
  }

  getline(f, *name);

  /* We enter this function with the semantic in the lookahead, so we  *
   * read a new one so that we're in the same state for the next call. */
  f >> *lookahead;

  return 0;
}

static uint32_t parse_monster_symb(std::ifstream &f,
                                   std::string *lookahead,
                                   char *symb)
{
  eat_blankspace(f);

  if (f.peek() == '\n') {
    return 1;
  }

  *symb = f.get();

  eat_blankspace(f);
  if (f.peek() != '\n') {
    return 1;
  }

  f >> *lookahead;

  return 0;
}

static uint32_t parse_monster_color(std::ifstream &f,
                                    std::string *lookahead,
                                    uint32_t *color)
{
  uint32_t i;

  *color = UINT_MAX;

  eat_blankspace(f);

  if (f.peek() == '\n') {
    return 1;
  }

  f >> *lookahead;

  for (i = 0; colors_lookup[i].name; i++) {
    if (*lookahead == colors_lookup[i].name) {
      *color = colors_lookup[i].value;
      break;
    }
  }

  if (!colors_lookup[i].name) {
    return 1;
  }

  eat_blankspace(f);
  if (f.peek() != '\n') {
    return 1;
  }

  f >> *lookahead;

  return 0;
}

static uint32_t parse_monster_desc(std::ifstream &f,
                                   std::string *lookahead,
                                   std::string *desc)
{
  /* DESC is special.  Data doesn't follow on the same line *
   * as the keyword, so we want to eat the newline, too.    */
  eat_blankspace(f);

  if (f.peek() != '\n') {
    return 1;
  }

  f.get();

  while (f.peek() != EOF) {
    getline(f, *lookahead);
    if (lookahead->length() > 77) {
      return 1;
    }

    lookahead->push_back('\n');

    if (*lookahead == ".\n") {
      break;
    }

    *desc += *lookahead;
  }

  /* Strip off the trailing newline */
  desc->erase(desc->length() - 1);

  if (*lookahead != ".\n") {
    return 1;
  }

  f >> *lookahead;

  return 0;
}

static uint32_t parse_monster_speed(std::ifstream &f,
                                    std::string *lookahead,
                                    dice *speed)
{
  return parse_dice(f, lookahead, speed);
}

static uint32_t parse_monster_dam(std::ifstream &f,
                                  std::string *lookahead,
                                  dice *dam)
{
  return parse_dice(f, lookahead, dam);
}

static uint32_t parse_monster_hp(std::ifstream &f,
                                 std::string *lookahead,
                                 dice *hp)
{
  return parse_dice(f, lookahead, hp);
}

static uint32_t parse_monster_abil(std::ifstream &f,
                                   std::string *lookahead,
                                   uint32_t *abil)
{
  uint32_t i;

  *abil = 0;

  eat_blankspace(f);

  if (f.peek() == '\n') {
    return 1;
  }

  /* Will not lead to error if an ability is listed multiple times. */
  while (f.peek() != '\n') {
    f >> *lookahead;

    for (i = 0; abilities_lookup[i].name; i++) {
      if (*lookahead == abilities_lookup[i].name) {
        *abil |= abilities_lookup[i].value;
        break;
      }
    }

    if (!abilities_lookup[i].name) {
      return 1;
    }

    eat_blankspace(f);
  }

  f >> *lookahead;

  return 0;
}

static uint32_t parse_monster_description(std::ifstream &f,
                                          std::string *lookahead,
                                          std::vector<monster_description> *v)
{
  std::string s;
  bool read_name, read_symb, read_color, read_desc,
       read_speed, read_dam, read_hp, read_abil;
  std::string name, desc;
  char symb;
  uint32_t color, abil;
  dice speed, dam, hp;
  monster_description m;
  int count;

  read_name = read_symb = read_color = read_desc =
              read_speed = read_dam = read_hp = read_abil = false;

  if (*lookahead != "BEGIN") {
    std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
              << "Parse error in monster description.\n"
              << "Discarding monster." << std::endl;
    do {
      f >> *lookahead;
    } while (*lookahead != "BEGIN" && f.peek() != EOF);
  }
  if (f.peek() == EOF) {
    return 1;
  }
  f >> *lookahead;
  if (*lookahead != "MONSTER") {
    return 1;
  }

  for (f >> *lookahead, count = 0;
       count < NUM_MONSTER_DESCRIPTION_FIELDS;
       count++) {
    /* This could definitely be more concise. */
    if        (*lookahead == "NAME")  {
      if (read_name || parse_monster_name(f, lookahead, &name)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in monster name.\n"
                  << "Discarding monster." << std::endl;
        return 1;
      }
      read_name = true;
    } else if (*lookahead == "DESC")  {
      if (read_desc || parse_monster_desc(f, lookahead, &desc)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in monster description.\n"
                  << "Discarding monster." << std::endl;
        return 1;
      }
      read_desc = true;
    } else if (*lookahead == "SYMB")  {
      if (read_symb || parse_monster_symb(f, lookahead, &symb)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in monster symbol.\n"
                  << "Discarding monster." << std::endl;
        return 1;
      }
      read_symb = true;
    } else if (*lookahead == "COLOR") {
      if (read_color || parse_monster_color(f, lookahead, &color)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in monster color.\n"
                  << "Discarding monster." << std::endl;
        return 1;
      }
      read_color = true;
    } else if (*lookahead == "SPEED") {
      if (read_speed || parse_monster_speed(f, lookahead, &speed)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in monster speed.\n"
                  << "Discarding monster." << std::endl;
        return 1;
      }
      read_speed = true;
    } else if (*lookahead == "ABIL")  {
      if (read_abil || parse_monster_abil(f, lookahead, &abil)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in monster abilities.\n"
                  << "Discarding monster." << std::endl;
        return 1;
      }
      read_abil = true;
    } else if (*lookahead == "HP")    {
      if (read_hp || parse_monster_hp(f, lookahead, &hp)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in monster hitpoints.\n"
                  << "Discarding monster." << std::endl;
        return 1;
      }
      read_hp = true;
    } else if (*lookahead == "DAM")   {
      if (read_dam || parse_monster_dam(f, lookahead, &dam)) {
        std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                  << "Parse error in monster damage.\n"
                  << "Discarding monster." << std::endl;
        return 1;
      }
      read_dam = true;
    } else                           {
      std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
                << "Parse error in monster description.\n"
                << "Discarding monster." << std::endl;
      return 1;
    }
  }

  if (*lookahead != "END") {
    return 1;
  }

  eat_blankspace(f);
  if (f.peek() != '\n' && f.peek() != EOF) {
    return 1;
  }
  f >> *lookahead;

  m.set(name, desc, symb, color, speed, abil, hp, dam);
  v->push_back(m);

  return 0;
}

static uint32_t parse_monster_descriptions(std::ifstream &f,
                                           dungeon_t *d,
                                           std::vector<monster_description> *v)
{
  std::string s;
  std::stringstream expected;
  std::string lookahead;

  expected << MONSTER_FILE_SEMANTIC << " " << MONSTER_FILE_VERSION;

  eat_whitespace(f);

  getline(f, s);

  if (s != expected.str()) {
    std::cerr << "Discovered at " << __FILE__ << ":" << __LINE__ << "\n"
              << "Parse error in monster description file.\nExpected: \""
              << expected.str() << "\"\nRead:     \"" << s << "\"\n\nAborting."
              << std::endl;
    return 1;
  }

  f >> lookahead;
  do {
    parse_monster_description(f, &lookahead, v);
  } while (f.peek() != EOF);

  return 0;
}

uint32_t parse_descriptions(dungeon_t *d)
{
  std::string monster_file, item_file;
  std::ifstream mon_f, item_f;
  std::vector<monster_description> *mon_v;
  std::vector<item_description> *item_v;
  uint32_t retval;

  retval = 0;

  monster_file = getenv("HOME");
  if (monster_file.length() == 0) {
    monster_file = ".";
  }
  monster_file += std::string("/") + SAVE_DIR + "/" + MONSTER_DESC_FILE;

  mon_f.open(monster_file.c_str());

  mon_v = new std::vector<monster_description>();
  d->monster_descriptions = mon_v;

  if (parse_monster_descriptions(mon_f, d, mon_v)) {
    delete mon_v;
    d->monster_descriptions = 0;
    retval = 1;
  }

  mon_f.close();

  // If we've already hit an error, stop before reading item descriptions
  if(retval == 1) {
    return retval;
  }

  item_file = getenv("HOME");
  if (item_file.length() == 0) {
    item_file = ".";
  }
  item_file += std::string("/") + SAVE_DIR + "/" + ITEM_DESC_FILE;

  item_f.open(item_file.c_str());

  item_v = new std::vector<item_description>();
  d->item_descriptions = item_v;

  if(parse_item_descriptions(item_f, d, item_v)) {
    delete item_v;
    retval = 1;
  }

  item_f.close();

  return retval;
}

uint32_t print_descriptions(dungeon_t *d)
{
  std::vector<monster_description> *v;
  std::vector<monster_description>::iterator i;

  v = (std::vector<monster_description> *) d->monster_descriptions;

  for (i = v->begin(); i != v->end(); i++) {
    i->print(std::cout);
  }

  return 0;
}

void monster_description::set(const std::string &name,
                              const std::string &description,
                              const char symbol,
                              const uint32_t color,
                              const dice &speed,
                              const uint32_t abilities,
                              const dice &hitpoints,
                              const dice &damage)
{
  this->name = name;
  this->description = description;
  this->symbol = symbol;
  this->color = color;
  this->speed = speed;
  this->abilities = abilities;
  this->hitpoints = hitpoints;
  this->damage = damage;
}

void item_description::set(const std::string &name,
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
         const dice &val)
{
  this->name = name;
  this->description = description;
  this->type = type;
  this->color = color;
  this->attributes = attributes;
  this->speed = speed;
  this->hit = hit;
  this->damage = damage;
  this->weight = weight;
  this->dodge = dodge;
  this->def = def;
  this->val = val;
}

std::ostream &monster_description::print(std::ostream& o)
{
  uint32_t i;

  o << name << std::endl;
  o << description << std::endl;
  o << symbol << std::endl;
  for (i = 0; colors_lookup[i].name; i++) {
    if (color == colors_lookup[i].value) {
      o << colors_lookup[i].name << std::endl;
      break;
    }
  }
  speed.print(o);
  o << std::endl;
  for (i = 0; abilities_lookup[i].name; i++) {
    if (abilities & abilities_lookup[i].value) {
      o << abilities_lookup[i].name << " ";
    }
  }
  o << std::endl;
  hitpoints.print(o);
  o << std::endl;
  damage.print(o);
  o << std::endl;
  o << std::endl;

  return o;
}

std::ostream &item_description::print(std::ostream& o)
{
  uint32_t i;

  o << name << std::endl;
  o << description << std::endl;
  o << type << std::endl;
  for (i = 0; colors_lookup[i].name; i++) {
    if (color == colors_lookup[i].value) {
      o << colors_lookup[i].name << std::endl;
      break;
    }
  }
  attributes.print(o);
  o << std::endl;
  speed.print(o);
  o << std::endl;
  hit.print(o);
  o << std::endl;
  damage.print(o);
  o << std::endl;
  weight.print(o);
  o << std::endl;
  dodge.print(o);
  o << std::endl;
  def.print(o);
  o << std::endl;
  val.print(o);
  o << std::endl;
  o << std::endl;

  return o;
}

uint32_t destroy_descriptions(dungeon_t *d)
{
  delete (std::vector<monster_description> *) d->monster_descriptions;
  d->monster_descriptions = NULL;

  return 0;
}
