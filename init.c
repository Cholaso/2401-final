#include "defs.h"
#include <stdlib.h>
void initRoomList(RoomListType*roomList) {
  roomList->head = roomList->tail = NULL;
  roomList->size = 0;
}

void initEvidenceList(EvidenceListType *evidenceList) {
  evidenceList->head = evidenceList->tail = NULL;
}

void initHunter(char *name, RoomType *room, EvidenceType device, EvidenceListType *sharedEvidence, EvidenceType (*variantEvidence)[4][3], HunterType **hunter) {
  *hunter = (HunterType*) malloc(sizeof(HunterType));
  strcpy((*hunter)->name, name);
  (*hunter)->room = room;
  (*hunter)->device = device;
  (*hunter)->sharedEvidence = sharedEvidence;
  (*hunter)->fear = (*hunter)->boredom = 0;
  (*hunter)->variantEvidence = variantEvidence;
}

void initGhost(GhostClass variant, EvidenceType (*variantEvidence)[3], RoomType *room, GhostType **ghost) {
  *ghost = (GhostType*) malloc(sizeof(GhostType));
  (*ghost)->ghostVariant = variant;
  (*ghost)->room = room;
  (*ghost)->boredom = 0;
  (*ghost)->possibleEvidence = variantEvidence;
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
  EvidenceType init[4][3] = {{EMF,TEMPERATURE,FINGERPRINTS},{EMF,TEMPERATURE,SOUND},{EMF,FINGERPRINTS,SOUND},{TEMPERATURE,FINGERPRINTS,SOUND}};
  house->variantEvidence[0][0] = EMF;
  for(int i = 0; i<4; i++){
    for(int j = 0; j < 3; j++){
      house->variantEvidence[i][j] = init[i][j];
    }
  }
}