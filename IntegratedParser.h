/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#include <stdio.h>
#include <stdlib.h>
#include "lexer.c"
#include "IntegratedParserDef.h"

void stackTreeFunction(alpha_node *reversedGrammar, Stack *s, int **parseTable, token_info lookAhead);

parseTree parseInputSourceCode(char *testcaseFile, int **parseTable, alpha_node *grammar, alpha_node *reversedGrammar);

Set *getSynchronisationSet(Stack *st);

void errorRecovery(Stack *st, token_info lookAhead);

void createParseTable(Set **firstSetArray, Set **followSetArray, alpha_node *grammar, int **parseTable);

void printParseTable(int **parseTable, alpha_node *grammar);

alpha_node insertFirstAlpha(alpha_node n, alpha_node headOfRule);

void equateAlphaNodes(alpha_node n1, alpha_node n2);

alpha_node *reverseGrammar(alpha_node *inputGrammar);

void recursivelyPrintParseTree(treeNode node, FILE *fp);

void printParseTree(parseTree pt, char *outFile);

void insertAtBeginning(ASTList list, ASTNode node);

void insertAtEnd(ASTList list, ASTNode node);


