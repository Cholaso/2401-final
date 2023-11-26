#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     1000
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     0
#define FEAR_MAX        10
#define LOGGING         C_TRUE

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;
// our definitions
typedef struct Room RoomType; 
typedef struct RoomNode RoomNodeType;
typedef struct EvidenceNode EvidenceNodeType;
typedef struct RoomList RoomListType;
typedef struct EvidenceList EvidenceListType;
typedef struct EvidenceNode EvidenceNodeType;

typedef struct Hunter HunterType;
typedef struct Ghost GhostType;

typedef struct House HouseType;

typedef struct args pthread_args;

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
};
struct Hunter {
  char name[MAX_STR];
  int fear;
  int boredom;
  RoomType* room;
  EvidenceType device;
  EvidenceListType* sharedEvidence;
  EvidenceType (*variantEvidence)[4][3];
};
struct Ghost {
  int boredom;
  GhostClass ghostVariant;
  RoomType* room;
  EvidenceType (*possibleEvidence)[3];
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
  EvidenceType variantEvidence[4][3];
  int hunterCount;
};

struct args{
  GhostType* ghost;
  HouseType* house;
};

// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
 void l_ghostExit(enum LoggerDetails reason);

//init functions
void initRoomList(RoomListType* roomList);
void initEvidenceList(EvidenceListType* evidenceList);
void initHunter(char *name, RoomType *room, EvidenceType device, EvidenceListType *sharedEvidence, EvidenceType (*variantEvidence)[4][3], HunterType **hunter);
void initGhost(GhostClass variant, EvidenceType (*variantEvidence)[3], RoomType *room, GhostType **ghost);
void initRoom(char* name, RoomType** room);
void initRoomNode(RoomType* room, RoomNodeType* next, RoomNodeType* this);
void initEvidenceNode( EvidenceType* evidence, EvidenceNodeType* next, EvidenceNodeType* this);
void initHouse(HouseType* house);

// room functions
RoomType* createRoom(char* name);
void addRoom(RoomListType* roomList, RoomType* room);
void connectRooms(RoomType* first, RoomType* second);
void printRoom(RoomType* room);
void printRoomList(RoomListType* roomList);

//hunter functions
void createHunter(char* name, HouseType* house, EvidenceType device);
void printHunter(HunterType* hunter);
void addHunter(HunterType** hunters, HunterType* hunter, int* count);
void collectEvidence(HunterType* hunter);
void* hunterActivity(void* voidHunter, void* voidHouse);
void removeHunter(HunterType* hunter, HunterType** hunters, int* count);
void moveHunter(HunterType* hunter);
void deleteHunter(HunterType* hunter, HouseType* house, enum LoggerDetails reason);

//house functions
void populateRooms(HouseType* house);

//evidence functions
void addEvidence(EvidenceListType *evidenceList, EvidenceType *evidence);
EvidenceType* removeEvidence(EvidenceListType* evidenceList, EvidenceType device);

//main functions
void askForName(char* name);

// ghost functions
GhostType* createGhost(HouseType* house);
void leaveEvidence(GhostType* ghost);
void changeRoom(GhostType* ghost);
void* ghostActivity(void* ghost);