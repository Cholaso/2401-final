//Nicholas O'Neil : Jazeel Abdul-Jabbar
//101200961       : 101253438
#include "defs.h"

void initEvidenceList(EvidenceListType *evidenceList) {
  evidenceList->head = evidenceList->tail = NULL;
  evidenceList->size = 0;
}

/* 
    Adds evidence to the evidence list
    in/out: evidenceList - the list that we store the evidence in
    out: evidence - the evidence we are adding
*/
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

/* 
    removes first instance of device readable evidence from the evidence list
    in/out: evidenceList - the list that we store the evidence in
    in: device - the device we are using to find evidence
*/
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
/* 
    Cleans all dynamically allocated memory from the evidence list
    in/out: evidenceList - the list that we are cleaning
*/
void cleanupEvidenceList(EvidenceListType* evidenceList) {
  EvidenceNodeType* temp;
  while(evidenceList->head!=NULL) {
    temp = evidenceList->head;
    evidenceList->head = evidenceList->head->next;
    free(temp);
  }
}