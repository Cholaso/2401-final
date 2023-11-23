#include "defs.h"
void addEvidence(EvidenceListType *evidenceList, EvidenceType *evidence){
  EvidenceNodeType* toAdd = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
  toAdd->evidence = evidence;
  toAdd->next = NULL;
  if(evidenceList->head == NULL) evidenceList->head = evidenceList->tail = toAdd;
  else{
    evidenceList->tail->next = toAdd;
    evidenceList->tail = toAdd;
  }
}