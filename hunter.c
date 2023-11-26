#include "defs.h"

void createHunter(char *name, HouseType *house, EvidenceType device) {
  HunterType* hunter;
  initHunter(name, house->rooms.head->room, device, &house->sharedEvidence,&house->variantEvidence, &hunter);
  addHunter(house->rooms.head->room->hunters, hunter, &house->rooms.head->room->hunterCount);
  addHunter(house->hunters, hunter, &house->hunterCount);
  l_hunterInit(name, device);
}

void addHunter(HunterType** hunters, HunterType* hunter, int* count){
    hunters[(*count)++]=hunter;
}

void printHunter(HunterType* hunter) {
  char evidence[MAX_STR];
  evidenceToString(hunter->device, evidence);
  printf("| Name: %s | Boredom: %d | Fear: %d | Device: %s | Location: %s |\n", hunter->name, hunter->boredom, hunter->fear, evidence, hunter->room->name);
}

void* hunterActivity(void* voidHunter, void* voidHouse) {
  HunterType* hunter = (HunterType*) voidHunter;
  HouseType* house = (HouseType*) voidHouse;
  enum LoggerDetails reasonForExit = LOG_UNKNOWN;
  while(hunter->boredom < BOREDOM_MAX && hunter->fear < FEAR_MAX) {
    int choice = randInt(0, 3);
    if(hunter->room->ghost!=NULL) { // hunter is with a ghost
      hunter->fear++;
      hunter->boredom = 0;
    } else{ // hunter is not with a ghost
      hunter->boredom++;
    }
    if(choice == 0) {
      collectEvidence(hunter);
    } else if (choice==1) {
      moveHunter(hunter);
    } else {
      // review evidence
    }
    printf("Hunter is existing with boredom level %d!!!\n", hunter->boredom);
    sleep(1);
  }
  if(hunter->fear >= FEAR_MAX) reasonForExit = LOG_FEAR; 
  else if (hunter->boredom >= BOREDOM_MAX) reasonForExit = LOG_BORED;
  deleteHunter(hunter, house, reasonForExit);
  return NULL;
}

void collectEvidence(HunterType* hunter){
  int found = C_FALSE;
  EvidenceType* evidence = removeEvidence(&hunter->room->evidenceLeft, hunter->device);
  if(evidence!=NULL) {
    for(EvidenceNodeType* it = hunter->sharedEvidence->head; it!=NULL; it=it->next) {
      if(it->evidence == evidence){
        found = C_TRUE;     
        break;
      } 
    }
    if(found == C_FALSE){
      addEvidence(hunter->sharedEvidence, evidence);
    }
    l_hunterCollect(hunter->name, *evidence, hunter->room->name);
  }
  printf("No Evidence to collect.\n");
}

void removeHunter(HunterType* hunter, HunterType** hunters, int* count) {
  for(int i = 0; i < *count; i++) {
    if(hunters[i]==hunter) {
      hunters[i] = NULL;
      for(int j = i+i; j<*count; j++) {
        if(j == *count-1){
          hunters[j] = NULL;
          break;
        }
        hunters[j-1] = hunters[j];
      }
      break;
    }
  }
  (*count)--;
  hunter->room = NULL;
}
void moveHunter(HunterType* hunter){
  RoomType* oldRoom = hunter->room;
  //remove hunter from old room
  removeHunter(hunter, hunter->room->hunters, &hunter->room->hunterCount);
  int roomToChoose = randInt(0, oldRoom->neighbors.size);// pick random neighbor
  RoomNodeType* currentRoom = oldRoom->neighbors.head;
  for(int i = 0; i<roomToChoose; i++){
    currentRoom = currentRoom->next;
  }
  hunter->room = currentRoom->room;
  //add hunter
  addHunter(currentRoom->room->hunters, hunter, &currentRoom->room->hunterCount);
  l_hunterMove(hunter->name, currentRoom->room->name);
}

void deleteHunter(HunterType* hunter, HouseType* house, enum LoggerDetails reason){
  l_hunterExit(hunter->name, reason);
  removeHunter(hunter, hunter->room->hunters, &hunter->room->hunterCount);
  removeHunter(hunter, house->hunters, &house->hunterCount);
  free(hunter);
}