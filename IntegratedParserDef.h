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

struct parseTreeNodeData
{
    char *lexeme;
    int lineNo;
    char *tokenName;
    float valueIfNumber;
    char *currentNodeSymbol;
    char *parentNodeSymbol;
    int isLeafNode;
};

struct alpha_and_token
{
    alpha_node alpha;
    token_info token;
};
typedef struct alpha_and_token *alpha_and_token;

struct treeNode
{
    alpha_and_token data;
    struct parseTreeNodeData *dataForPrint;
    int numOfChildren;
    struct treeNode *leftmostChild;
    struct treeNode *secondLeftmostChild;
    struct treeNode *rightSibling;
    int whetherFirstChild;
};
typedef struct treeNode *treeNode;

struct parseTree
{
    struct treeNode *root;
};
typedef struct parseTree *parseTree;

int last_rule_expanded = -1;

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


