/*
 *      Clemens Dehnert
 *      s85492
 *      MN: 53041
 *      Kontakt: clemensdehnert@stud.htw-dresden-de
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "gui.h"

/*Datenstruktur 'Node' als Listenelement
 * ~ Zeiger je auf Vorgänger & Nachfolger
 * ~ Zeiger auf gespeicherte Daten*/
typedef struct Node{
        struct Node* pNext;
        struct Node* pPrev;
        void* pData;
}node;

/*Datenstruktur 'List' als Liste
 * ~ Listenkopf als Verbindung zw. erstem & letztem Element
 * ~ Zeiger auf aktuelles Listenelement*/
typedef struct List{
        node head;
        node* pCurr;
}list;

/*Funktionsdeklarationen
 * ~ Nutzen: siehte list.h*/
list* createList();
int deleteList(list* pList);
int insertNodeBehind(node* BeforeNode, void* pData);
int insertHead(list* pList, node* pItem);
void* getByIndex(list* pList, int index);
int removeFirst(list* pList);
int removeByIndex(list* pList, int index);
int changeNodes(list* pList, int index1, int index2);

/*Erzeugen einer Liste
 * ~ ggf. Fehlermeldung
 * ~ Initialisierung Listenkopf*/
list* createList(){
        list* pList=(list*)malloc(sizeof(list));
        if(pList==NULL){
                printf("Liste konnte nicht angelegt werden.\n");
                return NULL;
        }else{
                pList->head.pNext=(node*)pList;
                pList->head.pPrev=(node*)pList;
                pList->head.pData=NULL;
                pList->pCurr=NULL;
        }
        return pList;
}

/*Löschen einer Liste
 * ~ Löschen aller Elemente & schließlich Listenkopf*/
int deleteList(list* pList){
        while(pList->head.pNext->pData!=NULL){
                removeFirst(pList);
        }
	free(pList);
        return 0;
}

/*Einfügen eines neues Elements hinter einem bestehenden ein
 * ~ ggf. Fehlermeldung
 * ~ Initialisierung neues Element
 * ~ Aktualisierung Zeiger Vorgänger & Nachfolger des neuen Elements*/
int insertNodeBehind(node* pBeforeNode, void* pData){
		node* pNode=malloc(sizeof(node));
        if(pNode==NULL){
			change_gtk_info("Neues Element konnte nicht eingefügt werden.");
		return 1;
	}else{
        pNode->pNext=pBeforeNode->pNext;
		pNode->pPrev=pBeforeNode;
		pNode->pData=pData;
		pNode->pNext->pPrev=pNode;
		pBeforeNode->pNext=pNode;
        }
        return 0;
}

/*Einfügen am Listenanfang
 * ~ Einfügen nach Listenkopf*/
int insertHead(list* pList, node* pData){
        int ret=insertNodeBehind(&pList->head, pData);
	if(ret == 1){
		return 1;
	}
        return 0;
}

/*Gibt Daten eines Elements nach Index zurück
 * ~ Iteration & ggf. Fehlerbehandlung in rufender Funktion*/
void* getByIndex(list* pList, int index){
        pList->pCurr=pList->head.pNext;
		for(int i=0;i<index;i++){
                pList->pCurr=pList->pCurr->pNext;
                if(pList->pCurr->pData==NULL){
                        return NULL;
                }
        }
        return pList->pCurr->pData;
}

/*Entfernen eines Elements nach Index
 * ~ Iteration & ggf. Fehlerbehandlung in rufender Funktion
 * ~ Ausklinken des Element & Speicherfreigabe*/
int removeByIndex(list* pList, int index){
	pList->pCurr=pList->head.pNext;
	for(int i=0;i<index;i++){
		pList->pCurr=pList->pCurr->pNext;
		if(pList->pCurr->pData==NULL){
			return 1;
		}
	}
	pList->pCurr->pPrev->pNext=pList->pCurr->pNext;
    pList->pCurr->pNext->pPrev=pList->pCurr->pPrev;
    free(pList->pCurr);
	change_gtk_info("Eintrag gelöscht");
	return 0;
}

/*Entfernen des ersten Elements
 * ~ Überprüfung, ob ein solches Element überhaupt existiert
 * ~ Ausklinken des Elements & Speicherfreigabe*/
int removeFirst(list* pList){
    pList->pCurr=pList->head.pNext;
    if(pList->pCurr==&pList->head){
		return 1;
	}
	pList->head.pNext=pList->pCurr->pNext;
    free(pList->pCurr);
    return 0;
}

/*Nur für benachbarte Nodes*/
int changeNodes(list* pList, int index1, int index2){
	node* temp1;
	node* temp2;
	node* p1;
	node* n2;
	pList->pCurr=pList->head.pNext;
	for(int i=0;i<index1;i++){
    	pList->pCurr=pList->pCurr->pNext;
		if(pList->pCurr->pData==NULL){
			return 1;
		}
    }
	temp1=pList->pCurr;
	pList->pCurr=pList->head.pNext;
	for(int i=0;i<index2;i++){
        pList->pCurr=pList->pCurr->pNext;
		if(pList->pCurr->pData==NULL){
        	return 1;
        }
    }
    temp2=pList->pCurr;
	p1=temp1->pPrev;
	n2=temp2->pNext;
	temp1->pPrev=temp2;
	temp1->pNext=temp2->pNext;
	temp2->pPrev=p1;
	temp2->pNext=temp1;
	p1->pNext=temp2;
	n2->pPrev=temp1;
	return 0;
}
