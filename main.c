//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"
#include <pthread.h>

int main() {
  // Initialize the random number generator
  srand(time(NULL));
  
  char name[MAX_STR];
  GhostType* ghost;
  pthread_t ghostThread;
  pthread_t hunterThread[NUM_HUNTERS];

  // Create the house: You may change this, but it's here for demonstration purposes
  // Note: This code will not compile until you have implemented the house functions and structures
  HouseType house;
  initHouse(&house);
  populateRooms(&house);
  for(int i = 0; i<NUM_HUNTERS; i++) {
    askForName(name);
    createHunter(name, &house, i%EV_COUNT);
  }
  ghost = createGhost(&house);
  
  pthread_create(&ghostThread,NULL,ghostActivity,ghost);
  for(int i = 0; i<NUM_HUNTERS; i++) {
    pthread_create(&hunterThread[i],NULL,hunterActivity,house.hunters[i]);
  }
  for(int i = 0; i<NUM_HUNTERS; i++) {
    pthread_join(hunterThread[i],NULL);
  }
  pthread_join(ghostThread, NULL);
  // printHouse(&house);
  if(house.sufficientEvidenceFound == C_TRUE) {
    for(int i = 0; i<GHOST_COUNT; i++) {
      for(int j = 0; i<EV_COUNT-1; i++) {
        if
      }
    }
  }
  cleanupHouse(&house);
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

void printGameResults(HouseType* house) {
  int huntersThatLeft = 0;
  printf("[GAME RESULTS]: \n");
  printf("Hunters that have exited: \n");
  for(int i = 0; i<house->hunterCount; i++) {
    HunterType *hunter = house->hunters[i];
    if(hunter->boredom >= BOREDOM_MAX) {
      printf("[%s] has exited due to [BOREDOM].\n", hunter->name);
      huntersThatLeft++;
    }
    else if(hunter->fear >= FEAR_MAX) {
      printf("[%s] has exited due to [FEAR].\n",hunter->name);
      huntersThatLeft++;
    } 
  }
  if(huntersThatLeft == house->hunterCount) {
    printf("[GAME RESULT]: All hunters left, ghost wins.\n");
  }
}