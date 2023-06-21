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
#include <stdbool.h>
#include <string.h>
#include "lexerDef.h"
#include "macros.h"

token_info getNextToken(FILE *fp, hash_linked_list *hash_table, int *begin, int *forward);

hash_linked_list *construct_hash_table();

hash_linked_list *populate_hash_table(hash_linked_list *hash_table);

void fillBuffers(FILE *fp);

void printBuffer();

void printToken(token_info token);

int isAlphabet(char c);

int isDigit(char c);

void removeComments(char *testcaseFile, char *cleanFile);

int get_token_key(char *token, int begin, int forward);

void print_hash_table(hash_linked_list *hash_table);

int check_for_keyword(char *token, int begin, int forward, hash_linked_list *hash_table);

FILE *getStream(FILE *fp);
