#include "defs.h"

GhostType* createGhost(HouseType* house) {
  GhostType* ghost;
  int roomPos = randInt(1, house->rooms.size-1);
  RoomNodeType* current = house->rooms.head;
  for(int i = 0; i<roomPos; i++) {
    current = current->next;
  }
  initGhost(randomGhost(), current->room, &ghost);
  current->room->ghost = ghost;
  l_ghostInit(ghost->ghostVariant, current->room);
  return ghost;
}

void leaveEvidence(GhostType* ghost) {
  static EvidenceType variantEvidence[4][3] = {{EMF,TEMPERATURE,FINGERPRINTS},{EMF,TEMPERATURE,SOUND},{EMF,FINGERPRINTS,SOUND},
  {TEMPERATURE,FINGERPRINTS,SOUND}};
  int evidenceChoice = randInt(0,3);
  EvidenceType* evidence = &variantEvidence[ghost->ghostVariant][evidenceChoice];
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
  GhostType* ghost = (GhostType*) voidGhost;
  int madeDecision = C_FALSE;
  while(ghost->boredom < BOREDOM_MAX) {
    int huntersInRoom = ghost->room->hunterCount > 0;
    int choice = randInt(0,3);
    if(huntersInRoom) {
      ghost->boredom = 0;
    }
    else { //if no hunters in room
      ghost->boredom++;
    }
    if(choice == 0 && madeDecision == C_FALSE){//leave evidence
      leaveEvidence(ghost);
    } else if (choice == 1 && !huntersInRoom){//leave room
      changeRoom(ghost);
      madeDecision = C_FALSE;
      sleep(1);
      continue;
    } // choice == 2: do nothing
    else{
      //printf("Ghost chilling for 1 second... boredom is %d\n", ghost->boredom);
    }
    
    madeDecision = C_TRUE;
    sleep(1);
  }
  l_ghostExit(LOG_BORED);
  return NULL;
}