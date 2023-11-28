//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"

/*
   initializes a room by allocating it on the heap, giving it a name, initializing its array of hunters and its linked list of evidence, and setting its initial ghost to null
   
   in: name - the name that the room will have
   in/out: room - the room we are initializing
*/
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

/*
   initializes a room and returns a pointer to it
   in: name - the name that the room should have
   
   returns: a pointer to the room that was created
*/
RoomType* createRoom(char* name) {
  RoomType* room;
  initRoom(name, &room);
  return room;
}

/*
   initializes a room list by setting its head and tail to null, and size to 0
   out: roomList - the room list to initialize
*/
void initRoomList(RoomListType *roomList) {
  roomList->head = roomList->tail = NULL;
  roomList->size = 0;
}

/*
   adds a room to the end of a room list
   in/out: roomList - the room list we want to add to
   in: room - the room we are adding
*/
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

/*
   adds one room to the other's neighbors list (a list of rooms) and vice-versa
   in/out: first - any given room
   in/out: second - another room that is to be a neighbor to the first
*/
void connectRooms(RoomType *first, RoomType *second) {
  addRoom(&first->neighbors, second);
  addRoom(&second->neighbors, first);
}

/*
   cleans up a room list but NOT the data in the list
   in/out: roomList - the roomList we want to clean up
*/
void cleanupRoomList(RoomListType* roomList) {
  RoomNodeType* temp;
  while(roomList->head!=NULL) {
    temp = roomList->head;
    roomList->head = roomList->head->next;
    free(temp);
  }
}