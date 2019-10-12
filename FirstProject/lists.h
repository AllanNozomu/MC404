#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*LISTA SIMPLES DE LABELS-----------------------------------------------------*/

typedef struct label
{
    char labelText[65];
    int lineNumber;
    int left;
} Label;

typedef struct labelNode
{
    Label label;
    struct labelNode* next;
} LabelNode;

/*LISTA SIMPLES DE SIMBOLOS---------------------------------------------------*/

typedef struct symbol
{
    char symbolText[65];
    int value;
} Symbol;

typedef struct symbolNode
{
    Symbol symbol;
    struct symbolNode* next;
} SymbolNode;


/*Metodos dos rotulos*/
/*Retorna um label novo criado*/
LabelNode* createLabelNode(char* labelText, int lineNumber, int left);
/*Adiciona no comeco da lista APENAS, nao quis fazer metodos de lista mais
complexos porque nao achei que era o foco do trabalho*/
LabelNode* addLabelNode(char* labelText, int lineNumber, int left, LabelNode* node);
/*Procura pelo label e o retorna*/
LabelNode* getLabelNode(char* labelText, LabelNode* now);

void freeLabelNodes(LabelNode* now);


/*Metodo usado apenas para debug*/
void printLabels(LabelNode* now);

/*Metodos dos simbolos*/
/*Meotods semelhantes ao de rotulos*/
SymbolNode* createSymbolNode(char* symbolText, int value);
SymbolNode* addSymbolNode(char* symbolText, int value, SymbolNode* node);
SymbolNode* getSymbolNode(char* symbolText, SymbolNode* now);

void freeSymbolNodes(SymbolNode* now);

/*Metodo usado apenas para debug*/
void printSymbols(SymbolNode* now);
