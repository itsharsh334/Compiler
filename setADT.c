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
#include "setADT.h"
// #define NUM_T 20     // Number of terminals or whatever, this will be used in set operations; there will be multiple such sizes set for different set types.



int isInSet(Set* s, int x) 
{
    return s->array[x];
}

void insertIntoSet(Set* s, int x) {
    if(!isInSet(s,x))
    {
        s->array[x] = 1;
        s->size++;
    }
}

void removeFromSet(Set* s, int x) {
    if(isInSet(s,x))
    {
        s->array[x] = 0;
        s->size--;
    }
}

Set* createIntersection(Set* s1, Set* s2)
{
    Set* s3 = (Set*)malloc(sizeof(Set));
    s3->array = (int*)malloc(NUM_TERMINALS * sizeof(int));
    s3->size = 0;
    for (int i = 0; i < NUM_TERMINALS; i++) {
        if (s1->array[i] == 1 && s2->array[i] == 1) {
            s3->array[i] = 1;
            s3->size++;
        }
        else {
            s3->array[i] = 0;
        }
    }
    return s3;
}

Set* createUnion(Set* s1, Set* s2)
{
    Set* s3 = (Set*)malloc(sizeof(Set));
    s3->array = (int*)malloc(NUM_TERMINALS * sizeof(int));
    s3->size = 0;
    for (int i = 0; i < NUM_TERMINALS; i++) {
        if (s1->array[i] == 1 || s2->array[i] == 1) {
            s3->array[i] = 1;
            s3->size++;
        }
        else {
            s3->array[i] = 0;
        }
    }
    return s3;
}

void printSet(Set* s)
{
    printf("Set: ");
    for (int i = 0; i < NUM_TERMINALS; i++) 
    {
        if (s->array[i] == 1) {
            printf("%d ", i);
        }
    }
    printf("\n");
}

// Driver code to test the above functions
// int main(void)
// {
//     Set **x;
//     x = (Set**)malloc(2 * sizeof(Set*));
//     x[0] = (Set*)malloc(sizeof(Set));

//     if(x[0] == NULL)
//     {
//         printf("Error in allocating memory for set\n");
//         exit(1);
//     }
//     if(x[1] == NULL)
//     {
//         printf("Not yet allocated\n");
//     }

//     Set* s1 = (Set*)malloc(sizeof(Set));
//     // initialize the set
//     s1->array = (int*)malloc(NUM_TERMINALS * sizeof(int));
//     s1->size = 0;
//     for (int i = 0; i < NUM_TERMINALS; i++) {
//         s1->array[i] = 0;
//     }
//     // insert some elements
//     insertIntoSet(s1, 1);
//     insertIntoSet(s1, 3);
//     insertIntoSet(s1, 5);

//     Set* s2 = (Set*)malloc(sizeof(Set));
//     // initialize the set
//     s2->array = (int*)malloc(NUM_TERMINALS * sizeof(int));
//     s2->size = 0;
//     for (int i = 0; i < NUM_TERMINALS; i++) 
//     {
//         s2->array[i] = 1;
//     }

//     removeFromSet(s2, 3);
//     removeFromSet(s2, 15);

//     // create the union
//     Set* s3 = createUnion(s1, s2);

//     // print the union
//     printSet(s3);

//     // create intersection
//     Set* s4 = createIntersection(s1, s2);

//     // print the intersection
//     printSet(s4);
// }
