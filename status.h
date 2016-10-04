#include <stdlib.h>
#include <string.h>


/*SIMPLE LIST OF LABELS-------------------------------------------------------*/

typedef struct label
{
    char labelText[65];
    int lineNumber;
    int left;
} Label;

typedef struct node
{
    Label label;
    struct node* next;
}Node;

/*STATUS OF THE ASSEMBLER-----------------------------------------------------*/

typedef struct status
{
    long actualLine;    /*Linha do montador*/
    int left;           /*Posicao atual da linha (direita ou esquerda)*/

    char memoryMap[1024][10];

    Node* listLabels;   /*Lista dos labes*/

    int label;          /*Salva se ja foi digitado um label*/
    int cmdOrDir;       /*Sala se ja foi digitado um comando ou diretriz*/
    int error;          /*Verifica se ja deu algum erro*/
} Status;


/*METHODS*/

Node* createNode(char* labelText, int lineNumber, int left);

Node* addNode(char* labelText, int lineNumber, int left, Status* status);

Node* getNodeAux(char* labelText, Node* now);
Node* getNode(char* labelText, Status* status);

void freeNodes(Node* now);


/*METHODS*/

void initialize(Status* status);
/*incStatus = Avanca o contador do lado esquerdo ou direito da palavra e a linha
atual caso esteja na direita
status  = Status a ser montador a ser modificado*/
void incStatus(Status* status);
