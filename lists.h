#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*LISTA SIMPLES DE LABELS-----------------------------------------------------*/

typedef struct label
{
    char labelText[65];
    long lineNumber;
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
    long value;
} Symbol;

typedef struct symbolNode
{
    Symbol symbol;
    struct symbolNode* next;
} SymbolNode;

/*Metodos dos rotulos*/

LabelNode* createLabelNode(char* labelText, int lineNumber, int left);
LabelNode* addLabelNode(char* labelText, int lineNumber, int left, LabelNode* node);
LabelNode* getLabelNode(char* labelText, LabelNode* now);

void freeLabelNodes(LabelNode* now);

void printLabels(LabelNode* now);

/*Metodos dos simbolos*/

SymbolNode* createSymbolNode(char* symbolText, int value);
SymbolNode* addSymbolNode(char* symbolText, int value, SymbolNode* node);
SymbolNode* getSymbolNode(char* symbolText, SymbolNode* now);

void freeSymbolNodes(SymbolNode* now);

void printSymbols(SymbolNode* now);
