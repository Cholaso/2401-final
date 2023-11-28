//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"

void initRoomList(RoomListType*roomList) {
  roomList->head = roomList->tail = NULL;
  roomList->size = 0;
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

RoomType* createRoom(char* name) {
  RoomType* room;
  initRoom(name, &room);
  return room;
}

void addRoom(RoomListType *roomList, RoomType *room) {
  RoomNodeType* toAdd = (RoomNodeType*) malloc(sizeof(RoomNodeType));
  toAdd->room = room;
  toAdd->next = NULL;
  if(roomList->head == NULL) roomList->head = roomList->tail = toAdd;
  else{
    roomList->tail->next = toAdd;
    roomList->tail = toAdd;
  }
  roomList->size++;
}

void connectRooms(RoomType *first, RoomType *second) {
  addRoom(&first->neighbors, second);
  addRoom(&second->neighbors, first);
}

void printRoom(RoomType* room) {
  char ghostName[MAX_STR];
  char evidence[MAX_STR];
  printf("| %19s | Neighbors: ", room->name);
  for(RoomNodeType* it = room->neighbors.head; it!=NULL; it=it->next) {
    printf(" %s ", it->room->name);
  }
  printf("| Hunters: ");
  for(int i = 0; i< NUM_HUNTERS; i++){
    if(room->hunters[i] != NULL){
      printf("%s,", room->hunters[i]->name);
    }
  }
  if(room->ghost!=NULL){
    ghostToString(room->ghost->ghostVariant, ghostName);
    printf("| GHOST: %s", ghostName);
  }
  printf(" | Evidence: ");
  for(EvidenceNodeType* it = room->evidenceLeft.head; it!=NULL; it=it->next) {
    evidenceToString(*(it->evidence), evidence);
    printf("%s,", evidence);
  }
  printf(" |\n");
  //to be continues...
}

void cleanupRoomList(RoomListType* roomList) {
  RoomNodeType* temp;
  while(roomList->head!=NULL) {
    temp = roomList->head;
    roomList->head = roomList->head->next;
    free(temp);
  }
}