/*
 *	Clemens Dehnert
 *	s85492
 *	MN: 53041
 *	Kontakt: clemensdehnert@stud.htw-dresden-de
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "list.h"
#include "loader.h"
#include "gui.h"

/*Datenstruktur 'Record' als vollständigen Eintrag
 * ~ Artikelname
 * ~ Artikelnumber
 * ~ Vorrat*/
typedef struct Record{
        char* name;
        int number;
        int stock;
}record;


/*Globale Variablen
 * ~ Builder & Window zur Erreichbarkeit
 * ~ Liste & Anzahl Zeilen, da pro Programmaufruf nur eine Liste bzw. eine Tabelle benötigt wird*/
GtkBuilder* builder;
GtkWidget* window;
list* pList;
int ROWS=1;

/*-----------------------------------------------*/
/*FUNKTIONSDEKLARATIONEN*/

/*Funktionsdeklarationen: Grafik*/
void displayList(list* pList);					/*Zeigt Liste basierend auf File*/
void clearAllTableRows();					/*Entfernt alle Listeneinträge (graphisch)*/

/*Funktionsdeklarationen: Callback (siehe Glade)*/
void on_window_destroy();					/*Beendet Applikation*/
void on_gtk_add_apply_clicked();			/*Hinzufügen eines Eintrags aus Feldern*/
void changeStock(GtkWidget* entry);			/*Manuell generiert (siehe addTableRow)*/
void removeItem(GtkWidget* delete);			/*Manuell generiert (siehe addTableRow)*/
void on_gtk_search_search_changed();				/*Suchfunktion*/
void on_gtk_sort_clicked();					/*Sortierfunktion*/

/*Funktionsdeklarationen: Sonstige*/
void addTableRow(char* name, int number, int stock);
int checkInput();
void change_gtk_info(char* text);
/*-----------------------------------------------*/

int main(int argc, char* argv[]){
	/*Initialisierung GTK & globaler Variablen*/
	gtk_init(NULL, NULL);
	builder=gtk_builder_new_from_file("ui.glade");
	window=GTK_WIDGET(gtk_builder_get_object(builder,"window"));
	gtk_builder_connect_signals(builder,NULL);
	pList=createList();
	if(pList==NULL){
		exit(1);
	}

	/*Befüllen & Laden der Liste*/
	loadList(pList);
	displayList(pList);

	/*Darstellung & Übergabe an gtk*/
	gtk_widget_show(window);
	gtk_main();

	return 0;
}

/*Darstellung der Listenelemente
 * ~ Iteration durch Liste, bis NULL-Pointer*/
void displayList(list* pList){
	void* pData;
	int i=0;
	while(1){
		pData=getByIndex(pList,i);
		if(pData==NULL){
			break;
		}
		record* pRecord=pData;
		addTableRow(pRecord->name,pRecord->number,pRecord->stock);
		i++;
	}
}

/*Entfernen aller Elemente aus Tabelle
 * ~ Iteration mithilfe ROWS, welches während Programmverlauf Zeilenanzahl zählt*/
void clearAllTableRows(){
        GtkGrid* gtk_grid=GTK_GRID(gtk_builder_get_object(builder,"gtk_grid"));
		for(int i=ROWS;i>0;i--){
                gtk_grid_remove_row(gtk_grid, i);
        }
        ROWS=1;
}

/*Vollständiges Beenden des Programmes*/
void on_window_destroy(){
	saveList(pList);
	deleteList(pList);
        gtk_main_quit();
}

/*Hinzufügen Elemente zur Liste & GUI*/
void on_gtk_add_apply_clicked(){
        int rnumber, rstock;
        if(0==checkInput()){
        /*Auslesen der Felder zum Erstellen eines Eintrags
	 * ~ Einführung Felder mithilfe Builder
	 * ~ Speicherung in Variablen*/
	GtkWidget* gtk_add_name=GTK_WIDGET(gtk_builder_get_object(builder,"gtk_add_name"));
        const gchar* name=gtk_entry_get_text (GTK_ENTRY (gtk_add_name));
        GtkWidget* gtk_add_number=GTK_WIDGET(gtk_builder_get_object(builder,"gtk_add_number"));
        const gchar* number=gtk_entry_get_text (GTK_ENTRY (gtk_add_number));
        GtkWidget* gtk_add_stock=GTK_WIDGET(gtk_builder_get_object(builder,"gtk_add_stock"));
        const gchar* stock=gtk_entry_get_text (GTK_ENTRY(gtk_add_stock));
        rnumber=atoi(number);
        rstock=atoi(stock);
	/*Befüllung neuer Eintrag*/
        record* pRecord=malloc(sizeof(record));
        if(pRecord==NULL){
                free(pRecord);
                return;
        }
	
	pRecord->name=(char*)malloc(sizeof(char)*strlen(name));
        if(pRecord->name==NULL){
		change_gtk_info("Speicher für Name nicht freigegeben");
		exit(1);
	}

	strcpy(pRecord->name, name);
        pRecord->number=rnumber;
        pRecord->stock=rstock;
	/*Einhängen in Liste & Aktualisierung der Darstellung*/
        void* pData=pRecord;
        insertHead(pList, pData);
        saveList(pList);
        clearAllTableRows();
        displayList(pList);
        }
}

