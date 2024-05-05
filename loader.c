/*
 *      Clemens Dehnert
 *      s85492
 *      MN: 53041
 *      Kontakt: clemensdehnert@stud.htw-dresden-de
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "loader.h"

/*Datenstruktur 'Record' als vollständigen Eintrag
 * ~ Artikelname
 * ~ Artikelnumber
 * ~ Vorrat*/
typedef struct Record{
        char* name;
        int number;
        int stock;
}record;

/*Laden der Einträge aus der Datei in die Liste
 * Eröffnung der Datei
 * Befüllen der Records mit Inhalt aus Datei & einhängen in List*/
int loadList(list* pList){
	FILE* file=fopen("save.txt","rt");
	void* pData;
	char vBuf[64];
	if(file==NULL){
		return 1;
	}
	while(1){
		/*neuer Eintrag*/
		record* pRecord=malloc(sizeof(record));
                if(pRecord==NULL){
                        return 1;
                }
		/*Einlesen Eintrag & schreiben*/
                //if(3==fscanf(file, "%[^,],%d,%d\n",pRecord->name,&(pRecord->number),&(pRecord->stock))){
		if(3==fscanf(file, "%[^,],%d,%d\n",vBuf,&(pRecord->number),&(pRecord->stock))){
			pRecord->name=(char*)malloc(sizeof(char)*sizeof(vBuf));
        		if(pRecord->name==NULL){
                		printf("Error with loading names\n");
                		exit(1);
        		}
        	strcpy(pRecord->name, vBuf);
			
			pData=pRecord;
			insertHead(pList,pData);
        }else{
			free(pRecord);
			break;
		}
	}
	return 0;
}

/*Speichern der Liste
 * ~ Iterieren durch Liste & schreiben des Datensatzes in Datei
 * ~ Abbruch bei NULL*/
int saveList(list* pList){
	FILE* file=fopen("save.txt","wt");
	void* pData;
	int i=0;
	if(file==NULL){
		return 1;
	}
	fseek(file,0,SEEK_END);
	while(1){
		pData=getByIndex(pList,i);
		if(pData==NULL){
			fclose(file);
			return 0;
		}
		record* pRecord=pData;
		fprintf(file,"%s,%d,%d\n",pRecord->name,pRecord->number,pRecord->stock);
		i++;
	}
	return 0;
}
