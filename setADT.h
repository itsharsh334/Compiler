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
#include "commons.h"

typedef struct set 
{
    int* array;     // This is an array of bits, whereby 0 represents the absence of an element and 1 represents its presence
    int size;       // Number of elements that are in the set
} Set;

int isInSet(Set* s, int x) ;

void insertIntoSet(Set* s, int x);

void removeFromSet(Set* s, int x);

Set* createIntersection(Set* s1, Set* s2);

Set* createUnion(Set* s1, Set* s2);

void printSet(Set* s);
