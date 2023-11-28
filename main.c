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

GhostClass determineGhost(HouseType* house) {
  // we sort the shared evidence by enum order using counting sort
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
  // return the ghost type by finding a matching array in house's combinations
  int found = C_FALSE;
  for(int i = 0; i < GHOST_COUNT; i++){
    for(int j = 0; j < EV_COUNT-1; j++){
      if(house->variantEvidence[i][j] != sortedEv[j]){
        found = C_FALSE;
        break;       
      }
      found = C_TRUE;
    }
    if(found)return i;
  }
  return GH_UNKNOWN;
}

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