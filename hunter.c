//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"

/*
    initializes a hunter with all of its necessary information:
    
    in: name - the hunter's name
    in: device - the device that the hunter will have to read one evidence type
    in: sharedEvidence - a pointer to a list of shared evidence all hunters can see and add to
    in: variantCombinations - a pointer to all possible combinations of evidences that a ghost can leave behind
    in: mutex - the mutex that the hunter thread will obey
*/
void initHunter(char *name, RoomType *room, EvidenceType device, EvidenceListType *sharedEvidence, EvidenceType (*variantCombinations)[4][3], sem_t* mutex, HunterType **hunter) {
  *hunter = (HunterType*) malloc(sizeof(HunterType));
  strcpy((*hunter)->name, name);
  (*hunter)->room = room;
  (*hunter)->device = device;
  (*hunter)->sharedEvidence = sharedEvidence;
  (*hunter)->fear = (*hunter)->boredom = 0;
  (*hunter)->variantCombinations = variantCombinations;
  (*hunter)->mutex = mutex;
}

/*
    Creates a hunter in our house and places them in the van.
    in: name - Name of our hunter
    in: house - pointer to our house
    in: device - enum representing type of device our hunter has
*/
void createHunter(char *name, HouseType *house, EvidenceType device) {
  HunterType* hunter;
  RoomType* van = house->rooms.head->room;
  initHunter(name, van, device, &house->sharedEvidence,&house->variantCombinations, &house->mutex, &hunter);
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
  addHunter(hunter, currentRoom->room);
  l_hunterMove(hunter->name, currentRoom->room->name);
}

/*
    lets a hunter review evidence, which switches a flag if there is enough evidence to identify the ghost. the ghost and every other hunters all see this flag.

    in/out: hunter - hunter reviewing evidence
*/
int reviewEvidence(HunterType* hunter) {
  int sufficient = hunter->sharedEvidence->size >= EV_COUNT-1;
  l_hunterReview(hunter->name, sufficient ? LOG_SUFFICIENT : LOG_INSUFFICIENT);
  return sufficient;
}

/*
    lets a hunter collect evidence. if there is evidence in the hunters' current room that matches the device the hunter has, they will remove it from the room. they will then check to see if it has been collected already. if not, it will be added to the hunters' shared evidence pile.

    in/out: hunter - the hunter that will collect the evidence
*/
void collectEvidence(HunterType* hunter){
  EvidenceType* evidence = removeEvidence(&hunter->room->evidenceLeft, hunter->device);
  if(evidence==NULL) return; // no evidence found for hunter to collect
  for(EvidenceNodeType* it = hunter->sharedEvidence->head; it!=NULL; it=it->next) {
    if(it->evidence == evidence) return; // it is already collected
  }
  addEvidence(hunter->sharedEvidence, evidence);
  l_hunterCollect(hunter->name, *evidence, hunter->room->name);
}

/*
    Main control flow for hunter threads. Hunters make random descisions that are handled here. 
    in/out: voidHunter - void pointer to our hunter
*/
void* hunterActivity(void* voidHunter) {
  HunterType* hunter = (HunterType*) voidHunter;
  enum LoggerDetails reasonForExit = LOG_UNKNOWN;
  enum hunterDecisions {COLLECT_EV, MOVE, REVIEW_EV, DECISION_COUNT};
  int sufficient = C_FALSE;
  while(!sufficient && hunter->fear<FEAR_MAX && hunter->boredom<BOREDOM_MAX) {
    usleep(HUNTER_WAIT);
    sem_wait(hunter->mutex);
    int ghostPresentInRoom = hunter->room->ghost!=NULL;
    sem_post(hunter->mutex);
    if(ghostPresentInRoom) {
      hunter->fear++;
      hunter->boredom = 0;
    } else hunter->boredom++;
    enum hunterDecisions choice = randInt(0, DECISION_COUNT);
    sem_wait(hunter->mutex);
    switch(choice) {
      case COLLECT_EV:
        collectEvidence(hunter);
        break;
      case MOVE:
        moveHunter(hunter);     
        break;
      case REVIEW_EV:
        sufficient = reviewEvidence(hunter);
        break;
      default:
        break;
    }
    sem_post(hunter->mutex);
  }
  if(sufficient) reasonForExit = LOG_EVIDENCE;
  else if(hunter->fear >= FEAR_MAX) reasonForExit = LOG_FEAR;
  else if(hunter->boredom >= BOREDOM_MAX) reasonForExit = LOG_BORED;
  sem_wait(hunter->mutex);
  removeHunter(hunter);
  l_hunterExit(hunter->name, reasonForExit);
  sem_post(hunter->mutex);
  return NULL;
}