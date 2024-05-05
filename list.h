/*
 *      Clemens Dehnert
 *      s85492
 *      MN: 53041
 *      Kontakt: clemensdehnert@stud.htw-dresden-de
 */

typedef struct Node node;
typedef struct List list;

list* createList();								/*Erstellt eine Liste*/
int deleteList(list* pList);					/*Löscht eine Liste*/
int insertHead(list* pList, node* pItem);		/*Fügt ein an Listenanfang*/
void* getByIndex(list* pList, int index);		/*Gibt Pointer auf Daten  eines Element nach Index*/
int removeByIndex(list* pList, int index);		/*Löscht Element nach Index*/
int changeNodes(list* pList, int index1, int index2);	/*Vertauscht Listenelemente*/
