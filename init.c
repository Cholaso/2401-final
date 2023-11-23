#include "defs.h"
#include <stdlib.h>
void initRoomList(RoomListType*roomList) {
  roomList->head = roomList->tail = NULL;
  roomList->size = 0;
}

void initEvidenceList(EvidenceListType *evidenceList) {
  evidenceList->head = evidenceList->tail = NULL;
}

void initHunter(char *name, RoomType *room, EvidenceType device, EvidenceListType *sharedEvidence, HunterType **hunter) {
  *hunter = (HunterType*) malloc(sizeof(HunterType));
  strcpy((*hunter)->name, name);
  (*hunter)->room = room;
  (*hunter)->device = device;
  (*hunter)->sharedEvidence = sharedEvidence;
  (*hunter)->fear = (*hunter)->boredom = 0;
}

void initGhost(GhostClass variant, RoomType *room, GhostType **ghost) {
  *ghost = (GhostType*) malloc(sizeof(GhostType));
  (*ghost)->ghostVariant = variant;
  (*ghost)->room = room;
  (*ghost)->boredom = 0;
}

void initRoom(char *name, RoomType **room) {
  *room = (RoomType*) malloc(sizeof(RoomType));
  strcpy((*room)->name, name);
  initRoomList(&(*room)->neighbors);
  (*room)->ghost = NULL;
  initEvidenceList(&(*room)->evidenceLeft);
  for(int i = 0; i<NUM_HUNTERS; i++) (*room)->hunters[i] = NULL;
  (*room)->hunterCount = 0;
}

// void initRoomNode(RoomType *room, RoomNodeType *next, RoomNodeType *this) {
//   this->room = room;
//   this->next = next;
// }

// void initEvidenceNode(EvidenceType *evidence, EvidenceNodeType *next, EvidenceNodeType *this) {
//   this->evidence = evidence;
//   this->next = next;
// }

void initHouse(HouseType *house) {
  initRoomList(&house->rooms);
  initEvidenceList(&house->sharedEvidence);
  house->hunterCount = 0;
  for(int i = 0; i<NUM_HUNTERS; i++) house->hunters[i] = NULL;
}