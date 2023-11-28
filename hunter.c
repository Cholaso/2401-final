//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"

/*
    Creates a hunter in our house and places them in the van.
    in: name - Name of our hunter
    in: house - pointer to our house
    in: device - enum representing type of device our hunter has
*/
void createHunter(char *name, HouseType *house, EvidenceType device) {
  HunterType* hunter;
  RoomType* van = house->rooms.head->room;
  initHunter(name, van, device, &house->sharedEvidence,&house->variantEvidence, &house->mutex, &house->sufficientEvidenceFound, &hunter);
  addHunter(hunter, van);
  house->hunters[(house->hunterCount)++] = hunter;
  l_hunterInit(name, device);
}

/*
    Adds a hunter to a room.
    in/out: hunter - hunter getting added
    in/out: room - room that the hunter is getting added to.
*/
void addHunter(HunterType* hunter, RoomType* room){
    room->hunters[(room->hunterCount)++] = hunter;
    hunter->room = room;
}
/*
    removes a hunter from a hunter array, and shifts the rest of the array to maintain arraylist properties.
    in/out: hunter - hunter getting removed
*/
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
/*
    moves a hunter from current room to a random neighbor of the current room.
    in/out: hunter - hunter getting moved
*/
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
/*
    prints the hunter in a readable format.
    in: hunter - our hunter
*/
void printHunter(HunterType* hunter) {
  char evidence[MAX_STR];
  evidenceToString(hunter->device, evidence);
  printf("| Name: %s | Boredom: %d | Fear: %d | Device: %s | Location: %s |\n", hunter->name, hunter->boredom, hunter->fear, evidence, hunter->room->name);
}
/*
    hunter reviews evidence, returns true if there is enough evidence to identify the ghost.
    in/out: hunter - hunter reviewing evidence
*/
void reviewEvidence(HunterType* hunter) {
  int sufficient = hunter->sharedEvidence->size >= 3;
  l_hunterReview(hunter->name, sufficient ? LOG_SUFFICIENT : LOG_INSUFFICIENT);
  if(sufficient) *hunter->sufficientEvidenceFound = C_TRUE;
}
/*
    Main control flow for hunter threads. Hunters make random descisions that are handled here. 
    in/out: voidHunter - void pointer to our hunter
*/
void* hunterActivity(void* voidHunter) {
  HunterType* hunter = (HunterType*) voidHunter;
  enum LoggerDetails reasonForExit = LOG_UNKNOWN;
  enum hunterDecisions {COLLECT_EV, MOVE, REVIEW_EV, DECISION_COUNT};
  do {
    usleep(HUNTER_WAIT);
    sem_wait(hunter->mutex);
    int sufficientEvidenceFound = *hunter->sufficientEvidenceFound;
    int ghostPresent = hunter->room->ghost!=NULL;
    sem_post(hunter->mutex);
    if(sufficientEvidenceFound==C_TRUE) break;
    if(ghostPresent) {
      hunter->fear++;
      hunter->boredom = 0;
    } else{
      hunter->boredom++;
    }
    enum hunterDecisions choice = randInt(0, DECISION_COUNT);
    if(choice == COLLECT_EV) {
      sem_wait(hunter->mutex);
      collectEvidence(hunter);
      sem_post(hunter->mutex);
    }
    else if (choice == MOVE) { 
      sem_wait(hunter->mutex);
      moveHunter(hunter);     
      sem_post(hunter->mutex);
    }
    else if (choice==REVIEW_EV) {
      sem_wait(hunter->mutex);
      reviewEvidence(hunter);
      sem_post(hunter->mutex);
    }
  } while(hunter->boredom < BOREDOM_MAX && hunter->fear < FEAR_MAX);
  if(hunter->fear >= FEAR_MAX) reasonForExit = LOG_FEAR; 
  else if (hunter->boredom >= BOREDOM_MAX) reasonForExit = LOG_BORED;
  else reasonForExit = LOG_EVIDENCE;
  sem_wait(hunter->mutex);
  removeHunter(hunter);
  l_hunterExit(hunter->name, reasonForExit);
  sem_post(hunter->mutex);
  return NULL;
}
/*
    Main control flow for hunter threads. 
    in/out
*/
void collectEvidence(HunterType* hunter){
  EvidenceType* evidence = removeEvidence(&hunter->room->evidenceLeft, hunter->device);
  if(evidence==NULL) {
    printf("%s tried to collect evidence but failed.\n",hunter->name);
    return; // no evidence found for hunter to collect
  }
  for(EvidenceNodeType* it = hunter->sharedEvidence->head; it!=NULL; it=it->next) {
    if(it->evidence == evidence) return; // it is already collected
  }
  addEvidence(hunter->sharedEvidence, evidence);
  l_hunterCollect(hunter->name, *evidence, hunter->room->name);
}