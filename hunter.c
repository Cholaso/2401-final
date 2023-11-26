#include "defs.h"

void createHunter(char *name, HouseType *house, EvidenceType device) {
  HunterType* hunter;
  initHunter(name, house->rooms.head->room, device, &house->sharedEvidence, &hunter);
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

void* hunterActivity(void* voidHunter) {
  HunterType* hunter = (HunterType*) voidHunter;
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
      // move
    } else {
      // review evidence
    }
    printf("Hunter is existing with boredom level %d!!!\n", hunter->boredom);
    sleep(1);
  }
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