/*Manipulation Artikelvorrat*/
void changeStock(GtkWidget* entry){
        int row=0;
        GtkGrid* gtk_grid=GTK_GRID(gtk_builder_get_object(builder,"gtk_grid"));
        if(gtk_entry_get_text(GTK_ENTRY(entry))!=NULL){
                GtkWidget* child;
                do{
                        row++;
                        child=gtk_grid_get_child_at(gtk_grid,3,row);
                }while(entry!=child);
                record* pRecord=getByIndex(pList, row-1);
                if(pRecord==NULL){
                        return;
                }
                pRecord->stock=atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
                saveList(pList);
                clearAllTableRows();
                displayList(pList);
        }else{
                return;
        }
}

/*Hinzufügen Elemente zur GUI*/
void addTableRow(char* name, int number, int stock){
	GtkGrid* gtk_grid=GTK_GRID(gtk_builder_get_object(builder,"gtk_grid"));

	/*Typänderung Artikelnummer (int -> char*)
	 * Typänderung Atrikelvorrat (int -> char*)*/
	char snumber[32];
	sprintf(snumber, "%d", number);
	char sstock[32];
	sprintf(sstock, "%d", stock);

	/*Erstellung Elemente neuer Zeile*/
	GtkWidget* labelname=gtk_label_new(name);		/*Artikelname*/
	GtkWidget* labelnumber=gtk_label_new(snumber);		/*Artikelnummer*/
	GtkWidget* labelstock=gtk_label_new(sstock);		/*Artikelvorrat*/
	GtkWidget* entry=gtk_entry_new();			/*Modifikation Vorrat*/
	GtkWidget* delete=gtk_button_new();			/*Eintrag löschen*/
        gtk_button_set_label (GTK_BUTTON(delete),"delete");

	/*Erstellung Callbackfkt. zum Modifizieren & Löschen*/
	g_signal_connect(entry, "activate", G_CALLBACK(changeStock),NULL);
	g_signal_connect(delete, "clicked", G_CALLBACK(removeItem),NULL);

	/*Hinzufügen neuer Zeile*/
	gtk_grid_insert_row(gtk_grid, ROWS);
	gtk_grid_attach(gtk_grid, labelname, 0, ROWS,1,1);	/*Artikelname*/
	gtk_widget_show(labelname);				/*Artikelname*/
        gtk_grid_attach(gtk_grid, labelnumber, 1, ROWS,1,1);	/*Artikelnummer*/
        gtk_widget_show(labelnumber);				/*Artikelnummer*/
        gtk_grid_attach(gtk_grid, labelstock, 2, ROWS,1,1);	/*Artikelvorrat*/
        gtk_widget_show(labelstock);				/*Artikelvorrat*/
	gtk_grid_attach(gtk_grid, entry, 3, ROWS,1,1);		/*Modifikation Vorrat*/
	gtk_widget_show(entry);					/*Modifikation Vorrat*/
	gtk_grid_attach(gtk_grid, delete, 4, ROWS,1,1);		/*Eintrag löschen*/
	gtk_widget_show(delete);				/*Eintrag löschen*/

	/*Zeilenanzahl aktualisieren*/
	ROWS++;
}

/*Iteration bis ges. Eintrag gefunden, Entfernung aus Liste, Speicherung Liste & erneutes Laden*/
void removeItem(GtkWidget* delete){
	int row=0;
	record* temp = NULL;
	GtkGrid* gtk_grid=GTK_GRID(gtk_builder_get_object(builder,"gtk_grid"));
	GtkWidget* child;
	do{
		row++;
		child=gtk_grid_get_child_at(gtk_grid,4,row);

	}while(delete!=child);
	
	temp=getByIndex(pList, row-1);
	free(temp->name);
	temp->name=NULL;
	temp=NULL;
	
	removeByIndex(pList, row-1);
	saveList(pList);
	clearAllTableRows();
	displayList(pList);
}

