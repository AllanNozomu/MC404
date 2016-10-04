#include "status.h"

Node* createNode(char* labelText, int lineNumber, int left)
{
    Node* node = malloc(sizeof(Node));
    node->label.lineNumber = lineNumber;
    node->label.left = left;
    strcpy(node->label.labelText, labelText);
    node->next = NULL;
    return node;
}

Node* addNode(char* labelText, int lineNumber, int left, Status* status)
{
    Node* newNode = createNode(labelText, lineNumber, left);

    newNode->next = status->listLabels;
    status->listLabels = newNode;
}

Node* getNodeAux(char* labelText, Node* now)
{
    if (now == NULL)
        return NULL;
    if (strcmp(labelText, now->label.labelText) == 0)
        return now;
    return getNodeAux(labelText, now->next);
}


Node* getNode(char* labelText, Status* status)
{
    return getNodeAux(labelText, status->listLabels);
}

void freeNodes(Node* now)
{
    if (now == NULL)
        return;
    freeNodes(now->next);

    free(now);
}

/*ASSEMBLER METHODS-----------------------------------------------------------*/

void initialize(Status* status)
{
    status->actualLine = 0;
    status->left = 1;
    status->error = 0;
    status->listLabels = NULL;
    for (int i = 0 ; i < 1024; ++i)
        for (int j = 0 ; j < 10; ++j)
            status->memoryMap[i][j] = ' ';
}

/*incStatus = Avanca o contador do lado esquerdo ou direito da palavra e a linha
atual caso esteja na direita
status  = Status a ser montador a ser modificado*/
void incStatus(Status* status)
{
    if (!status->left)
        ++status->actualLine;
    status->left = !status->left;
}
