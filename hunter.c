#include "defs.h"

// WE DO NOT REMOVE HUNTERS FROM THE HOUSE UNTIL THE VERY END, SEE 4.2

void createHunter(char *name, HouseType *house, EvidenceType device) {
  HunterType* hunter;
  RoomType* van = house->rooms.head->room;
  initHunter(name, van, device, &house->sharedEvidence,&house->variantEvidence, &hunter);
  addHunter(hunter, van);
  house->hunters[(house->hunterCount)++] = hunter;
  l_hunterInit(name, device);
}

void addHunter(HunterType* hunter, RoomType* room){
    room->hunters[(room->hunterCount)++] = hunter;
    hunter->room = room;
}

void removeHunter(HunterType* hunter) {
  HunterType** arr = hunter->room->hunters;
  int size = hunter->room->hunterCount, i, j;
  for(i = 0; i < size; i++) {
    if(arr[i]==hunter) { // hunter found
      for(j = i+1; j < size && arr[j]!=NULL; j++) { //shift back the array
        arr[j-1] = arr[j];
      }
      arr[j-1] = NULL; // remove last item
      break;
    }
  }
  hunter->room->hunterCount--;
  hunter->room = NULL;
}

void moveHunter(HunterType* hunter){
  RoomType* oldRoom = hunter->room;
  removeHunter(hunter); //remove hunter from old room
  int roomToChoose = randInt(0, oldRoom->neighbors.size);// pick random neighbor
  RoomNodeType* currentRoom = oldRoom->neighbors.head;
  for(int i = 0; i<roomToChoose; i++){
    currentRoom = currentRoom->next;
  }
  //add hunter
  addHunter(hunter, currentRoom->room);
  l_hunterMove(hunter->name, currentRoom->room->name);
}

void printHunter(HunterType* hunter) {
  char evidence[MAX_STR];
  evidenceToString(hunter->device, evidence);
  printf("| Name: %s | Boredom: %d | Fear: %d | Device: %s | Location: %s |\n", hunter->name, hunter->boredom, hunter->fear, evidence, hunter->room->name);
}

int reviewEvidence(HunterType* hunter) {
  if(hunter->sharedEvidence->size < 3) return C_FALSE;
  return C_TRUE;
}
void* hunterActivity(void* voidHunter) {
  HunterType* hunter = (HunterType*) voidHunter;
  enum LoggerDetails reasonForExit = LOG_UNKNOWN;
  enum hunterDecisions {COLLECT_EV, MOVE, REVIEW_EV, DECISION_COUNT};
  while(hunter->boredom < BOREDOM_MAX && hunter->fear < FEAR_MAX) {
    enum hunterDecisions choice = randInt(0, DECISION_COUNT);
    if(hunter->room->ghost!=NULL) {
      hunter->fear++;
      hunter->boredom = 0;
    } else{
      hunter->boredom++;
    }
    if(choice == COLLECT_EV) {
      sem_wait(&hunter->mutex);
      collectEvidence(hunter);
      sem_post(&hunter->mutex);
    }
    else if (choice == MOVE) { 
      sem_wait(&hunter->mutex);
      moveHunter(hunter);     
      sem_post(&hunter->mutex);
    }
    else if (choice==REVIEW_EV) {
      sem_wait(&hunter->mutex);
      int result = reviewEvidence(hunter);
      if(result==C_FALSE) l_hunterReview(hunter->name, LOG_INSUFFICIENT);
      else l_hunterReview(hunter->name, LOG_SUFFICIENT);
      sem_post(&hunter->mutex);
      if(result==C_TRUE) return NULL;
    }
    // sleep(1);
  }
  if(hunter->fear >= FEAR_MAX) reasonForExit = LOG_FEAR; 
  else if (hunter->boredom >= BOREDOM_MAX) reasonForExit = LOG_BORED;
  sem_wait(&hunter->mutex);
  removeHunter(hunter);
  l_hunterExit(hunter->name, reasonForExit);
  sem_post(&hunter->mutex);
  return NULL;
}

void collectEvidence(HunterType* hunter){
  EvidenceType* evidence = removeEvidence(&hunter->room->evidenceLeft, hunter->device);
  if(evidence==NULL) {
    // printf("No Evidence to collect.\n");
    return; // no evidence found for hunter to collect
  }
  for(EvidenceNodeType* it = hunter->sharedEvidence->head; it!=NULL; it=it->next) {
    if(it->evidence == evidence) return; // it is already collected
  }
  addEvidence(hunter->sharedEvidence, evidence);
  l_hunterCollect(hunter->name, *evidence, hunter->room->name);
}