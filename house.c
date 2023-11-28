//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"

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

/*
    Dynamically allocates several rooms and populates the provided house.
    Note: You may modify this as long as room names and connections are maintained.
        out: house - the house to populate with rooms. Assumes house has been initialized.
*/
void populateRooms(HouseType* house) {
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");

    // This adds each room to each other's room lists
    // All rooms are two-way connections
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    addRoom(&house->rooms, van);
    addRoom(&house->rooms, hallway);
    addRoom(&house->rooms, master_bedroom);
    addRoom(&house->rooms, boys_bedroom);
    addRoom(&house->rooms, bathroom);
    addRoom(&house->rooms, basement);
    addRoom(&house->rooms, basement_hallway);
    addRoom(&house->rooms, right_storage_room);
    addRoom(&house->rooms, left_storage_room);
    addRoom(&house->rooms, kitchen);
    addRoom(&house->rooms, living_room);
    addRoom(&house->rooms, garage);
    addRoom(&house->rooms, utility_room);
}
/*
    Prints all data in our house in a readable format.
    in: house - our house
*/
void printHouse(HouseType* house) {
  for(RoomNodeType* it = house->rooms.head; it!=NULL; it=it->next) {
    printRoom(it->room);
  }
  printf("SHARED EVIDENCE LIST: ");
  for(EvidenceNodeType* it = house->sharedEvidence.head; it!=NULL; it=it->next) {
    char evidenceStr[MAX_STR];
    evidenceToString(*it->evidence, evidenceStr);
    printf("%s, ", evidenceStr);
  }
}
/*
    Cleans up our house and all data within, freeing all dynamic memory in our program.
    in/out: house - our house
    in/out: ghost - our ghost
*/
void cleanupHouse(HouseType* house, GhostType* ghost) {
  for(int i = 0; i<house->hunterCount; i++) {
    free(house->hunters[i]);
  }
  RoomNodeType* temp;
  while(house->rooms.head!=NULL) {
    temp = house->rooms.head;
    house->rooms.head = house->rooms.head->next;
    cleanupEvidenceList(&temp->room->evidenceLeft);
    cleanupRoomList(&temp->room->neighbors);
    free(temp->room);
    free(temp);
  }
  cleanupEvidenceList(&house->sharedEvidence);
  free(ghost);
}