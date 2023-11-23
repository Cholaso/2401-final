#include "defs.h"

void createGhost(HouseType* house) {
  GhostType* ghost;
  int roomPos = randInt(1, house->rooms.size-1);
  RoomNodeType* current = house->rooms.head;
  for(int i = 0; i<roomPos; i++) {
    current = current->next;
  }
  initGhost(randomGhost(), current->room, &ghost);
  current->room->ghost = ghost;
}