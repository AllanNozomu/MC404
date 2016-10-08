#include "lists.h"

LabelNode* createLabelNode(char* labelText, int lineNumber, int left)
{
    LabelNode* node = malloc(sizeof(LabelNode));
    node->label.lineNumber = lineNumber;
    node->label.left = left;
    strcpy(node->label.labelText, labelText);
    node->next = NULL;
    return node;
}

LabelNode* addLabelNode(char* labelText, int lineNumber, int left, LabelNode* node)
{
    LabelNode* newNode = createLabelNode(labelText, lineNumber, left);

    newNode->next = node;
    return newNode;
}

LabelNode* getLabelNode(char* labelText, LabelNode* now)
{
    if (now == NULL)
        return NULL;
    if (strcmp(labelText, now->label.labelText) == 0)
        return now;
    return getLabelNode(labelText, now->next);
}

void freeLabelNodes(LabelNode* now)
{
    if (now == NULL)
        return;
    freeLabelNodes(now->next);

    free(now);
}

void printLabels(LabelNode* now)
{
    if (now == NULL){
        printf("NULL\n");
        return;
    }
    printf("%s %ld %s -> ", now->label.labelText, now->label.lineNumber, (now->label.left ? "Left" : "Rigth"));
    printLabels(now->next);
}


/*----------------------------------------------------------------------------*/



SymbolNode* createSymbolNode(char* symbolText, int value)
{
    SymbolNode* node = malloc(sizeof(SymbolNode));
    node->symbol.value = value;
    strcpy(node->symbol.symbolText, symbolText);
    node->next = NULL;
    return node;
}

SymbolNode* addSymbolNode(char* symbolText, int value, SymbolNode* node)
{
    SymbolNode* newNode = createSymbolNode(symbolText, value);

    newNode->next = node;
    return newNode;
}

SymbolNode* getSymbolNode(char* symbolText, SymbolNode* now)
{
    if (now == NULL)
        return NULL;
    if (strcmp(symbolText, now->symbol.symbolText) == 0)
        return now;
    return getSymbolNode(symbolText, now->next);
}

void freeSymbolNodes(SymbolNode* now)
{
    if (now == NULL)
        return;
    freeSymbolNodes(now->next);

    free(now);
}

void printSymbols(SymbolNode* now)
{
    if (now == NULL)
    {
        printf("NULL\n");
        return;
    }
    printf("%s %ld -> ", now->symbol.symbolText, now->symbol.value);
    printSymbols(now->next);
}
