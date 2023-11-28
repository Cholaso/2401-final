//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"

/* 
    Creates a ghost and adds it to a random room in our house
    in/out: house - The house we add the ghost to
*/
GhostType* createGhost(HouseType* house) {
  GhostType* ghost;
  int roomPos = randInt(1, house->rooms.size);
  RoomNodeType* current = house->rooms.head;
  for(int i = 0; i<roomPos; i++) {
    current = current->next;
  }
  GhostClass variant = randomGhost();
  EvidenceType (*possibleEvidence)[3] = &house->variantEvidence[variant];
  initGhost(variant, possibleEvidence, current->room, &house->mutex, &house->sufficientEvidenceFound, &ghost);
  current->room->ghost = ghost;
  l_ghostInit(ghost->ghostVariant, current->room->name);
  return ghost;
}
/* 
    leaves evidence in the current room. Allowed evidence is defined by ghost type.
    in/out: ghost - our ghost
*/
void leaveEvidence(GhostType* ghost) {
  int evidenceChoice = randInt(0,EV_COUNT-1);
  EvidenceType* evidence = *(ghost->possibleEvidence)+evidenceChoice;
  addEvidence(&ghost->room->evidenceLeft, evidence);
  l_ghostEvidence(*evidence, ghost->room->name);
}
/* 
    Ghost moves from it's current room to a random neighboring room.
    in/out: ghost - our ghost
*/
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
/* 
    Main control flow for the ghost thread. Ghost makes random decisions and all of those are handled here.
    in/out: voidGhost - a void pointer to our ghost
*/
void* ghostActivity(void* voidGhost) {
  GhostType* ghost = (GhostType*)voidGhost;
  enum GhostDecisions {DO_NOTHING, LEAVE_EVIDENCE, MOVE, DECISION_COUNT};
  while(C_TRUE){
    usleep(GHOST_WAIT);
    sem_wait(ghost->mutex);
    int hunterInRoom = ghost->room->hunterCount > 0;
    int sufficientEvidenceFound = *ghost->sufficientEvidenceFound;
    sem_post(ghost->mutex);
    int choice;
    if(hunterInRoom) {
      choice = randInt(0, DECISION_COUNT-1); // prevent move option
      ghost->boredom = 0;      
    } 
    else{
      choice = randInt(0, DECISION_COUNT);
      ghost->boredom++;     
    } 
    if(sufficientEvidenceFound==C_TRUE || ghost->boredom >= BOREDOM_MAX) break;
    if(choice== LEAVE_EVIDENCE) {
      sem_wait(ghost->mutex);
      leaveEvidence(ghost);     
      sem_post(ghost->mutex);
    }
    else if (choice == MOVE) {
      sem_wait(ghost->mutex);
      changeRoom(ghost);     
      sem_post(ghost->mutex);
    }
  }
  deleteGhost(ghost);
  return NULL;
}
/* 
    Deletes our ghost from the house.
    in/out: ghost - our ghost
*/
void deleteGhost(GhostType* ghost) {
  sem_wait(ghost->mutex);
  ghost->room->ghost = NULL;
  sem_post(ghost->mutex);
  ghost->room = NULL;
  if(ghost->boredom >= BOREDOM_MAX) l_ghostExit(LOG_BORED);
}