/*Bubblesort je nach Buttontext & erneutes Laden*/
void on_gtk_sort_clicked(){
	char text[64+1];
	record* temp1=NULL;
	record* temp2=NULL;
	
	GtkButton* gtk_sort=GTK_BUTTON(gtk_builder_get_object(builder,"gtk_sort"));
	strcpy(text,gtk_button_get_label(gtk_sort));
	if(0==strcmp(text,"Sortierung: Name")){
		gtk_button_set_label(gtk_sort,"Sortierung: Nummer");
		for (int n=ROWS-1; n>1; n=n-1){
    		for (int i=0; i<n-1; ++i){
      			temp1=getByIndex(pList,i);
				temp2=getByIndex(pList,i+1);
				if (temp1->number < temp2->number){
					changeNodes(pList, i, i+1);
				}
			}
		}
		clearAllTableRows();
		displayList(pList);
		return;
	}
	if(0==strcmp(text,"Sortierung: Nummer")){
		gtk_button_set_label(gtk_sort,"Sortierung: Vorrat");
        for (int n=ROWS-1; n>1; n=n-1){
           	for (int i=0; i<n-1; ++i){
                temp1=getByIndex(pList,i);
                temp2=getByIndex(pList,i+1);
                if (temp1->stock < temp2->stock){
                	changeNodes(pList, i, i+1);
            	}   
        	}   
        } 
		clearAllTableRows();
        displayList(pList);
		return;
	}
	if(0==strcmp(text,"Sortierung: Vorrat")){
		gtk_button_set_label(gtk_sort,"Sortierung: Name");
  		for (int n=ROWS-1; n>1; n=n-1){
        	for (int i=0; i<n-1; ++i){
            	temp1=getByIndex(pList,i);
                temp2=getByIndex(pList,i+1);
                if (0<strcmp(temp1->name,temp2->name)){
                	changeNodes(pList, i, i+1);
                }   
          	}   
      	}
		clearAllTableRows();
        displayList(pList);
		return;
	}
}

/*Überprüfung der Feldeinträge auf Richtigkeit & Löschung aller fehlerhaften Einträge*/
int checkInput(){
	int error=0;
        GtkWidget* gtk_add_name=GTK_WIDGET(gtk_builder_get_object(builder,"gtk_add_name"));
        const gchar* name=gtk_entry_get_text (GTK_ENTRY (gtk_add_name));
        GtkWidget* gtk_add_number=GTK_WIDGET(gtk_builder_get_object(builder,"gtk_add_number"));
        const gchar* number=gtk_entry_get_text (GTK_ENTRY (gtk_add_number));
        GtkWidget* gtk_add_stock=GTK_WIDGET(gtk_builder_get_object(builder,"gtk_add_stock"));
        const gchar* stock=gtk_entry_get_text (GTK_ENTRY(gtk_add_stock));

        if(0==strcmp(name,"")){
                error++;
        }

        if(0==atoi(number)||0==strcmp(number,"")){
                gtk_entry_set_text(GTK_ENTRY(gtk_add_number),"");
                error++;
        }

	record* rec;
	for(int i=0; i<ROWS-1; i++){
		rec = getByIndex(pList, i);
		if(rec->number == atoi(number)){
			change_gtk_info("Artikelnummer bereits vergeben");
			error++;
		}
	}

        if(0==atoi(stock)||0==strcmp(stock,"")){
                gtk_entry_set_text(GTK_ENTRY(gtk_add_stock),"");
                error++;
        }

        if(0!=error){
                return 1;
        }

        return 0;
}

/*Grafisches Entfernen aller Items
Iteration durch Liste & Überprüfung auf Treffer
ggf. Anzeigen des Treffers*/
void on_gtk_search_search_changed(){
	clearAllTableRows();
	void* pData;
	int i=0;
	GtkWidget* gtk_search=GTK_WIDGET(gtk_builder_get_object(builder,"gtk_search"));
        while(1){
                if(0==strcmp(gtk_entry_get_text(GTK_ENTRY(gtk_search)),"")){
			displayList(pList);
			break;
		}
		pData=getByIndex(pList,i);
                if(pData==NULL){
                        break;
                }
                record* pRecord=pData;
                if(0==strcmp(gtk_entry_get_text(GTK_ENTRY(gtk_search)),pRecord->name)||
		   atoi(gtk_entry_get_text(GTK_ENTRY(gtk_search)))==pRecord->number||
		   atoi(gtk_entry_get_text(GTK_ENTRY(gtk_search)))==pRecord->stock){
			addTableRow(pRecord->name,pRecord->number,pRecord->stock);
		}
		i++;
        }
}

/*Änderderung der Meldung*/
void change_gtk_info(char* text){
	GtkWidget* gtk_info=GTK_WIDGET(gtk_builder_get_object(builder,"gtk_info"));
	gtk_label_set_text (GTK_LABEL(gtk_info),text);

	return;
}
