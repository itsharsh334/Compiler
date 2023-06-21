/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#include "lexer.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int syntax_error = 0;

struct parseTreeNodeData
{
    char *lexeme;
    int lineNo;
    char *tokenName;
    float valueIfNumber;
    char *currentNodeSymbol;
    char *parentNodeSymbol;
    int isLeafNode;
    type t;
    union
    {
        terminal_token term;
        non_terminals non_term;
    } u;
    int rule_num;
};
typedef struct parseTreeNodeData *parseTreeNodeData;

// We unite alphanode and tokeninfo for our parse tree node because the parse tree node data can be taken from a grammar rule as well as from the lookahead token that is returned by the lexer.

struct alpha_and_token
{
    alpha_node alpha;
    token_info token;
};
typedef struct alpha_and_token *alpha_and_token;

// This is the structure of our parse tree node

struct treeNode
{
    alpha_and_token data;
    struct parseTreeNodeData *dataForPrint;
    int numOfChildren;

    // The following treeNode pointers are necessary to enable its traversal

    struct treeNode *leftmostChild;
    struct treeNode *secondLeftmostChild;
    struct treeNode *rightSibling;
    int whetherFirstChild;

    // The following data is needed for the AST creation
    int rule_num;
    struct ASTNode *addr;
    struct ASTNode *synaddr;
    struct ASTNode *inhaddr;
};
typedef struct treeNode *treeNode;

// The following abstracted structure gives us the root of our parse tree, which will be used by our traversal functions

struct parseTree
{
    struct treeNode *root;
};
typedef struct parseTree *parseTree;

struct ASTList
{
    int size;
    struct ASTListNode *head;
    struct ASTListNode *tail;
};
typedef struct ASTList *ASTList;

struct ASTListNode
{
    struct ASTNode *data;
    struct ASTListNode *next;
};
typedef struct ASTListNode *ASTListNode;

struct ASTNode
{
    struct parseTreeNodeData *dataForPrint;
    int num_children;
    struct ASTList *info_list;
    struct ASTNode **children;
    int range_ast[2];
};
typedef struct ASTNode *ASTNode;

struct AST
{
    ASTNode root;
};
typedef struct AST *AST;

void insertAtBeginning(ASTList list, ASTNode node)
{
    ASTListNode new_node = malloc(sizeof(struct ASTListNode));
    new_node->data = node;
    new_node->next = NULL;

    if (list->head == NULL)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        new_node->next = list->head;
        list->head = new_node;
    }
    list->size++;
}

void insertAtEnd(ASTList list, ASTNode node)
{
    if (node == NULL)
        return;

    ASTListNode new_node = malloc(sizeof(struct ASTListNode));
    new_node->data = node;
    new_node->next = NULL;

    if (list->size == 0)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        list->tail->next = new_node;
        list->tail = new_node;
        list->tail->next = NULL;
    }
    list->size++;
}

int last_rule_expanded = -1;
Set *getSynchronisationSet(stackNode st)
{
    Set *sync_set = malloc(sizeof(Set));

    sync_set = set_follow[st->u.non_term];

    return sync_set;
}

Stack *errorRecovery(Stack *st, token_info lookAhead, int *skip_flag, stackNode stacktop)
{
    if (stacktop->t == TERMINAL && stacktop->u.term != lookAhead->type)
    {
        // We have terminal mismatch and we need to pop the stack and continue parsing;

        stackNode sn = pop(st);
        free(sn); // doubtchange
        return st;
    }
    Set *sync_set = getSynchronisationSet(stacktop);
    if (isInSet(sync_set, lookAhead->type)) // Rule doesn't exist in the parse table
    {
        /*
            We pop the stack and continue parsing hoping that the next Stack symbol will contain the rule;
            stackNode sn = pop(st);
        */

        return st;
    }
    else
    {
        // I guess we just skip the current token and continue parsing :)

        *skip_flag = 1;
        return st;
    }
}

void stackTreeFunction(alpha_node *reversedGrammar, Stack *s, int **parseTable, token_info lookAhead)
{
    // Retrieve the top element of the stack to perform operations on it

    struct stackNode *stackTop = pop(s);

    /*
        If the popped element is a non-terminal, and neither have we have not run out of tokens to parse, nor have we received a junk token
        we want to push the RHS of the rule to thhe stack until we have a terminal hit. We are also dealing with the EPSILON stack popped symbol here.
    */

    int skip_flag = 0; // To check if we need to skip the current token or not
    while ((stackTop->t != TERMINAL || stackTop->u.term == EPSILON) && (lookAhead != NULL) && (lookAhead->type <= NUM_TERMINALS))
    {
        treeNode temp = NULL; // To keep track of right sibling of current tree node

        if (stackTop->t == TERMINAL && stackTop->u.term == EPSILON)
        {
            // Pop stack top element to check if even the next element is EPSILON, if so, keep popping until we don't find EPSILON

            stackTop->correspondingTreeNode->rule_num = -1;
            stackTop->correspondingTreeNode->dataForPrint->rule_num = -1;
            stackTop = pop(s);

            continue;
        }

        // Check the parse table to see which rule to apply by looking at the current non-terminal on the stack and the current lookahead terminal

        int ruleToApply = parseTable[stackTop->u.non_term][lookAhead->type];

        // If the rule to apply based on the parse table etry accesed does not exist, then we have to perform error recovery

        int flag_for_rule = 0;
        while (ruleToApply == -1)
        {
            syntax_error = 1;
            printf("Line %d: Error in the input\n", lookAhead->lineNum);

            s = errorRecovery(s, lookAhead, &skip_flag, stackTop);
            if (skip_flag == 1) // skip token
            {
                skip_flag = 0;
                push(stackTop, s);
                return;
            }
            stackTop = pop(s);
            // pop(s);
            if (stackTop->t == TERMINAL)
            {
                flag_for_rule = 1;
                break;
            }
            ruleToApply = parseTable[stackTop->u.non_term][lookAhead->type];
        }
        if (flag_for_rule == 1)
        {
            flag_for_rule = 0;
            break;
        }

        // Last node of the RHS of the rule to be applied. We use reversed grammar to make sure we are making our parsing left-associative.

        struct alpha_node *currentRule = reversedGrammar[ruleToApply]->next;

        // Initialise a counter that will look at the number of children the parent node that was popped from the stack will have.

        int count = 0;
        struct alpha_node *childrenCounter = currentRule;

        while (childrenCounter != NULL)
        {
            count++;
            childrenCounter = childrenCounter->next;
        }

        /*
            Declare the stack element that is the child of the parent node that was popped from the stack. This will be initialised using the
            rule travesal while loop below.
        */

        struct stackNode *newStackNode = malloc(sizeof(struct stackNode)); // Create first node to be pushed onto stack

        /*
            We wish to also keep track of the tree parent tree node from which the rules stem from. We will initialise its number of children
            first & second children using this pointer. Since it is a parent, it isn't a leaf node anymore, so we update that as well.
            It will also be later used to set the "parent node" pointer of the child tree node.
        */

        // treeNode parentTreeNode = malloc(sizeof(struct treeNode));
        treeNode parentTreeNode = stackTop->correspondingTreeNode;
        parentTreeNode->numOfChildren = count;
        parentTreeNode->dataForPrint->isLeafNode = 0;
        parentTreeNode->rule_num = ruleToApply;
        parentTreeNode->dataForPrint->rule_num = ruleToApply;

        // Traversal step of the rule.

        while (currentRule != NULL)
        {
            // Initialise the child stack node using the current rule entry (terminal or non-terminal, and the corresponding data)

            newStackNode->t = currentRule->t;
            newStackNode->u.non_term = currentRule->u.non_term;

            treeNode newTreeNode = malloc(sizeof(struct treeNode)); // Create a new tree node (the child node) to be used to initialise stack element

            // INITIALISE newTreeNode WITH DATA FROM currenRule entry

            newTreeNode->data = malloc(sizeof(struct alpha_and_token));
            newTreeNode->data->alpha = currentRule;
            newTreeNode->dataForPrint = malloc(sizeof(struct parseTreeNodeData));

            // Iff the current rule entry is a terminal, we want to initialise token related values of the corresponding tree node. Else set them to NULL.

            if (currentRule->t == TERMINAL)
            {
                // Self-explanatory

                newTreeNode->data->token = malloc(sizeof(struct token_info));
                newTreeNode->data->token->type = currentRule->u.term;
                newTreeNode->dataForPrint->t = TERMINAL;
                newTreeNode->dataForPrint->u.term = currentRule->u.term;

                if (newTreeNode->data->token->type == NUM)
                {
                    newTreeNode->dataForPrint->valueIfNumber = lookAhead->content.valueOfInt;
                }
                else if (newTreeNode->data->token->type == RNUM)
                {
                    newTreeNode->dataForPrint->valueIfNumber = lookAhead->content.valueOfFloat;
                }
                else
                {
                    newTreeNode->dataForPrint->valueIfNumber = 0;
                }
            }
            else
            {
                newTreeNode->data->token = NULL;
                newTreeNode->dataForPrint->t = NON_TERMINAL;
                newTreeNode->dataForPrint->u.non_term = currentRule->u.non_term;
            }

            // Initialise line number and current node symbol of the tree node
            newTreeNode->dataForPrint->currentNodeSymbol = malloc(sizeof(char) * (VAR_LENGTH + 1));
            newTreeNode->dataForPrint->lineNo = lookAhead->lineNum;

            /*
                If current node symbol is a terminal then we want to initialise the token name as well. Else set it to NULL. We are assigning the
                current node symbol here as well.
            */

            if (newTreeNode->data->alpha->t == TERMINAL)
            {
                newTreeNode->dataForPrint->tokenName = malloc(sizeof(char) * (VAR_LENGTH + 1));
                strcpy(newTreeNode->dataForPrint->tokenName, terminals_string[currentRule->u.term]);
                strcpy(newTreeNode->dataForPrint->currentNodeSymbol, terminals_string[currentRule->u.term]);
            }
            else
            {

                newTreeNode->dataForPrint->tokenName = NULL;
                strcpy(newTreeNode->dataForPrint->currentNodeSymbol, non_terminals_string[currentRule->u.non_term]);
            }

            // Initialise parent node symbol of the tree node and set it to be a leaf node.

            newTreeNode->dataForPrint->parentNodeSymbol = malloc(sizeof(char) * (VAR_LENGTH + 1));
            strcpy(newTreeNode->dataForPrint->parentNodeSymbol, parentTreeNode->dataForPrint->currentNodeSymbol);
            newTreeNode->dataForPrint->isLeafNode = 1;

            newStackNode->correspondingTreeNode = newTreeNode; // Initialise stack element with tree node
            count--;                                           // Decrementing count to signify that we have added a child to the parent node

            if (count == 0) // Counting backwards, so child number 0 is the last child to be added
            {
                parentTreeNode->leftmostChild = newTreeNode;
                newTreeNode->whetherFirstChild = 1;
            }
            else if (count == 1) // Counting backwards, so child number 1 is the second last child to be added
            {
                parentTreeNode->secondLeftmostChild = newTreeNode;
                newTreeNode->whetherFirstChild = 0;
            }
            else
            {
                newTreeNode->whetherFirstChild = 0;
            }

            /*
                Using the temp pointer to keep track of the last child added to the parent node. This is used to set the right sibling pointer of the
                child tree node.
            */

            newTreeNode->rightSibling = temp;
            temp = newTreeNode;

            // We have completed initialising the child stack element, so we can now push it onto the stack.

            push(newStackNode, s);

            // Move to the next rule entry
            currentRule = currentRule->next;
        }

        // We want to now look at the next stack element to perform the next iteration of the while loop.

        stackTop = pop(s);
    }

    /*
        We have now exited the while loop meaning that we have popped a TERMINAL from the stack. We now want to check if the popped terminal matches the
        the look ahead token. If it does not match, we want to print a syntax error message and exit the program. Else we want to pop the stack and
    */

    while (stackTop->t == TERMINAL && stackTop->u.term != lookAhead->type)
    {
        /* Write a syntax error message for mismatch of stack top and look ahead token using printf stating the type expected and the type received citing the line number: */
        syntax_error = 1;
        printf("Line %d: Error in the input as expected token is %s\n", lookAhead->lineNum, terminals_string[stackTop->u.term]);

        stackTop = pop(s);
    }
    if (stackTop->t == TERMINAL && stackTop->u.term == lookAhead->type)
    {
        /*
            Here we want to set the lexeme of the tree node to be the lexeme of the look ahead token since there has been a match.
            It is correct to do this here rather than in the while loop because we want to set the lexeme of the tree node to only that terminal which has been
            successfully matched. If we do it in the while loop, we will set the lexeme of every tree node to the lexeme of the lookahead symbol unitl that
            lookAhead symbol is successfully matched EACH TIME!!
        */

        stackTop->correspondingTreeNode->dataForPrint->lexeme = malloc(sizeof(char) * (LEXEME_SIZE + 1)); 
        
        stackTop->correspondingTreeNode->dataForPrint->lineNo = lookAhead->lineNum;
        if (lookAhead->type == NUM)
        {
            sprintf(stackTop->correspondingTreeNode->dataForPrint->lexeme, "%d", lookAhead->content.valueOfInt);
        }
        else if (lookAhead->type == RNUM)
        {
            sprintf(stackTop->correspondingTreeNode->dataForPrint->lexeme, "%f", lookAhead->content.valueOfFloat);
        }
        else
        {
            strcpy(stackTop->correspondingTreeNode->dataForPrint->lexeme, lookAhead->content.identifier);
        }
        stackTop->correspondingTreeNode->rule_num = -1;
        stackTop->correspondingTreeNode->dataForPrint->rule_num = -1;
        return;
    }
    if (stackTop->t == NON_TERMINAL)
    {
        push(stackTop, s);
        stackTreeFunction(reversedGrammar, s, parseTable, lookAhead);
    }
}

parseTree parseInputSourceCode(char *testcaseFile, int **parseTable, alpha_node *grammar, alpha_node *reversedGrammar)
{
    // Initialise begin and forward pointers

    int begin = 0;
    int forward = 0;

    // Create hash table to pass to lexer
    hash_linked_list *hash_table_lexer = construct_hash_table_lexer();
    hash_table_lexer = populate_hash_table_lexer(hash_table_lexer);

    // Open file, initialise file pointer

    FILE *fp = fopen(testcaseFile, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    // Fill buffers and return file pointer

    fp = getStream(fp);

    // Initialise stack

    Stack *stack = createStack();

    // Create, initialise, and push stack bottom symbol

    struct stackNode *bottomSymbol = malloc(sizeof(struct stackNode));
    bottomSymbol->t = TERMINAL;
    bottomSymbol->u.term = DOLLAR;
    bottomSymbol->correspondingTreeNode = NULL;
    bottomSymbol->next = NULL;
    push(bottomSymbol, stack);

    // Create. initialise, and push first starter 'program' symbol onto stack to kickstart the parsing process

    struct stackNode *programStack = malloc(sizeof(struct stackNode));
    treeNode program_tree = malloc(sizeof(struct treeNode));

    // Initialise program tree node that will be the root of the parse tree and be pointed to by correspondingTreeNode of program stack element

    program_tree->data = malloc(sizeof(struct alpha_and_token));
    program_tree->data->alpha = grammar[0]; // grammar[0] is the first non-terminal and in general the starting symbol of the grammar, which is program

    program_tree->data->token = NULL;
    program_tree->dataForPrint = malloc(sizeof(struct parseTreeNodeData));
    program_tree->dataForPrint->currentNodeSymbol = malloc(sizeof(char) * (VAR_LENGTH + 1)); // Max length of a non-terminal is VAR_LENGTH
    strcpy(program_tree->dataForPrint->currentNodeSymbol, non_terminals_string[0]);
    program_tree->dataForPrint->isLeafNode = 1;
    program_tree->whetherFirstChild = 1;

    /*
        For instance, on the planet Earth, man had always assumed that he was more intelligent than dolphins because he had achieved so much—the wheel,
        New York, wars and so on—whilst all the dolphins had ever done was muck about in the water having a good time. But conversely, the dolphins had
        always believed that they were far more intelligent than man—for precisely the same reasons.
    */

    programStack->t = NON_TERMINAL;
    programStack->u.non_term = program;

    programStack->correspondingTreeNode = program_tree;
    programStack->next = NULL;
    push(programStack, stack);

    // Definfing token and stackTop outside so that we can call it in the while loop

    token_info token;
    struct stackNode *stackTop;

    // Parser keeps retrieving a token to build tree until it reaches the end of the file

    while ((token = getNextToken(fp, hash_table_lexer, &begin, &forward)) != NULL && EOF_Flag == 0)
    {
        bufferSwitch(&begin, &forward, fp, 1);
        // If token returned is an ERROR token, then we skip it and all the next potential error tokens until we get the next valid non error token
        // printToken(token);
        if (token->type == ERROR || token->type == ERROR_ID)
        {
            token = getNextToken(fp, hash_table_lexer, &begin, &forward);
            bufferSwitch(&begin, &forward, fp, 1);
            while (token != NULL && token->type == ERROR && token->type == ERROR_ID)
            {
                token = getNextToken(fp, hash_table_lexer, &begin, &forward);
                bufferSwitch(&begin, &forward, fp, 1);
            }
            if (token == NULL)
            {
                break;
            }
        }

        // Get top of stack to check if while there are tokens still yet to come, whether we are hitting the stack bottom, in which case we have an unrecoverable syntax error!

        stackTop = top(stack);

        if (stackTop->t == TERMINAL && stackTop->u.term == DOLLAR)
        {
            // If stack top is $, then we have reached the end of the start...end block

            printf("Error: Stack is empty, kindly write all program code within start...end delimiter\n\t\t...Skipping the rest of the tokens from line number %d onwards...\n", token->lineNum);
            break;
        }

        /*
            Call stackTreeFunction with to begin our parsing process and also to build our stack tree.
            All (barring otherModules', and possibly other non-terminals' EPSILON case) stack operations
            related to matching rules and building the tree are done in this function
        */

        stackTreeFunction(reversedGrammar, stack, parseTable, token);
    }

    /*
        To ensure that the stack top does not contain non-terminals that can be derived to EPSILON before we give an error for a non-empty stack
        we pop all the non-terminals that can be derived to EPSILON from the stack and add the EPSILON terminals to the tree as the children of these non-terminals
    */

    while (top(stack) != NULL && ((top(stack)->t == TERMINAL && top(stack)->u.term != DOLLAR) || (top(stack)->t == NON_TERMINAL)))
    {
        if (isInSet(epsilon_set, (top(stack))->u.non_term)) // Logic to check if epsilon is in the first set of the non-terminal on top of the stack.
        {                                                   // Standard intialisation of the tree node
            stackTop = pop(stack);
            treeNode firstChild = malloc(sizeof(struct treeNode)); // epsilon child
            firstChild->data = malloc(sizeof(struct alpha_and_token));
            firstChild->data->token = malloc(sizeof(struct token_info));
            firstChild->data->alpha = malloc(sizeof(struct alpha_node));
            firstChild->dataForPrint = malloc(sizeof(struct parseTreeNodeData));
            firstChild->dataForPrint->currentNodeSymbol = malloc(sizeof(char) * (VAR_LENGTH + 1));
            firstChild->dataForPrint->isLeafNode = 1;
            firstChild->whetherFirstChild = 1;
            strcpy(firstChild->dataForPrint->currentNodeSymbol, "EPSILON");
            firstChild->dataForPrint->tokenName = firstChild->dataForPrint->currentNodeSymbol;
            firstChild->dataForPrint->parentNodeSymbol = stackTop->correspondingTreeNode->dataForPrint->currentNodeSymbol;
            firstChild->data->token->type = EPSILON;
            firstChild->data->alpha->t = TERMINAL;
            firstChild->data->alpha->u.term = EPSILON;
            firstChild->rule_num = -1;
            firstChild->dataForPrint->rule_num = -1;

            // Update non-terminal's child node entry

            stackTop->correspondingTreeNode->rule_num = 5;
            stackTop->correspondingTreeNode->dataForPrint->rule_num = 5;
            stackTop->correspondingTreeNode->leftmostChild = firstChild;
        }
        else
        {
            // Emptying the stack and giving an error message since the stack is not empty

            printf("Error: Stack is not empty! Please check for missing code!\nEmptying stack ...\n");
            while (top(stack) != NULL)
            {
                pop(stack);
            }
            break;
        }
    }

    // Creating the parse tree struct that is to be returned by parseInputSourceCode

    parseTree pt = malloc(sizeof(struct parseTree));

    if (top(stack) != NULL && top(stack)->t == TERMINAL && top(stack)->u.term == DOLLAR && syntax_error == 0)
    {
        printf("\nInput source code is syntactically correct............\n\n");
    }
    // Since program is the root of the parse tree, we set the root of the parse tree to be program

    pt->root = program_tree;

    return pt;
}

void createParseTable(Set **firstSetArray, Set **followSetArray, alpha_node *grammar, int **parseTable)
{
    // This is an upper caste function.
    // initialise parse table of appropriate dimensions with -1

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        for (int j = 0; j < NUM_TERMINALS; j++)
        {
            parseTable[i][j] = -1; // -1 denotes no rule
        }
    }

    // Our parse table population logic involves iterating over the entire rule set

    for (int i = 0; i < NUM_RULES; i++)
    {
        // If a particular rule derives EPSILON on its RHS, then we consider the follow set logic, else we consider the first set logic

        if (grammar[i]->next->t == TERMINAL && grammar[i]->next->u.term == EPSILON)
        {
            // As per the follow set logic, we add rule R to parse table for a non terminal NT and terminal T iff that terminal T belongs to the follow set of the non terminal NT and the non terminal NT derives EPSILON through rule R

            for (int j = 0; j < NUM_TERMINALS; j++)
            {
                if (isInSet(followSetArray[grammar[i]->u.non_term], j))
                {
                    parseTable[grammar[i]->u.non_term][j] = i;
                }
            }
        }
        else
        {
            // Per the first set logic, we add rule R to parse table for a non terminal NT and terminal T iff that terminal T belongs to the first set of the RHS of the rule R which is deriving something from our non terminal NT that is on the RHS

            for (int j = 0; j < NUM_TERMINALS; j++)
            {
                if (isInSet(firstSetArray[i], j))
                {
                    parseTable[grammar[i]->u.non_term][j] = i;
                }
            }
        }
    }
}

void printParseTable(int **parseTable, alpha_node *grammar)
{
    printf("\nParse Table:\n");
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        for (int j = 0; j < NUM_TERMINALS; j++)
        {
            if (parseTable[i][j] != -1)
            {
                printf("[%s,%s,(%d)]", non_terminals_string[i], terminals_string[j], parseTable[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

alpha_node insertFirstAlpha(alpha_node n, alpha_node headOfRule)
{
    // Inserts a node at the beginning of the RHS of the rule pointed to by first by pointer manipulation

    alpha_node new = malloc(sizeof(struct alpha_node));
    new->t = n->t;
    new->u = n->u;
    new->next = headOfRule->next;
    headOfRule->next = new;
    return new;
}

void equateAlphaNodes(alpha_node n1, alpha_node n2)
{
    // Equate the attributes of the struct for two entities

    n1->t = n2->t;
    n1->u = n2->u;
    strcpy(n1->var_name, n2->var_name);
    n1->next = n2->next;
}

alpha_node *reverseGrammar(alpha_node *inputGrammar)
{
    // This function will generate a grammar structure for which the RHS of each rule is in reversed order. This is essentially used by the stackTreeFunction method which requires to push the RHS of a grammar rule in reverse order onto the stack

    alpha_node *outputGrammar = malloc(NUM_RULES * sizeof(struct alpha_node));
    for (int i = 0; i < NUM_RULES; i++)
    {
        // printf("i = %d\n",i);
        outputGrammar[i] = malloc(sizeof(struct alpha_node));
        outputGrammar[i]->t = inputGrammar[i]->t;
        outputGrammar[i]->u = inputGrammar[i]->u;
        strcpy(outputGrammar[i]->var_name, inputGrammar[i]->var_name);
        outputGrammar[i]->next = NULL;

        alpha_node current = inputGrammar[i]->next;

        while (current != NULL)
        {
            // printf("Checkpoint 1\n");
            insertFirstAlpha(current, outputGrammar[i]);
            // printf("Checkpoint 2\n");
            current = current->next;
        }
    }
    return outputGrammar;
}

void recursivelyPrintParseTree(treeNode node, FILE *fp)
{
    // inorder traversal of parse tree done recursively
    if (node == NULL) // base case
    {
        return;
    }
    else // inorder traversal is here defined as <0th child, self, 1st child, 2nd child, and so on upto (n-1)th child> for each node
    {
        parse_tree_size += sizeof(node);
        parse_tree_count++;
        recursivelyPrintParseTree(node->leftmostChild, fp);

        char isLeafNode[4];
        char lineNum[5];
        lineNum[4] = '\0';
        isLeafNode[3] = '\0';
        if (node->dataForPrint->isLeafNode == 1)
        {
            strcpy(isLeafNode, "Yes");
            int len = snprintf(NULL, 0, "%d", node->dataForPrint->lineNo);
            // char *str = malloc(sizeof(char)*(len + 1)); //doubtchange
            snprintf(lineNum, len + 1, "%d", node->dataForPrint->lineNo);
        }
        else
        {
            strcpy(isLeafNode, "No");
            strcpy(lineNum, "----");
        }

        char lexeme[100];
        lexeme[99] = '\0';
        if (node->dataForPrint->lexeme == NULL)
            strcpy(lexeme, "----");
        else
        {
            strcpy(lexeme, node->dataForPrint->lexeme);
        }

        char tokenName[20];
        tokenName[19] = '\0';
        if (node->dataForPrint->tokenName != NULL)
            strcpy(tokenName, node->dataForPrint->tokenName);
        else
            strcpy(tokenName, "----");

        char valueIfNumber[20];
        valueIfNumber[19] = '\0';
        if (!(strcmp(node->dataForPrint->currentNodeSymbol, "NUM") && strcmp(node->dataForPrint->currentNodeSymbol, "RNUM")))
        {
            int len = snprintf(NULL, 0, "%f", node->dataForPrint->valueIfNumber);
            // char *result = malloc(len + 1); //doubtchange
            snprintf(valueIfNumber, len + 1, "%f", node->dataForPrint->valueIfNumber);
        }
        else
            strcpy(valueIfNumber, "----");

        // printf("%s %s %s %s %s %s %s %d\n", lexeme, lineNum, tokenName, valueIfNumber, node->dataForPrint->parentNodeSymbol, isLeafNode, node->dataForPrint->currentNodeSymbol, node->rule_num);
        fprintf(fp, "%s %s %s %s %s %s %s %d %d %d\n", lexeme, lineNum, tokenName, valueIfNumber, node->dataForPrint->parentNodeSymbol, isLeafNode, node->dataForPrint->currentNodeSymbol, node->rule_num, node->dataForPrint->rule_num, node->dataForPrint->u.term);

        recursivelyPrintParseTree(node->secondLeftmostChild, fp);
        if (node->whetherFirstChild == 0)
        {
            recursivelyPrintParseTree(node->rightSibling, fp);
        }
    }
}

void printParseTree(parseTree pt, char *outFile)
{
    treeNode current = pt->root;

    FILE *fp = fopen(outFile, "w");
    // fprintf(fp, "In-order traversal of the parse tree is as follows:\n");
    recursivelyPrintParseTree(current, fp); // Calls the recursive function to perform the in-order traversal
    fclose(fp);
}

// int main(void)
// {
// //     struct parseTree* pt = malloc(sizeof(struct parseTree));
// //     // Code for creating the parseTree of integers
// //     struct treeNode* node1 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node2 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node3 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node4 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node5 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node6 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node7 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node8 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node9 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node10 = malloc(sizeof(struct treeNode));
// //     struct treeNode* node11 = malloc(sizeof(struct treeNode));
// //     node1->data = 1;
// //     node1->numOfChildren = 3;
// //     node1->leftmostChild = node2;
// //     node1->secondLeftmostChild = node3;
// //     node1->rightSibling = NULL;
// //     node1->whetherFirstChild = 1;
// //     node2->data = 2;
// //     node2->numOfChildren = 3;
// //     node2->leftmostChild = node5;
// //     node2->secondLeftmostChild = node7;
// //     node2->rightSibling = node3;
// //     node2->whetherFirstChild = 1;
// //     node3->data = 3;
// //     node3->numOfChildren = 1;
// //     node3->leftmostChild = node8;
// //     node3->secondLeftmostChild = NULL;
// //     node3->rightSibling = node4;
// //     node3->whetherFirstChild = 0;
// //     node4->data = 4;
// //     node4->numOfChildren = 2;
// //     node4->leftmostChild = node9;
// //     node4->secondLeftmostChild = node10;
// //     node4->rightSibling = NULL;
// //     node4->whetherFirstChild = 0;
// //     node5->data = 5;
// //     node5->numOfChildren = 1;
// //     node5->leftmostChild = node6;
// //     node5->secondLeftmostChild = NULL;
// //     node5->rightSibling = node7;
// //     node5->whetherFirstChild = 1;
// //     node6->data = 6;
// //     node6->numOfChildren = 0;
// //     node6->leftmostChild = NULL;
// //     node6->secondLeftmostChild = NULL;
// //     node6->rightSibling = NULL;
// //     node6->whetherFirstChild = 1;
// //     node7->data = 7;
// //     node7->numOfChildren = 0;
// //     node7->leftmostChild = NULL;
// //     node7->secondLeftmostChild = NULL;
// //     node7->rightSibling = node11;
// //     node7->whetherFirstChild = 0;
// //     node8->data = 8;
// //     node8->numOfChildren = 0;
// //     node8->leftmostChild = NULL;
// //     node8->secondLeftmostChild = NULL;
// //     node8->rightSibling = NULL;
// //     node8->whetherFirstChild = 1;
// //     node9->data = 9;
// //     node9->numOfChildren = 0;
// //     node9->leftmostChild = NULL;
// //     node9->secondLeftmostChild = NULL;
// //     node9->rightSibling = node10;
// //     node9->whetherFirstChild = 1;
// //     node10->data = 10;
// //     node10->numOfChildren = 0;
// //     node10->leftmostChild = NULL;
// //     node10->secondLeftmostChild = NULL;
// //     node10->rightSibling = NULL;
// //     node10->whetherFirstChild = 0;
// //     node11->data = 11;
// //     node11->numOfChildren = 0;
// //     node11->leftmostChild = NULL;
// //     node11->secondLeftmostChild = NULL;
// //     node11->rightSibling = NULL;
// //     node11->whetherFirstChild = 0;
// //     pt->root = node1;
// //     printParseTree(pt, "parseTree.txt");

// //      buffer_1[BUFFER_SIZE] = '\0';
// //      buffer_2[BUFFER_SIZE] = '\0';
// //      selected_buffer[BUFFER_SIZE] = '\0';
// //      hash_linked_list *hash_table_lexer = construct_hash_table_lexer();
// //      hash_table_lexer = populate_hash_table_lexer(hash_table_lexer);
// //      // print_hash_table(hash_table);
// //      // removeComments("sample.txt", "clean.txt");
// //      FILE *fp_1 = fopen("sample.txt", "r");
// //      if (fp_1 == NULL)
// //      {
// //          printf("Error opening file\n");
// //          exit(1);
// //      }
// //      fillBuffers(fp_1);
// //      printBuffer();
// //      int begin = 0;
// //      int forward = 0;
// //      for (int tempi = 0; tempi < 4; tempi++)
// //      {
// //          token_info token8 = getNextToken(fp_1, hash_table_lexer, &begin, &forward);
// //          if(token8 == NULL)
// //          {
// //            printf("Error: \"%s\" Size exceeds %d | Line: %d\n", big_identifier,LEXEME_SIZE, lineNum);
// //            free(big_identifier);
// //          }
// //          else
// //              printToken(token8);
// //  // //         {
// //  //           if (lexerError != 1)
// //  //             printToken(token8);
// //  //           else
// //  //             lexerError = 0;
// //  // //         }
// //          bufferSwitch(&begin, &forward, fp_1, 1);
// //      }
// //      // a loop to obtain all tokens and print them like above would be convenient
// //      fclose(fp_1);

// //     return 0;

// //     alpha_list *hash_table_parser = construct_hash_table_parser();
// //     hash_table_parser = populate_hash_table_parser(hash_table_parser);
// //     print_alpha_list(hash_table_parser);

// //     FILE *fp_2 = fopen("grammar_modified.txt", "r");

// //     if (fp_2 == NULL)
// //         printf("File is NULL\n");

// //     grammar = malloc(NUM_RULES * sizeof(alpha_node));
// //     grammar = createGrammarBase(fp_2, hash_table_parser);
// //     // printf("\n check 1");
// //     if (grammar == NULL)
// //         printf("Grammar is NULL\n");

// //     printGrammar(grammar, NUM_RULES);

// //     reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
// //     reversedGrammar = reverseGrammar(grammar);

// //     printf("Reverse()");
// //     printGrammar(reversedGrammar, NUM_RULES);


// // - All the tasks done below this point make use of the Set ADT
// // - The Set ADT is defined in the file set.h
// // - Set essentially has two fields:
// //     - array: This is an array of integers. The array is of size NUM_TERMINALS
// //     - size: This is the number of elements in the array


// // set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
// // for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
// // {
// //     set_first[i] = (Set *)malloc(sizeof(Set));
// //     set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
// //     set_first[i]->array = getFirstSet(grammar, i);
// //     set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
// // }

// // displaySet(set_first, NUM_NON_TERMINALS, 0);

// // rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

// // for (int i = 0; i < NUM_RULES; i++)
// // {
// //     rule_wise_first[i] = (Set *)malloc(sizeof(Set));
// //     rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
// //     rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
// //     rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
// // }

// // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

// // set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
// // set_follow[0] = (Set *)malloc(sizeof(Set));
// // set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
// // set_follow[0]->array[DOLLAR] = 1;
// // set_follow[0]->size = 1;

// // for (int i = 1; i < NUM_NON_TERMINALS; i++)
// // {
// //     set_follow[i] = (Set *)malloc(sizeof(Set));
// //     set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
// //     set_follow[i]->array = getFollowSet(grammar, i);
// //     set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
// // }

// // displaySet(set_follow, NUM_NON_TERMINALS, 1);

// // int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));
// // for (int i = 0; i < NUM_NON_TERMINALS; i++)
// // {
// //     parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
// // }
// // createParseTable(rule_wise_first, set_follow, grammar, parseTable);
// // printParseTable(parseTable, grammar);

// // fclose(fp_2);

// // /*Free the memory*/
// // for (int i = 0; i < NUM_RULES; i++)
// // {
// //     free(grammar[i]);
// //     free(rule_wise_first[i]);
// // }
// // free(grammar);

// // for (int i = 0; i < NUM_NON_TERMINALS; i++)
// // {
// //     free(set_first[i]);
// //     free(set_follow[i]);
// // }

// // free(rule_wise_first);grammar = malloc(NUM_RULES * sizeof(alpha_node));
// // grammar = createGrammarBase(fp_2, hash_table_parser);
// // free(set_first);
// // free(set_follow);

//     BUFFER_SIZE = 64;
//     buffer_1 = (char *)malloc((BUFFER_SIZE+1) * sizeof(char));
//     buffer_2 = (char *)malloc((BUFFER_SIZE+1) * sizeof(char));
//     selected_buffer = (char *)malloc((BUFFER_SIZE+1) * sizeof(char));

//     buffer_1[BUFFER_SIZE] = '\0';
//     buffer_2[BUFFER_SIZE] = '\0';
//     selected_buffer[BUFFER_SIZE] = '\0';

//     alpha_list *hash_table_parser = construct_hash_table_parser();
//     hash_table_parser = populate_hash_table_parser(hash_table_parser);
//     // print_alpha_list(hash_table_parser);

//     FILE *fp_2 = fopen("grammar_modified.txt", "r");

//     if (fp_2 == NULL)
//         printf("File is NULL\n");

//     grammar = malloc(NUM_RULES * sizeof(alpha_node));
//     grammar = createGrammarBase(fp_2, hash_table_parser);
//     // printf("\n check 1");
//     if (grammar == NULL)
//         printf("Grammar is NULL\n");

//     // printGrammar(grammar, NUM_RULES);

//     epsilon_set = getEpsilonNonTerminals(grammar,NUM_RULES);

//     reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
//     reversedGrammar = reverseGrammar(grammar);

//     printf("\n[Reversed]");
//     // printGrammar(reversedGrammar, NUM_RULES);

//     set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

//     for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
//     {
//         set_first[i] = (Set *)malloc(sizeof(Set));
//         set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
//         set_first[i]->array = getFirstSet(grammar, i);
//         set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
//     }

//     // displaySet(set_first, NUM_NON_TERMINALS, 0);

//     rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

//     for (int i = 0; i < NUM_RULES; i++)
//     {
//         rule_wise_first[i] = (Set *)malloc(sizeof(Set));
//         rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
//         rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
//         rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
//     }

//     // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

//     set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
//     set_follow[0] = (Set *)malloc(sizeof(Set));
//     set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
//     set_follow[0]->array[DOLLAR] = 1;
//     set_follow[0]->size = 1;

//     for (int i = 1; i < NUM_NON_TERMINALS; i++)
//     {
//         set_follow[i] = (Set *)malloc(sizeof(Set));
//         set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
//         set_follow[i]->array = getFollowSet(grammar, i);
//         set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
//     }

//     // displaySet(set_follow, NUM_NON_TERMINALS, 1);

//     int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

//     for (int i = 0; i < NUM_NON_TERMINALS; i++)
//     {
//         parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
//     }
//     createParseTable(rule_wise_first, set_follow, grammar, parseTable);

//     // printParseTable(parseTable, grammar);

//     fclose(fp_2);

//     parseTree pt = parseInputSourceCode("t6.txt", parseTable, grammar, reversedGrammar);

//     printf("\nbruh02\n");
//     printParseTree(pt, "xhoda4.txt");

//     return 0;
// }
