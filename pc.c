#include <stdlib.h>
#include <ncurses.h> /* for COLOR_WHITE */

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "io.h"
#include "object.h"
#include "dice.h"

void pc_delete(pc_t *pc)
{
  if (pc) {
    free(pc);
  }
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc.alive;
}

void place_pc(dungeon_t *d)
{
  d->pc.position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc.position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon_t *d)
{
  d->pc.symbol = '@';
  d->pc.color = COLOR_WHITE;

  place_pc(d);

  d->pc.speed = PC_SPEED;
  d->pc.next_turn = 0;
  d->pc.alive = 1;
  d->pc.sequence_number = 0;
  d->pc.pc = malloc(sizeof (*d->pc.pc));
  strncpy(d->pc.pc->name, "Isabella Garcia-Shapiro", sizeof (d->pc.pc->name));
  strncpy(d->pc.pc->catch_phrase,
          "Whatcha doin'?", sizeof (d->pc.pc->name));
  d->pc.npc = NULL;

  d->character[d->pc.position[dim_y]][d->pc.position[dim_x]] = &d->pc;

  io_calculate_offset(d);

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  dir[dim_y] = dir[dim_x] = 0;

  /* Tunnel to the nearest dungeon corner, then move around in hopes *
   * of killing a couple of monsters before we die ourself.          */

  if (in_corner(d, &d->pc)) {
    /*
    dir[dim_x] = (mapxy(d->pc.position[dim_x] - 1,
                        d->pc.position[dim_y]) ==
                  ter_wall_immutable) ? 1 : -1;
    */
    dir[dim_y] = (mapxy(d->pc.position[dim_x],
                        d->pc.position[dim_y] - 1) ==
                  ter_wall_immutable) ? 1 : -1;
  } else {
    dir_nearest_wall(d, &d->pc, dir);
  }

  return 0;
}

uint32_t equip_item(dungeon_t *d, int slot) {

  if(slot < 0 || slot > 12) {
    return 1;
  }
  if(!d->object[d->pc.position[dim_y]][d->pc.position[dim_x]]) {
    return 1;
  }

  object_t *o = d->object[d->pc.position[dim_y]][d->pc.position[dim_x]];
  if(d->pc.pc->equipment[slot] != NULL) {
    int i;
    for(i = 0; i < 12; i++) {
      if(d->pc.pc->inventory[i] != NULL) {
        d->pc.pc->inventory[i] = d->pc.pc->equipment[slot];
        break;
      }
    }
  }
  d->pc.pc->equipment[slot] = o;

  return 0;
}
uint32_t drop_item(dungeon_t *d, int slot) {

  if(slot < 0 || slot > 12) {
    return 1;
  }

  object_t *o = d->pc.pc->inventory[slot];
  d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = o;
  free(d->pc.pc->inventory[slot]);

  return 0;
}
uint32_t remove_item(dungeon_t *d, int slot) {

  if(slot < 0 || slot > 12) {
    return 1;
  }

  int i;
  for(i = 0; i < 12; i++) {
    if(d->pc.pc->inventory[i] != NULL) {
      d->pc.pc->inventory[i] = d->pc.pc->equipment[slot];
      break;
    }
    d->pc.pc->equipment[slot] = NULL;
  }

  return 0;
}
uint32_t expunge_item(dungeon_t *d, int slot) {

  if(slot < 0 || slot > 12) {
    return 1;
  }

  object_t *o = d->pc.pc->inventory[slot];
  pair_t *coords = get_coords(o);
  delete_object(d, *coords[dim_x], *coords[dim_y]);

  return 0;
}

uint32_t pc_attack_damage(dungeon_t *d) {

  int damage = 0;
  int i;
  for(i = 0; i < 12; i++) {
    damage += roll_dice_damage(d->pc.pc->inventory[i]);
  }

  return damage;
}

uint32_t process_damage(uint32_t damage, character_t *recipient) {

  recipient->hp -= damage;

  return 0;
}