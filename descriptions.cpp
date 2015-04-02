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
    /* This could definately be more concise. */
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
  std::string file;
  std::ifstream f;
  std::vector<monster_description> *v;
  uint32_t retval;

  retval = 0;

  file = getenv("HOME");
  if (file.length() == 0) {
    file = ".";
  }
  file += std::string("/") + SAVE_DIR + "/" + MONSTER_DESC_FILE;

  f.open(file.c_str());

  v = new std::vector<monster_description>();
  d->monster_descriptions = v;

  if (parse_monster_descriptions(f, d, v)) {
    delete v;
    d->monster_descriptions = 0;
    retval = 1;
  }

  f.close();

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

uint32_t destroy_descriptions(dungeon_t *d)
{
  delete (std::vector<monster_description> *) d->monster_descriptions;
  d->monster_descriptions = NULL;

  return 0;
}
