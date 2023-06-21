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
#include <string.h>
#include "First_Follow_Set.c"

// # define VAR_LENGTH 32
// # define NUM_RULES 134
// # define NUM_NON_TERMINALS 71
// # define NUM_TERMINALS 58

// enum non_terminals {program, moduleDeclarations, moduleDeclaration, otherModules, driverModule, module,
//                     ret, input_plist, n1, output_plist, n2, dataType, range_arrays, index_arr, sign,
//                     new_index, type_, moduleDef, statements, statement, ioStmt, boolConstt, id_num_rnum, var_print,
//                     p1, simpleStmt, assignmentStmt, whichStmt, lvalueIDStmt, lvalueARRStmt, moduleReuseStmt,
//                     optional, idList, n3, expression, u, new_NT, unary_op, arithmeticOrBooleanExpr, n7, anyTerm, n8,
//                     arithmeticExpr, n4, term, n5, factor, n_11, element_index_with_expressions, n_10, arrExpr, arr_N4,
//                     arrTerm, arr_N5, arrFactor, op1, op2, logicalOp, relationalOp, declareStmt, condionalStmt, caseStmts,
//                     n9, default_, value, iterativeStmt, range_for_loop, index_for_loop, new_index_for_loop, sign_for_loop,var_id_num};

// typedef enum non_terminals non_terminals;

// enum terminals {EPSILON, INTEGER, REAL, BOOLEAN, OF, ARRAY, START, END, DECLARE,
//                 MODULE, DRIVER, PROGRAM, GET_VALUE, PRINT, USE, WITH, PARAMETERS, TAKES, INPUT,
//                 RETURNS, FOR, IN, SWITCH, CASE, BREAK, DEFAULT, WHILE, ID, NUM, RNUM, AND, OR,
//                 TRUE, FALSE, PLUS, MINUS, MUL, DIV, LT, LE, GE, GT, EQ, NE, DEF, ENDDEF, DRIVERDEF,
//                 DRIVERENDDEF, COLON, RANGEOP, SEMICOL, COMMA, ASSIGNOP, SQBO, SQBC, BO, BC, COMMENTMARK};

// typedef enum terminals terminals;

// enum type {TERMINAL, NON_TERMINAL};
// typedef enum type type;
struct stackNode
{
    type t;
    union
    {
        terminal_token term;
        non_terminals non_term;
    } u;
    struct treeNode *correspondingTreeNode;
    struct stackNode *next;
};
typedef struct stackNode *stackNode;

struct linkedlist
{
    int count;
    struct stackNode *head;
    struct stackNode *tail; // We may need the tail pointer for a future function
};

typedef struct stack
{
    struct linkedlist *ll; // Every required data is present in the underlying linked list
} Stack;

/* Linked List Functions */

void insertFirst(struct stackNode *n, struct linkedlist *ll)
{
    struct stackNode *new = malloc(sizeof(struct stackNode));

    if (ll->count == 0)
    {
        ll->tail = n;
        ll->head = malloc(sizeof(struct stackNode));
        ll->head->t = n->t;
        ll->head->u = n->u;
        ll->head->correspondingTreeNode = n->correspondingTreeNode;
        ll->head->next = NULL;
        ll->count++;
        return;
    }
    new->t = n->t;
    new->u = n->u;
    new->correspondingTreeNode = n->correspondingTreeNode;
    new->next = ll->head;
    ll->head = new;
    ll->count++;
}

/* Stack Functions */

Stack *createStack() // Creates an empty stack
{
    Stack *s = malloc(sizeof(Stack));          // reserve space on heap for stack structure
    s->ll = malloc(sizeof(struct linkedlist)); // reserve space on heap for stack's linked list
    s->ll->count = 0;                          // obviously count is zero for a newly created stack
    s->ll->head = NULL;                        // head and tail pointers point nowhere at the start
    s->ll->tail = NULL;

    return s;
}

void push(struct stackNode *n, Stack *s)
{
    insertFirst(n, s->ll); // we maintain the top of the stack at the head of the constituting linked list
}

struct stackNode *pop(Stack *s)
{
    struct stackNode *temp = s->ll->head;
    s->ll->head = s->ll->head->next;
    temp->next = NULL;
    s->ll->count--;
    return temp;
}

struct stackNode *top(Stack *s)
{
    return s->ll->head; // Retrieve head data
}

void printStackNode(stackNode x)
{
    printf("Type: %d | ", x->t);
    if (x->t == TERMINAL)
    {
        printf("Number: %d | ", x->u.term);
        printf("Terminal: %s | \n", terminals_string[x->u.term]);
    }
    else
    {
        printf("Number: %d | ", x->u.non_term);
        printf("Non-Terminal: %s | \n", non_terminals_string[x->u.non_term]);
    }
}

void printStack(Stack *s)
{
    printf("Current Stack Size: %d\n", s->ll->count);
    struct stackNode *current = s->ll->head;
    while (current != NULL)
    {
        printStackNode(current);
        current = current->next;
    }
}

int isEmpty(Stack *s)
{
    return s->ll->count == 0;
}

// Driver program to test the working of the stack
// int main(void)
// {
//     Stack* s = createStack();

//     struct stackNode* x = malloc(sizeof(struct stackNode));
//     x -> t = NON_TERMINAL;
//     x -> u.non_term = program;
//     x -> correspondingTreeNode = 123;
//     push(x, s);

//     struct stackNode* y = malloc(sizeof(struct stackNode));
//     y -> t = TERMINAL;
//     y -> u.term = INTEGER;
//     push(y, s);

//     struct stackNode* z = top(s);
//     printf("Topped element: %d %d\n", x->t, x->correspondingTreeNode);

// struct stackNode* popped = pop(s)
// printf("Peek element: %d\n", top(s));
// printf("Is stack empty: %d\n", isEmpty(s));
// }
