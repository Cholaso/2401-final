#include "defs.h"

void createHunter(char *name, HouseType *house, EvidenceType device) {
  HunterType* hunter;
  initHunter(name, house->rooms.head->room, device, &house->sharedEvidence, &hunter);
  addHunter(house->rooms.head->room->hunters, hunter, &house->rooms.head->room->hunterCount);
  addHunter(house->hunters, hunter, &house->hunterCount);
}

void addHunter(HunterType** hunters, HunterType* hunter, int* count){
    hunters[(*count)++]=hunter;
}

void printHunter(HunterType* hunter) {
  char evidence[MAX_STR];
  evidenceToString(hunter->device, evidence);
  printf("| Name: %s | Boredom: %d | Fear: %d | Device: %s | Location: %s |\n", hunter->name, hunter->boredom, hunter->fear, evidence, hunter->room->name);
}