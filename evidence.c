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
  evidenceList->size++;
}

EvidenceType* removeEvidence(EvidenceListType* evidenceList, EvidenceType device) {
  EvidenceNodeType* prev = NULL;
  EvidenceType* evidence = NULL;
  for(EvidenceNodeType* it = evidenceList->head; it!=NULL; it=it->next) {
    if(*(it->evidence) == device){
      evidence = it->evidence;
      if(it == evidenceList->head) evidenceList->head = it->next;
      else prev->next = it->next;
      if(it == evidenceList->tail) evidenceList->tail = prev;
      free(it);
      evidenceList->size--;
      break;
    }
    prev = it;
  }
  return evidence;
}