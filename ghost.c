#include "defs.h"
#include <time.h>

GhostType* createGhost(HouseType* house) {
  GhostType* ghost;
  int roomPos = randInt(1, house->rooms.size);
  RoomNodeType* current = house->rooms.head;
  for(int i = 0; i<roomPos; i++) {
    current = current->next;
  }
  GhostClass variant = randomGhost();
  EvidenceType (*possibleEvidence)[3] = &house->variantEvidence[variant];
  initGhost(variant, possibleEvidence, current->room, &ghost);
  current->room->ghost = ghost;
  l_ghostInit(ghost->ghostVariant, current->room->name);
  return ghost;
}

void leaveEvidence(GhostType* ghost) {
  int evidenceChoice = randInt(0,EV_COUNT-1);
  EvidenceType* evidence = *(ghost->possibleEvidence)+evidenceChoice;
  addEvidence(&ghost->room->evidenceLeft, evidence);
  l_ghostEvidence(*evidence, ghost->room->name);
}

void changeRoom(GhostType* ghost){
  RoomType* oldRoom = ghost->room;
  oldRoom->ghost = NULL;
  int roomToChoose = randInt(0, oldRoom->neighbors.size);
  RoomNodeType* currentRoom = oldRoom->neighbors.head;
  for(int i = 0; i<roomToChoose; i++){
    currentRoom = currentRoom->next;
  }
  ghost->room = currentRoom->room;
  currentRoom->room->ghost = ghost;
  l_ghostMove(currentRoom->room->name);
}

void* ghostActivity(void* voidGhost) {
  GhostType* ghost = (GhostType*)voidGhost;
  enum GhostDecisions {DO_NOTHING, LEAVE_EVIDENCE, MOVE, DECISION_COUNT};
  while(ghost->boredom < BOREDOM_MAX) {
    int hunterInRoom = ghost->room->hunterCount > 0;
    int choice;
    if(hunterInRoom) {
      choice = randInt(0, DECISION_COUNT-1); // prevent move option
      ghost->boredom = 0;      
    } 
    else{
      choice = randInt(0, DECISION_COUNT);
      ghost->boredom++;     
    } 
    if(choice== LEAVE_EVIDENCE) leaveEvidence(ghost);
    else if (choice == MOVE) changeRoom(ghost);
    // else printf("Ghost does nothing...\n");
    // sleep(1);
  }
  l_ghostExit(LOG_BORED);
  return NULL;
}