//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE
#define VAN_ROOM_POS    0

// our typedefs
typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;
typedef struct Room RoomType; 
typedef struct RoomNode RoomNodeType;
typedef struct EvidenceNode EvidenceNodeType;
typedef struct RoomList RoomListType;
typedef struct EvidenceList EvidenceListType;
typedef struct Hunter HunterType;
typedef struct Ghost GhostType;
typedef struct House HouseType;

// our data types
enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

struct RoomList {
  RoomNodeType* head;
  RoomNodeType* tail;
  int size;
};
struct EvidenceList {
  EvidenceNodeType* head;
  EvidenceNodeType* tail;
  int size;
};
struct Hunter {
  char name[MAX_STR];
  int fear;
  int boredom;
  RoomType* room;
  EvidenceType device;
  EvidenceListType* sharedEvidence;
  EvidenceType (*variantCombinations)[GHOST_COUNT][EV_COUNT-1];
  sem_t* mutex;
};
struct Ghost {
  int boredom;
  GhostClass ghostVariant;
  RoomType* room;
  EvidenceType (*possibleEvidences)[EV_COUNT-1];
  sem_t* mutex;
};
struct Room {
  char name[MAX_STR];
  RoomListType neighbors;
  GhostType* ghost;
  EvidenceListType evidenceLeft;
  HunterType* hunters[NUM_HUNTERS];
  int hunterCount;
};
struct RoomNode {
  RoomType* room;
  RoomNodeType* next;
};
struct EvidenceNode {
  EvidenceType* evidence;
  EvidenceNodeType* next;
};
struct House {
  HunterType* hunters[NUM_HUNTERS];
  RoomListType rooms;
  EvidenceListType sharedEvidence;
  EvidenceType variantCombinations[GHOST_COUNT][EV_COUNT-1];
  int hunterCount;
  sem_t mutex;
};

//evidence functions: evidence.c
void initEvidenceList(EvidenceListType* evidenceList);
void addEvidence(EvidenceListType *evidenceList, EvidenceType *evidence);
EvidenceType* removeEvidence(EvidenceListType* evidenceList, EvidenceType device);
void cleanupEvidenceList(EvidenceListType* evidenceList);

// ghost functions: ghost.c
void initGhost(GhostClass variant, EvidenceType (*variantEvidence)[EV_COUNT-1], RoomType *room, sem_t *mutex, GhostType **ghost);
GhostType* createGhost(HouseType* house);
void leaveEvidence(GhostType* ghost);
void changeRoom(GhostType* ghost);
void* ghostActivity(void* ghost);
void removeGhost(GhostType* ghost);

//house functions: house.c
void initHouse(HouseType* house);
void populateRooms(HouseType* house);
void cleanupHouse(HouseType* house, GhostType* ghost);

//hunter functions: hunter.c
void initHunter(char *name, RoomType *room, EvidenceType device, EvidenceListType *sharedEvidence, EvidenceType (*variantEvidence)[GHOST_COUNT][EV_COUNT-1], sem_t* mutex, HunterType **hunter);
void createHunter(char* name, HouseType* house, EvidenceType device);
void addHunter(HunterType* hunter, RoomType* room);
void collectEvidence(HunterType* hunter);
void* hunterActivity(void* voidHunter);
void removeHunter(HunterType* hunter);
void moveHunter(HunterType* hunter);
void deleteHunter(HunterType* hunter, HouseType* house, enum LoggerDetails reason);

// Logging Utilities: logger.c
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);

//main functions: main.c
void askForName(char* name);
void printGameResults(HouseType* house, GhostType* ghost);
GhostClass determineGhost(HouseType* house);

// room functions: room.c
void initRoom(char* name, RoomType** room);
RoomType* createRoom(char* name);
void initRoomList(RoomListType* roomList);
void addRoom(RoomListType* roomList, RoomType* room);
void connectRooms(RoomType* first, RoomType* second);
void cleanupRoomList(RoomListType* roomList);

// Helper Utilies: util.c
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter