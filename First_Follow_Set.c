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
#include <stdbool.h>
// #include "commons.h"
#include "setADT.h"

/*NOTE: Rules.txt should always have a '\n' before the EOF*/

/*
    Description of all the functions used:
    - get_token_key_parser(): returns the hash key for a given token
    - construct_hash_table_parser(): constructs a hash table of size GRAMMAR_TABLE_SIZE
    - populate_hash_table_parser(): populates the hash table with all the terminals and non terminals
    - print_alpha_list(): prints the hash table
    - getInformation(): returns the information about a given variable (terminal/non terminal) accesed in the hash table
    - createAlpha(): creates a new alpha_node with the description in the function parameters
    - checkForEpsilon(): checks if a given Chain of alpha_node(s) contains EPSILON (head is the parameter)
    - printAlpha(): prints a given Chain of alpha_node(s) with head as the parameter
    - getCopyOfChain(): returns a copy of a given Chain of alpha_node(s) to avoid pointer issues
    - removeDuplicates(): removes duplicates from a given Chain of alpha_node(s)
    - first_Set(): returns the first set of a given variable (terminal/non terminal)
    - getFirstSet(): returns the first set of a given non terminal
    - getFollowSet(): returns the follow set of a given non terminal
    - getFirstSetForRule(): returns the first set of a given rule
    - getSize(): returns the size of a given Set
    - printGrammar(): prints the grammar rules
    - printFirstSet(): prints the first sets of all the non terminals
    - displaySet(): prints the First or Follow set based on the flag
    - displayRuleWiseFirstSet(): prints the first sets of all the rules
    - createGrammarBase(): creates the grammar rules
*/

Set **set_first;       /*First sets of all the non terminals*/
Set **set_follow;      /*Follow sets of all the non terminals*/
Set **rule_wise_first; /*First sets of all the rules*/

/*
Daily Thoughts of a Programmer:
- I wonder if I will ever be able to write a code without any bugs
- I wish the debugger could speak to me better
- We really did spend 4 HOURS debugging a the Follow Set function
- I really wonder how I'll draw the equivalence between the token_info and the alpha_node
- Parsing is really hard
- Parse_Tree..... Ahhhh... Stress...!
- Man, the transition was not smooth at all
- Struggling a bit right now
- Teammates are really helpful
- Should I get a coffee? Or Sleep?
- I really need to get some sleep
- Man, I made such a stupid blunder in the First/Follow concept
- It was terrible, especially after all the pains I took to get them running
*/

enum non_terminals
{
    program,
    moduleDeclarations,
    moduleDeclaration,
    otherModules,
    driverModule,
    module,
    ret,
    input_plist,
    n1,
    output_plist,
    n2,
    dataType,
    range_arrays,
    index_arr,
    sign,
    new_index,
    type_,
    moduleDef,
    statements,
    statement,
    ioStmt,
    boolConstt,
    var_print,
    p1,
    simpleStmt,
    assignmentStmt,
    whichStmt,
    lvalueIDStmt,
    lvalueARRStmt,
    moduleReuseStmt,
    optional,
    actual_para_list,
    par_print,
    n_12,
    idList,
    n3,
    expression,
    u,
    new_NT,
    unary_op,
    arithmeticOrBooleanExpr,
    n7,
    anyTerm,
    n8,
    arithmeticExpr,
    n4,
    term,
    n5,
    factor,
    n_11,
    element_index_with_expressions,
    uarr,
    new_NT_arr,
    arrExpr,
    arr_N4,
    arrTerm,
    arr_N5,
    arrFactor,
    op1,
    op2,
    logicalOp,
    relationalOp,
    declareStmt,
    condionalStmt,
    caseStmts,
    n9,
    default_,
    value,
    iterativeStmt,
    range_for_loop,
    index_for_loop,
    new_index_for_loop,
    var_id_num
};
// enum non_terminals
// {
//     s,a,b,c
// };
typedef enum non_terminals non_terminals;

// enum terminals
// {
//     INTEGER, REAL, BOOLEAN, OF, ARRAY, START, END, DECLARE, MODULE, DRIVER,
//     PROGRAM, GET_VALUE, PRINT, USE, WITH, PARAMETERS, TRUE, FALSE, TAKES, INPUT,
//     RETURNS, AND, OR, FOR, IN,
//     SWITCH, CASE, BREAK, DEFAULT, WHILE, ID, NUM, RNUM, PLUS,
//     MINUS, MUL, DIV, LT, LE, GE, GT, EQ, NE, DEF, ENDDEF, DRIVERDEF, DRIVERENDDEF,
//     COLON, RANGEOP, SEMICOL, COMMA, ASSIGNOP, SQBO, SQBC, BO, BC, EPSILON, DOLLAR
// };
// enum terminals
// {
//     A,B,D,G,H,EPSILON,DOLLAR
// };
// typedef enum terminals terminals;

enum type
{
    TERMINAL,
    NON_TERMINAL
};
typedef enum type type;

Set *epsilon_set; /*Set containing non_terminals which derive epsilon*/

/*Global string arrays for terminals and non terminals*/
char *non_terminals_string[] = {"program", "moduleDeclarations", "moduleDeclaration", "otherModules", "driverModule", "module", "ret", "input_plist", "n1", "output_plist", "n2", "dataType", "range_arrays", "index_arr", "sign", "new_index", "type_", "moduleDef", "statements", "statement", "ioStmt", "boolConstt", "var_print", "p1", "simpleStmt", "assignmentStmt", "whichStmt", "lvalueIDStmt", "lvalueARRStmt", "moduleReuseStmt", "optional", "actual_para_list", "par_print", "n_12", "idList", "n3", "expression", "u", "new_NT", "unary_op", "arithmeticOrBooleanExpr", "n7", "anyTerm", "n8", "arithmeticExpr", "n4", "term", "n5", "factor", "n_11", "element_index_with_expressions", "uarr", "new_NT_arr", "arrExpr", "arr_N4", "arrTerm", "arr_N5", "arrFactor", "op1", "op2", "logicalOp", "relationalOp", "declareStmt", "condionalStmt", "caseStmts", "n9", "default_", "value", "iterativeStmt", "range_for_loop", "index_for_loop", "new_index_for_loop", "var_id_num"};
// char *non_terminals_string[] = {"s","a","b","c"};

char *terminals_string[] = {"INTEGER", "REAL", "BOOLEAN", "OF", "ARRAY", "START", "END", "DECLARE",
                            "MODULE", "DRIVER", "PROGRAM", "GET_VALUE", "PRINT", "USE", "WITH", "PARAMETERS", "TRUE", "FALSE",
                            "TAKES", "INPUT", "RETURNS", "AND", "OR", "FOR", "IN", "SWITCH", "CASE", "BREAK", "DEFAULT",
                            "WHILE", "ID", "NUM", "RNUM", "PLUS", "MINUS", "MUL",
                            "DIV", "LT", "LE", "GE", "GT", "EQ", "NE", "DEF", "ENDDEF", "DRIVERDEF", "DRIVERENDDEF",
                            "COLON", "RANGEOP", "SEMICOL", "COMMA", "ASSIGNOP", "SQBO", "SQBC", "BO", "BC", "EPSILON", "DOLLAR"};
// char *terminals_string[] = {"A","B","D","G","H","EPSILON","DOLLAR"};
/*
Structure for one variable of the grammar
- t: type of the variable (terminal/non terminal)
- var_name: name of the variable
- u: union type to store the terminal/non terminal value (based on the enum of terminals and non terminals)
- next: pointer to the next variable in the rule
*/
struct alpha_node
{
    type t;
    char var_name[VAR_LENGTH + 1];
    union
    {
        terminal_token term;
        non_terminals non_term;
    } u;
    struct alpha_node *next;
};
typedef struct alpha_node *alpha_node;

/*
- alpha_list is a linked list of alpha_nodes.
- Only to be used in constructing the hash table
*/
struct alpha_list
{
    alpha_node head;
    int size;
};
typedef struct alpha_list *alpha_list;

alpha_node *grammar;
alpha_node *reversedGrammar;
alpha_node *first_sets;

int get_token_key_parser(char *token, int begin, int forward)
{
    /*Self explanatory*/
    long key = 7;
    int scale_factor = 31;
    int i = begin;
    while (i < forward)
    {
        char c = *(token + i);
        key = key * scale_factor + c;
        i++;
    }
    if (key < 0)
        key = -key;
    return key % GRAMMAR_TABLE_SIZE;
}

alpha_list *construct_hash_table_parser()
{
    // Self explanatory
    alpha_list *hash_table = (alpha_list *)malloc(sizeof(alpha_list) * GRAMMAR_TABLE_SIZE);
    for (int i = 0; i < GRAMMAR_TABLE_SIZE; i++)
    {
        hash_table[i] = (alpha_list)malloc(sizeof(struct alpha_list));
        hash_table[i]->head = NULL;
        hash_table[i]->size = 0;
    }
    return hash_table;
}

alpha_list *populate_hash_table_parser(alpha_list *hash_table)
{
    /*First for loop for populating hash table with Non Terminals*/
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        int key = get_token_key_parser(non_terminals_string[i], 0, strlen(non_terminals_string[i]));
        // printf("%d, %s\n", key, non_terminals_string[i]);

        /*Create a new alpha_node*/
        alpha_node new_node = (alpha_node)malloc(sizeof(struct alpha_node));
        strcpy(new_node->var_name, non_terminals_string[i]);
        new_node->t = NON_TERMINAL;
        new_node->u.non_term = i;
        new_node->next = NULL;

        // printf("%d, %s\n", i, non_terminals_string[i]);

        if (hash_table[key]->head == NULL)
        {
            hash_table[key]->head = new_node;
            hash_table[key]->size++;
        }
        else
        {
            alpha_node t = hash_table[key]->head;
            while (t->next != NULL)
            {
                t = t->next;
            }
            t->next = new_node;
            hash_table[key]->size++;
        }
    }

    /*Second for loop for populating hash table with Terminals*/
    for (int i = 0; i < NUM_TERMINALS; i++)
    {
        int key = get_token_key_parser(terminals_string[i], 0, strlen(terminals_string[i]));
        // printf("%d, %s\n", key, terminals_string[i]);

        alpha_node new_node = (alpha_node)malloc(sizeof(struct alpha_node));
        strcpy(new_node->var_name, terminals_string[i]);
        new_node->t = TERMINAL;
        new_node->u.term = i;
        new_node->next = NULL;

        if (hash_table[key]->head == NULL)
        {
            hash_table[key]->head = new_node;
            hash_table[key]->size++;
        }
        else
        {
            alpha_node t = hash_table[key]->head;
            while (t->next != NULL)
            {
                t = t->next;
            }
            t->next = new_node;
            hash_table[key]->size++;
        }
    }
    return hash_table;
}

void print_alpha_list(alpha_list *hash_table)
{
    /*Self explanatory*/
    for (int i = 0; i < GRAMMAR_TABLE_SIZE; i++)
    {
        if (hash_table[i]->size == 0)
            continue;

        printf("Key: %d | Size: %d\n", i, hash_table[i]->size);
        alpha_node t = hash_table[i]->head;
        while (t != NULL)
        {
            printf("Var_name: %s | Type: %d", t->var_name, t->t);
            if (t->t == TERMINAL)
            {
                printf(" | Terminal: %d\n", t->u.term);
            }
            else
            {
                printf(" | Non_terminal: %d\n", t->u.non_term);
            }
            t = t->next;
        }
        printf("\n");
    }
}

alpha_node getInformation(char *token, int begin, int forward, alpha_list *hash_table)
{
    int key = get_token_key_parser(token, begin, forward);
    alpha_node t = hash_table[key]->head; /*Go to the head of the linked list at the index*/
    while (t != NULL)
    {
        if (strcmp(t->var_name, token) == 0)
        {
            return t;
        }
        t = t->next; /*Go to the next node*/
    }
    return NULL;
}

alpha_node createAlpha(type t, terminal_token term, non_terminals non_term, char *var_name)
{
    /*Creates a new alpha_node from the given parameters*/
    alpha_node temp = (alpha_node)malloc(sizeof(struct alpha_node));
    temp->t = t;
    strcpy(temp->var_name, var_name);
    if (t == TERMINAL)
        temp->u.term = term;
    else
        temp->u.non_term = non_term;
    temp->next = NULL;
    return temp;
}

bool checkForEpsilon(alpha_node temp)
{
    /*Self explanatory*/
    while (temp != NULL)
    {
        if (temp->t == TERMINAL && temp->u.term == EPSILON)
            return true;
        temp = temp->next;
    }
    return false;
}

void printAlpha(alpha_node temp)
{
    /*Prints the chain of alpha_nodes starting from temp*/
    while (temp != NULL)
    {
        if (temp->t == TERMINAL)
            printf("[%d,%s]", temp->u.term, terminals_string[temp->u.term]);
        else
            printf("[%d,%s]", temp->u.non_term, non_terminals_string[temp->u.non_term]);
        temp = temp->next;
    }
    printf("\n");
}

void printAlphaInfo(alpha_node temp)
{
    printf("Type: %d | ", temp->t);
    if (temp->t == TERMINAL)
        printf("Terminal: %s\n", terminals_string[temp->u.term]);
    else
        printf("Non_terminal: %s\n", non_terminals_string[temp->u.non_term]);
    return;
}

alpha_node getCopyOfChain(alpha_node head)
{
    /*
    - Returns a copy of the chain of alpha_nodes starting from head
    - Mainly done to avoid pointer issues and circular references
    */
    alpha_node temp = head;
    alpha_node new_head = malloc(sizeof(struct alpha_node));
    new_head = NULL;
    while (temp != NULL)
    {
        alpha_node temp1 = createAlpha(temp->t, temp->u.term, temp->u.non_term, temp->var_name);
        if (new_head == NULL)
        {
            new_head = temp1;
        }
        else
        {
            alpha_node pointer = new_head;
            while (pointer->next != NULL)
                pointer = pointer->next;
            pointer->next = temp1;
        }
        temp = temp->next;
    }
    free(temp);
    return new_head;
}

alpha_node removeDuplicates(alpha_node head)
{
    /*Removes duplicates from the chain of alpha_nodes starting from head*/
    alpha_node temp = head;
    int *e = (int *)malloc(sizeof(int) * NUM_TERMINALS);

    while (temp != NULL)
    {
        e[temp->u.term] += 1;
        temp = temp->next;
    }

    alpha_node new_head = malloc(sizeof(struct alpha_node));
    new_head = NULL;

    for (int i = 0; i < 6; i++)
    {
        if (e[i] > 0)
        {
            alpha_node temp1 = createAlpha(TERMINAL, i, 0, terminals_string[i]);
            if (new_head == NULL)
            {
                new_head = temp1;
            }
            else
            {
                alpha_node pointer = new_head;
                while (pointer->next != NULL)
                    pointer = pointer->next;
                pointer->next = temp1;
            }
        }
    }
    free(temp);
    return new_head;
}

alpha_node first_set(alpha_node *grammar, non_terminals x)
{
    /*head is the head of the linked list that will be returned*/
    alpha_node head = malloc(sizeof(struct alpha_node));
    head = NULL;

    for (int i = 0; i < NUM_RULES; i++) /*For each rule*/
    {
        if (grammar[i]->u.non_term == x) /*Only if the LHS of the rule is x*/
        {
            alpha_node temp = grammar[i]->next; /*temp is the RHS of the rule*/
            /*
            - If the RHS is a terminal, then add it to the linked list
            - If the RHS is a non-terminal, then add the first set of that non-terminal to the linked list
            */
            if (temp->t == TERMINAL)
            {
                alpha_node temp1 = createAlpha(TERMINAL, temp->u.term, 0, temp->var_name);
                if (head == NULL) /*If the linked list is empty*/
                {
                    head = temp1;
                }
                else /*If the linked list is not empty*/
                {
                    /*Go to the end of the linked list and add the new node*/
                    alpha_node pointer = head;
                    while (pointer->next != NULL)
                        pointer = pointer->next;
                    pointer->next = temp1;
                }
                // printAlpha(head);
            }
            else
            {
                if (temp->u.non_term == x) /*If the RHS is a non-terminal and it is the same as the LHS, then Ignore (it creates infinite recuursion)*/
                    continue;

                alpha_node temp1 = NULL;

                /*If the first set of the non-terminal has already been calculated, then use it*/
                if (first_sets[temp->u.non_term] == NULL)
                    temp1 = first_set(grammar, temp->u.non_term);
                else
                    temp1 = first_sets[temp->u.non_term];

                alpha_node t1 = getCopyOfChain(temp1); /*Make a copy of the first set of the non-terminal to avoid pointer issues*/

                if (head == NULL) /*If the linked list is empty*/
                {
                    head = t1;
                }
                else /*If the linked list is not empty*/
                {
                    /*Go to the end of the linked list and add the new node*/
                    alpha_node pointer = head;
                    while (pointer->next != NULL)
                        pointer = pointer->next;
                    pointer->next = t1;
                }

                /*
                - The while loop below checks if the first set of the non-terminal contains epsilon
                - If it does, then it adds the first set of the next non-terminal on RHS of x to the linked list
                - It continues doing this as long as the first set of the non-terminal contains epsilon
                - If it reaches end of the RHS of x, then the epsilon remains
                */
                while (checkForEpsilon(temp1) && temp->next != NULL) /*As long as the first set contains Epsilon and RHS is not over*/
                {
                    temp = temp->next;
                    if (temp->t == TERMINAL)
                    {
                        /*Same logic as above*/
                        temp1 = createAlpha(TERMINAL, temp->u.term, 0, temp->var_name);
                        alpha_node pointer = head;
                        while (pointer->next != NULL)
                            pointer = pointer->next;
                        pointer->next = temp1;
                    }
                    else
                    {
                        if (temp->u.non_term == x) /*Same logic as above*/
                            continue;

                        temp1 = NULL;
                        /*Same logic as above*/
                        if (first_sets[temp->u.non_term] == NULL)
                            temp1 = first_set(grammar, temp->u.non_term);
                        else
                            temp1 = first_sets[temp->u.non_term];

                        alpha_node t1 = getCopyOfChain(temp1);

                        alpha_node pointer = head;
                        // int j=0;
                        while (pointer->next != NULL) // && j<4)
                        {
                            pointer = pointer->next;
                            // j++;
                            // printf("pointer = %d\n",pointer->u.term);
                        }
                        pointer->next = t1;
                        temp1 = t1;
                    }
                }
                temp1 = NULL;
                free(temp1);
            }
            free(temp);
        }
    }
    return head; /*Return the linked list of terminals*/
}

int getSize(int *a, int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (a[i] != 0)
            count++;
    }
    return count;
}

int *getFirstSet(alpha_node *grammar, non_terminals x)
{
    /*This function returns the first set of a non-terminal*/
    int *set = malloc(sizeof(int) * NUM_TERMINALS);

    for (int i = 0; i < NUM_RULES; i++) /*For each rule*/
    {
        if (grammar[i]->u.non_term == x) /*Only if the LHS of the rule is x*/
        {
            alpha_node temp = grammar[i]->next; /*temp is the RHS of the rule*/
            /*
            - If the RHS is a terminal, then add it to the set
            - If the RHS is a non-terminal, then add the first set of that non-terminal to the set
            */
            if (temp->t == TERMINAL)
            {
                set[temp->u.term] = 1;
            }
            else
            {
                if (temp->u.non_term == x) /*If the RHS is a non-terminal and it is the same as the LHS, then Ignore (it creates infinite recursion)*/
                    continue;

                int *temp1 = malloc(sizeof(int) * NUM_TERMINALS);

                /*If the first set of the non-terminal has already been calculated, then use it*/
                if (set_first[temp->u.non_term] == NULL)
                    temp1 = getFirstSet(grammar, temp->u.non_term);
                else
                    temp1 = set_first[temp->u.non_term]->array;

                /*Add the first set of the non-terminal to the set*/
                for (int i = 0; i < NUM_TERMINALS; i++)
                {
                    if (temp1[i] != 0)
                        set[i] = 1;
                }

                /*
                - The while loop below checks if the first set of the non-terminal contains epsilon
                - If it does, then it adds the first set of the next non-terminal on RHS of x to the set
                - It continues doing this as long as the first set of the non-terminal contains epsilon
                - If it reaches end of the RHS of x, then the epsilon remains
                */
                while (isInSet(epsilon_set, temp->u.non_term) && temp->next != NULL) /*As long as the first set contains Epsilon and RHS is not over*/
                {
                    temp = temp->next;
                    if (temp->t == TERMINAL)
                    {
                        /*Same logic as above*/
                        set[temp->u.term] = 1;
                    }
                    else
                    {
                        if (temp->u.non_term == x) /*Same logic as above*/
                            continue;

                        temp1 = NULL;
                        /*Same logic as above*/
                        if (set_first[temp->u.non_term] == NULL)
                            temp1 = getFirstSet(grammar, temp->u.non_term);
                        else
                            temp1 = set_first[temp->u.non_term]->array;

                        // printf("nt = %s\n",non_terminals_string[temp->u.non_term]);

                        for (int i = 0; i < NUM_TERMINALS; i++)
                        {
                            if (temp1[i] != 0)
                                set[i] = 1;
                        }
                    }
                }
            }
        }
    }

    set[EPSILON] = 0; /*Remove epsilon from the set*/
    return set;
}

int *getFirstSetForRule(alpha_node lhs)
{
    int *set = malloc(sizeof(int) * NUM_TERMINALS);

    alpha_node temp = lhs->next; /*temp is the RHS of the rule*/

    if (temp->t == TERMINAL) /*If the RHS is a terminal, then add it to the set and return*/
    {
        set[temp->u.term] = 1;
        return set;
    }

    int *first = malloc(sizeof(int) * NUM_TERMINALS);
    first = set_first[temp->u.non_term]->array;
    /*The first set of the non-terminal has already been calculated, then use it*/
    for (int i = 0; i < NUM_TERMINALS; i++)
    {
        if (first[i] != 0)
            set[i] = 1;
    }

    /*Keep adding the first set of the next non-terminal on RHS of x to the set as long as the first set of the non-terminal contains epsilon*/
    while (isInSet(epsilon_set, temp->u.non_term) && temp->next != NULL)
    {
        temp = temp->next;
        if (temp->t == TERMINAL)
        {
            set[temp->u.term] = 1;
            break;
        }
        first = set_first[temp->u.non_term]->array;
        for (int i = 0; i < NUM_TERMINALS; i++)
        {
            if (first[i] != 0)
                set[i] = 1;
        }
    }
    return set;
}

int *getFollowSet(alpha_node *grammar, non_terminals x)
{
    /*This function returns the follow set of a non-terminal*/
    int *set = malloc(sizeof(int) * NUM_TERMINALS);
    for (int i = 0; i < NUM_RULES; i++)
    {
        alpha_node head = grammar[i]; /*head is the LHS of the rule*/
        alpha_node temp = head->next; /*temp is the first of the RHS of the rule*/

        while (temp != NULL)
        {
            if (temp->t == NON_TERMINAL && temp->u.non_term == x) /*If the non-terminal is x*/
            {
                alpha_node temp1 = temp->next;
                // if (x > 19)
                // printf("%d,%d\n", temp1->u.term, temp1->u.non_term);
                if (temp1 != NULL) /*If x is not the last non-terminal on the RHS of the rule*/ // A->aA beta
                {
                    int flag = 0;                                                                                       /*flag to denote if the EPSILON still remains even after consuming all RHS of x*/
                    if (temp1->t == TERMINAL) /*If the next non-terminal is a terminal, then add it to the follow set*/ // beta is terminal
                    {
                        set[temp1->u.term] = 1;
                    }
                    else // beta is not terminal
                    {
                        int *first = malloc(sizeof(int) * NUM_TERMINALS);
                        first = set_first[temp1->u.non_term]->array; /*Get the first set of the next non-terminal*/
                        /*Add the first set of the next non-terminal to the follow set*/
                        for (int i = 0; i < NUM_TERMINALS; i++)
                        {
                            if (first[i] != 0)
                                set[i] = 1;
                        }
                        int term_flag = 0;
                        while (isInSet(epsilon_set, temp1->u.non_term) && temp1->next != NULL) /*As long as the first set contains Epsilon and RHS is not over*/ // beta has variables that have epsilon
                        {
                            temp1 = temp1->next;
                            if (temp1->t == TERMINAL)
                            {
                                /*Same logic as above*/
                                set[temp1->u.term] = 1;
                                term_flag = 1;
                                // flag=0;
                                break; /*If the next non-terminal is a terminal, then add it to the follow set and break*/
                            }
                            else
                            {
                                first = NULL;
                                /*Same logic as above*/
                                first = set_first[temp1->u.non_term]->array;
                            }

                            for (int i = 0; i < NUM_TERMINALS; i++)
                            {
                                if (first[i] != 0)
                                    set[i] = 1;
                            }
                            if (first[EPSILON] != 0)
                                flag = 1;
                        }

                        if (isInSet(epsilon_set, temp1->u.non_term)) /*Setting the flag if the first set of the next non-terminal contains EPSILON*/
                            flag = 1;

                        if (term_flag == 1) /*If the while loop is broken because the next non-terminal is a terminal, then set the flag to 0*/
                        {
                            flag = 0;
                            term_flag = 0;
                        }

                        if (flag == 1) /*If the flag is set, then add the follow set of the LHS of the rule to the follow set of x*/
                        {
                            int *follow = malloc(sizeof(int) * NUM_TERMINALS);
                            /*Same logic as above*/
                            if (set_follow[head->u.non_term] == NULL && head->u.non_term != x)
                                follow = getFollowSet(grammar, head->u.non_term);
                            else if (head->u.non_term == x)
                            {
                                temp = temp->next;
                                continue;
                            }
                            else
                                follow = set_follow[head->u.non_term]->array;
                            for (int i = 0; i < NUM_TERMINALS; i++)
                            {
                                if (follow[i] != 0)
                                    set[i] = 1;
                            }
                        }
                    }
                }
                else /*If x is the last non-terminal on the RHS of the rule then we take follow set of LHS of the rule*/
                {
                    int *follow = malloc(sizeof(int) * NUM_TERMINALS);

                    /*Same logic as above*/
                    /*
                    If the follow set of the LHS of the rule is not computed AND it is not the same as x (to avoid infinite recursion),
                    then compute it
                    Else if it is the same as x, then we move to the next terminal/non-terminal on the RHS of the rule
                    Else, we have already computed the follow set of the LHS of the rule
                    */
                    if (set_follow[head->u.non_term] == NULL && head->u.non_term != x)
                    {
                        follow = getFollowSet(grammar, head->u.non_term);
                    }
                    else if (head->u.non_term == x)
                    {
                        temp = temp->next;
                        continue;
                    }
                    else
                        follow = set_follow[head->u.non_term]->array;

                    /*Add the follow set of the LHS of the rule to the follow set of x*/
                    for (int i = 0; i < NUM_TERMINALS; i++)
                    {
                        if (follow[i] != 0)
                            set[i] = 1;
                    }
                }
            }
            temp = temp->next;
        }
    }
    return set;
}

void printGrammar(alpha_node *grammar, int n)
{
    /*
    - This function prints the grammar
    - Template:
        <non_terminal,enum for it> -> <terminal,enum for it> | <non_terminal,enum for it>
    */
    printf("[Grammar]:\n");
    for (int i = 0; i < n; i++)
    {
        // printf("i = %d\n",i);
        alpha_node temp = grammar[i];
        printf("[%s,%d] -> ", temp->var_name, temp->u.non_term);
        temp = temp->next;
        while (temp != NULL)
        {
            printf("[%s,", temp->var_name);
            if (temp->t == TERMINAL)
                printf("%d] ", temp->u.term);
            else
                printf("%d] ", temp->u.non_term);
            temp = temp->next;
        }
        printf("\n");
    }
    printf("\n");
}

Set *getEpsilonNonTerminals(alpha_node *grammar, int n)
{
    epsilon_set = malloc(sizeof(Set));
    epsilon_set->array = malloc(sizeof(int) * NUM_NON_TERMINALS);
    epsilon_set->size = 0;

    for (int i = 0; i < n; i++)
    {
        alpha_node lhs = grammar[i];
        alpha_node rhs = lhs->next;
        if (rhs->t == TERMINAL && rhs->u.term == EPSILON)
        {
            epsilon_set->array[lhs->u.non_term] = 1;
            epsilon_set->size++;
        }
    }
    return epsilon_set;
}

void printFirstSet(alpha_node *first_sets, int n)
{
    /*
    - This function prints the first set of all the non-terminals
    - Template:
        First(<non_terminal,enum for it>) = {(terminal,enum for it)}
    */
    printf("[First Sets]:");
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        if (first_sets[i] == NULL)
            continue;

        alpha_node temp = first_sets[i];
        printf("First(%s,%d) = {", temp->var_name, i);
        while (temp != NULL)
        {
            printf("(%s,%d) ", temp->var_name, temp->u.term);
            temp = temp->next;
        }
        printf("}\n");
    }
}

void displaySet(Set **set, int n, int flag)
{
    /*
    - This function prints the first set of all the non-terminals
    - Template:
        First(<non_terminal,enum for it>) = {(terminal,enum for it)}
    - flag = 0 -> First Set
    - flag = 1 -> Follow Set
    */
    if (flag == 0)
        printf("[First Sets]:");
    else
        printf("[Follow Sets]:");
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        if (set[i] == NULL)
            continue;

        if (flag == 0)
            printf("First[%s,%d][Size = %d] = [", non_terminals_string[i], i, set[i]->size);
        else
            printf("Follow[%s,%d][Size = %d] = [", non_terminals_string[i], i, set[i]->size);

        for (int j = 0; j < NUM_TERMINALS; j++)
        {
            if (set[i]->array[j] == 1)
                printf("[%s,%d] ", terminals_string[j], j);
        }
        printf("]\n");
    }
    printf("\n\n");
}

void displayRuleWiseFirstSet(Set **set, int n)
{
    printf("[Rule Wise First Sets]:\n");
    for (int i = 0; i < n; i++)
    {
        printf("Rule [%d][Size = %d]: [", i, set[i]->size);
        for (int j = 0; j < NUM_TERMINALS; j++)
        {
            if (rule_wise_first[i]->array[j] != 0)
            {
                printf(" [%s,%d]", terminals_string[j], j);
            }
        }
        printf(" ]\n");
    }
    printf("\n");
}

alpha_node *createGrammarBase(FILE *fp, alpha_list *hash_table)
{
    alpha_node *grammar = (alpha_node *)malloc(NUM_RULES * sizeof(alpha_node));
    char *variable = malloc((VAR_LENGTH + 1) * sizeof(char));
    int rule_num = 0; /*This variable tracks the rule number*/
    int var_id = 0;   /*This variable stores the index of the variable name*/

    while (!feof(fp))
    {
        char c = fgetc(fp);
        // printf("c = %c, var_id = %d\n", c, var_id);
        if (c == ' ')
        {
            /*
            - If the character is a space, then it means that the variable name has ended
            - So, we store the variable name in the variable array
            - Then, we get the information of the variable name from the hash table
            - We create a new node with the information (We don't directly use the returned node because we don't want to change the hash table)
            - We add the node to the grammar (with index as the rule number)
            */
            if (var_id != 0)
            {
                variable[var_id] = '\0';
                // printf("var=%s\n",variable);
                alpha_node info = getInformation(variable, 0, var_id, hash_table);
                // printf("Returned from getInformation\n");
                alpha_node copy_info = NULL;

                if (info->t == NON_TERMINAL)
                {
                    copy_info = createAlpha(info->t, 0, info->u.non_term, info->var_name);
                }
                else
                {
                    // printf("info->t = %d\n",info->t);
                    copy_info = createAlpha(info->t, info->u.term, 0, info->var_name);
                    // printf("copy_info->t = %d\n", copy_info->t);
                }

                if (grammar[rule_num] == NULL)
                {
                    grammar[rule_num] = copy_info;
                }
                else
                {
                    alpha_node pointer = grammar[rule_num];
                    while (pointer->next != NULL)
                        pointer = pointer->next;
                    pointer->next = copy_info;
                }

                var_id = 0; /*Reset the variable id*/
            }
            else /*If var_id is 0, then it means that the variable name is empty*/
            {
                continue;
            }
        }
        else if (c == ':') /*Colon indicated crossing from LHS to RHS of rule. So we just continue*/
        {
            continue;
        }
        else if (c == '\n')
        {
            /*
            - If the character is a space, then it means that the variable name has ended
            - So, we store the variable name in the variable array
            - Then, we get the information of the variable name from the hash table
            - We create a new node with the information (We don't directly use the returned node because we don't want to change the hash table)
            - We add the node to the grammar (with index as the rule number)
            */
            if (var_id != 0)
            {
                variable[var_id] = '\0';
                // printf("var=%s\n",variable);
                alpha_node info = getInformation(variable, 0, var_id, hash_table);
                alpha_node copy_info = NULL;

                if (info->t == NON_TERMINAL)
                {
                    copy_info = createAlpha(info->t, 0, info->u.non_term, info->var_name);
                }
                else
                {
                    // printf("info->t = %d\n",info->t);
                    copy_info = createAlpha(info->t, info->u.term, 0, info->var_name);
                }
                // createAlpha(info->t,info->u.term,info->u.non_term);

                if (grammar[rule_num] == NULL)
                {
                    grammar[rule_num] = copy_info;
                }
                else
                {
                    alpha_node pointer = grammar[rule_num];
                    while (pointer->next != NULL)
                        pointer = pointer->next;
                    pointer->next = copy_info;
                }

                var_id = 0; /*Reset the variable id*/
                rule_num++; /*Increment the rule number as we have reached the end of the rule*/
            }
            else
            {
                rule_num++; /*Increment the rule number as we have reached the end of the rule*/
                continue;   /*If var_id is 0, then it means that the variable name is empty*/
            }
        }
        else if (c == '\r') /*This is a carriage return character. So we just continue*/
        {
            continue;
        }
        else if (c == EOF) /*This is the end of the file. So we just break*/
        {
            break;
        }
        else
        {
            variable[var_id] = c; /*If the character is not any of the above, then it is a part of the variable name*/
            var_id++;             /*So we add it to the variable array*/
        }
    }
    return grammar; /*Return the grammar*/
}
// int main()
// {
//     alpha_list *hash_table = construct_hash_table_parser();
//     hash_table = populate_hash_table_parser(hash_table);
//     print_alpha_list(hash_table);

//     FILE *fp = fopen("grammar_modified.txt", "r");

//     if (fp == NULL)
//         printf("File is NULL\n");

//     grammar = malloc(NUM_RULES * sizeof(alpha_node));
//     grammar = createGrammarBase(fp, hash_table);
//     // printf("\n check 1");
//     if (grammar == NULL)
//         printf("Grammar is NULL\n");

//     printGrammar(grammar, NUM_RULES);

//     /*
//     - All the tasks done below this point make use of the Set ADT
//     - The Set ADT is defined in the file set.h
//     - Set essentially has two fields:
//         - array: This is an array of integers. The array is of size NUM_TERMINALS
//         - size: This is the number of elements in the array
//     */

//     set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
//     for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
//     {
//         set_first[i] = (Set *)malloc(sizeof(Set));
//         set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
//         set_first[i]->array = getFirstSet(grammar, i);
//         set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
//     }

//     displaySet(set_first, NUM_NON_TERMINALS, 0);

//     rule_wise_first = (Set**)malloc(NUM_RULES*sizeof(Set*));

//     for(int i=0;i<NUM_RULES;i++)
//     {
//         rule_wise_first[i] = (Set*)malloc(sizeof(Set));
//         rule_wise_first[i]->array = (int*)malloc(NUM_TERMINALS * sizeof(int));
//         rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
//         rule_wise_first[i]->size = getSize(rule_wise_first[i]->array,NUM_TERMINALS);
//     }

//     displayRuleWiseFirstSet(rule_wise_first,NUM_RULES);

//     set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
//     set_follow[program] = (Set *)malloc(sizeof(Set));
//     set_follow[program]->array = malloc(NUM_TERMINALS * sizeof(int));
//     set_follow[program]->array[DOLLAR] = 1;
//     set_follow[program]->size = 1;

//     for (int i = 1; i < NUM_NON_TERMINALS; i++)
//     {
//         set_follow[i] = (Set *)malloc(sizeof(Set));
//         set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
//         set_follow[i]->array = getFollowSet(grammar, i);
//         set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
//     }

//     displaySet(set_follow, NUM_NON_TERMINALS, 1);

//     fclose(fp);

//     /*Free the memory*/
//     for (int i = 0; i < NUM_RULES; i++)
//     {
//         free(grammar[i]);
//         free(rule_wise_first[i]);
//     }
//     free(grammar);

//     for (int i = 0; i < NUM_NON_TERMINALS; i++)
//     {
//         free(set_first[i]);
//         free(set_follow[i]);
//     }

//     free(rule_wise_first);
//     free(set_first);
//     free(set_follow);

//     return 0;
// }
