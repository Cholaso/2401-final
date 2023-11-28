//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"
void initRoomList(RoomListType*roomList) {
  roomList->head = roomList->tail = NULL;
  roomList->size = 0;
}

void initEvidenceList(EvidenceListType *evidenceList) {
  evidenceList->head = evidenceList->tail = NULL;
  evidenceList->size = 0;
}

void initHunter(char *name, RoomType *room, EvidenceType device, EvidenceListType *sharedEvidence, EvidenceType (*variantEvidence)[4][3], sem_t* mutex, int* sufficientEvidenceFound, HunterType **hunter) {
  *hunter = (HunterType*) malloc(sizeof(HunterType));
  strcpy((*hunter)->name, name);
  (*hunter)->room = room;
  (*hunter)->device = device;
  (*hunter)->sharedEvidence = sharedEvidence;
  (*hunter)->fear = (*hunter)->boredom = 0;
  (*hunter)->variantEvidence = variantEvidence;
  (*hunter)->mutex = mutex;
  (*hunter)->sufficientEvidenceFound = sufficientEvidenceFound;
}

void initGhost(GhostClass variant, EvidenceType (*variantEvidence)[3], RoomType *room, sem_t *mutex, int* sufficientEvidenceFound, GhostType **ghost) {
  *ghost = (GhostType*) malloc(sizeof(GhostType));
  (*ghost)->ghostVariant = variant;
  (*ghost)->room = room;
  (*ghost)->boredom = 0;
  (*ghost)->possibleEvidence = variantEvidence;
  (*ghost)->mutex = mutex;
  (*ghost)->sufficientEvidenceFound = sufficientEvidenceFound;
}

void initRoom(char *name, RoomType **room) {
  *room = (RoomType*) malloc(sizeof(RoomType));
  strcpy((*room)->name, name);
  initRoomList(&(*room)->neighbors);
  (*room)->ghost = NULL;
  initEvidenceList(&(*room)->evidenceLeft);
  for(int i = 0; i<NUM_HUNTERS; i++) {
    (*room)->hunters[i] = NULL;
  }
  (*room)->hunterCount = 0;
}

void initHouse(HouseType *house) {
  initRoomList(&house->rooms);
  initEvidenceList(&house->sharedEvidence);
  for(int i = 0; i<NUM_HUNTERS; i++) {
    house->hunters[i] = NULL;
  }
  house->hunterCount = 0;
  EvidenceType variantCombinations[GHOST_COUNT][EV_COUNT-1] = {{EMF,TEMPERATURE,FINGERPRINTS},{EMF,TEMPERATURE,SOUND},{EMF,FINGERPRINTS,SOUND},{TEMPERATURE,FINGERPRINTS,SOUND}};
  for(int i = 0; i<GHOST_COUNT; i++){
    for(int j = 0; j < EV_COUNT-1; j++){
      house->variantEvidence[i][j] = variantCombinations[i][j];
    }
  }
  sem_init(&house->mutex, 0, 1);
  house->sufficientEvidenceFound = C_FALSE;
}