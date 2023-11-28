//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"

int main() {
  srand(time(NULL));
  char name[MAX_STR];
  HouseType house;
  GhostType* ghost;
  pthread_t ghostThread, hunterThread[NUM_HUNTERS];
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
  printGameResults(&house, ghost);
  cleanupHouse(&house, ghost);
  return 0;
}
/*
    prompts to user to enter the name of a hunter.
    in/out name: the name that a hunter will have
 */
void askForName(char* name) {
  char c;
  do{
    printf("Please select a name for hunter: ");
    fgets(name, MAX_STR, stdin);
  } while(strcspn(name,"\n")==0); // don't allow user to enter nothing
  if(strcspn(name, "\n") >= MAX_STR-1){ // if no "\n" found in the string
    while((c = getchar()) != '\n');  // we have an overflow, clear buffer
  }
  name[strcspn(name, "\n")] = '\0'; // remove the newline in the string
}

/*
    looks at the shared evidence of the house and determines which ghost it is
    in: house - the house that contains the shared evidence
    
    returns: the type of ghost that was determined
 */
GhostClass determineGhost(HouseType* house) {
  // we first sort the shared evidence by enum order using counting sort
  EvidenceType sharedEv[EV_COUNT];
  EvidenceType sortedEv[EV_COUNT-1];
  int j = 0;
  for(int i = 0; i<EV_COUNT; i++) {
    sharedEv[i] = 0;
  }
  for(EvidenceNodeType* it = house->sharedEvidence.head; it!=NULL; it=it->next){
    sharedEv[*it->evidence]++;
  }
  for(int i = 0; i<EV_COUNT; i++) {
    if(sharedEv[i] != 0) sortedEv[j++] = i;
  }
  // then find the ghost type by finding a matching sorted array in house
  int found = C_FALSE;
  for(int i = 0; i < GHOST_COUNT; i++){
    for(int j = 0; j < EV_COUNT-1; j++){
      if(house->variantCombinations[i][j] != sortedEv[j]){
        found = C_FALSE;
        break;       
      }
      found = C_TRUE;
    }
    if(found)return i;
  }
  return GH_UNKNOWN;
}

/*
   prints the outcome of the game by showing which hunters left, what evidence the hunters collected, and whether they won or not.

  in: house - the house which contains the hunters and shared evidence
  in: ghost - the ghost, used to reveal what type it was
 */
void printGameResults(HouseType* house, GhostType* ghost) {
  int huntersThatLeft = 0;
  char buffer[MAX_STR];
  printf("--------------------------------------------\n");
  for(int i = 0; i < house->hunterCount; i++) {
    HunterType *hunter = house->hunters[i];
    if(hunter->boredom >= BOREDOM_MAX) {
      printf("[GAME RESULT]: [%s] has exited due to [BOREDOM].\n", hunter->name);
      huntersThatLeft++;
    }
    else if(hunter->fear >= FEAR_MAX) {
      printf("[GAME RESULT]: [%s] has exited due to [FEAR].\n",hunter->name);
      huntersThatLeft++;
    } 
  }
  ghostToString(ghost->ghostVariant,buffer);
  printf("[GAME RESULT]: Ghost type was %s.\n", buffer);
  printf("[GAME RESULT]: Shared evidence that hunters collected: ");
  for(EvidenceNodeType* it = house->sharedEvidence.head; it!=NULL; it=it->next){
    evidenceToString(*it->evidence, buffer);
    strcat(buffer, it->next == NULL ? "\n" : ", ");
    printf("%s", buffer);
  }
  if(huntersThatLeft == house->hunterCount) {
    printf("[GAME RESULT]: All hunters left. Ghost wins.\n");
  } else {
    ghostToString(determineGhost(house), buffer);
    printf("[GAME RESULT]: Hunters identified ghost as \"%s\". Hunters win.\n", buffer);
  }
}