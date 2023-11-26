#include "defs.h"
#include <pthread.h>

int main() {
  // Initialize the random number generator
  srand(time(NULL));
  
  char name[MAX_STR];
  GhostType* ghost;
  pthread_t ghostThread;

  // Create the house: You may change this, but it's here for demonstration purposes
  // Note: This code will not compile until you have implemented the house functions and structures
  HouseType house;
  initHouse(&house);
  populateRooms(&house);
  // printRoomList(&house.rooms);
  for(int i = 0; i<NUM_HUNTERS; i++) {
    askForName(name);
    createHunter(name, &house, i);
    // printHunter(house.hunters[i]);
  }
  ghost = createGhost(&house);
  pthread_create(&ghostThread,NULL,ghostActivity,(void*)ghost);
  hunterActivity((void*)house.hunters[0]);
  pthread_join(ghostThread, NULL);
  printRoomList(&house.rooms);
  return 0;
}

void askForName(char* name) {
  char c;
  do{
    printf("Please Select a name for hunter: ");
    fgets(name, MAX_STR, stdin);
  } while(strcspn(name,"\n")==0);
  if(strcspn(name, "\n") >= MAX_STR-1){
    while((c = getchar()) != '\n');
  }
  name[strcspn(name, "\n")] = '\0'; // set the value of room[index of "\n"] to 0
}