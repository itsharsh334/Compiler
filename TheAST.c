/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#include "IntegratedParser.c"
#include <limits.h>

/*
struct Node
{
    char data[20];
    int isTerminal;
    int isFirst;
    int num_children;

    struct ASTNode *addr;
    struct ASTNode *synaddr;
    struct ASTNode *inhaddr;
    struct Node* right;
    struct Node* first_child;
    struct Node* second_child;
};
typedef struct Node* Node;
*/
/*
struct Tree
{
    Node root;
};
typedef struct Tree* Tree;
*/

/*
Node createNode(char *data, int isTerminal, int isFirst)
{
    Node node = (Node)malloc(sizeof(struct Node));
    strcpy(node->data, data);
    node->isTerminal = isTerminal;
    node->isFirst = isFirst;

    node->addr = NULL;
    node->synaddr = NULL;
    node->inhaddr = NULL;

    node->num_children = 0;
    node->right = NULL;
    node->first_child = NULL;
    node->second_child = NULL;
    return node;
}
*/


parseTreeNodeData copyField(parseTreeNodeData data)
{
    /*
    - Function to ease the copying the data of parseTreeNodeData from one to another
    */
    parseTreeNodeData temp = (parseTreeNodeData)malloc(sizeof(struct parseTreeNodeData));
    temp->lexeme = (char *)malloc(sizeof(char) * (LEXEME_SIZE + 1));
    if (data->lexeme != NULL)
        strcpy(temp->lexeme, data->lexeme);
    else
        temp->lexeme = NULL;
    temp->lineNo = data->lineNo;
    if (data->tokenName != NULL)
    {
        temp->tokenName = (char *)malloc(sizeof(char) * (LEXEME_SIZE + 1));
        strcpy(temp->tokenName, data->tokenName);
    }
    else
    {
        temp->tokenName = NULL;
    }
    temp->valueIfNumber = data->valueIfNumber;
    temp->currentNodeSymbol = (char *)malloc(sizeof(char) * (VAR_LENGTH));
    strcpy(temp->currentNodeSymbol, data->currentNodeSymbol);
    if (data->parentNodeSymbol != NULL)
    {
        temp->parentNodeSymbol = (char *)malloc(sizeof(char) * VAR_LENGTH);
        strcpy(temp->parentNodeSymbol, data->parentNodeSymbol);
    }
    else
    {
        temp->parentNodeSymbol = NULL;
    }
    return temp;
}

void displayParseTreeNodeData(parseTreeNodeData data)
{
    /*
    - Self explanatory
    */
    if (data == NULL)
    {
        printf("No Data\n");
        return;
    }
    printf("===========================================\n"
           "lexeme = %s |\n"
           "lineNo = %d |\n"
           "tokenName = %s |\n"
           "valueIfNumber = %f |\n"
           "currentNodeSymbol = %s |\n"
           "parentNodeSymbol = %s |\n"
           "===========================================\n\n",
           data->lexeme,
           data->lineNo,
           data->tokenName,
           data->valueIfNumber,
           data->currentNodeSymbol,
           data->parentNodeSymbol);
    return;
}

treeNode createTreeNode(parseTreeNodeData data, int whetherFirstChild, int rule_num)
{
    /*
    - Create and return a treeNode
    */
    treeNode node = (treeNode)malloc(sizeof(struct treeNode));
    node->data = NULL;
    node->dataForPrint = data;
    node->whetherFirstChild = whetherFirstChild;
    node->rule_num = rule_num;
    node->numOfChildren = 0;
    node->addr = NULL;
    node->synaddr = NULL;
    node->inhaddr = NULL;
    node->secondLeftmostChild = NULL;
    node->rightSibling = NULL;
    node->leftmostChild = NULL;
    return node;
}

parseTreeNodeData createParseTreeNodeData(char *lexeme, int lineNo, char *tokenName, int valueIfNumber, char *currentNodeSymbol, char *parentNodeSymbol)
{
    /*
    - Create and return a parseTreeNodeData contained within a treeNode as well as ASTNode
    */
    parseTreeNodeData data = (parseTreeNodeData)malloc(sizeof(struct parseTreeNodeData));
    data->lexeme = (char *)malloc(sizeof(char) * 21);
    strcpy(data->lexeme, lexeme);
    data->lineNo = lineNo;
    data->tokenName = (char *)malloc(sizeof(char) * 21);
    strcpy(data->tokenName, tokenName);
    data->valueIfNumber = valueIfNumber;
    data->currentNodeSymbol = (char *)malloc(sizeof(char) * 21);
    strcpy(data->currentNodeSymbol, currentNodeSymbol);
    data->parentNodeSymbol = (char *)malloc(sizeof(char) * 21);
    strcpy(data->parentNodeSymbol, parentNodeSymbol);
    return data;
}
ASTNode createASTNode(parseTreeNodeData data, int num_children)
{
    /*
    - Create and return an ASTNode
    */
    ASTNode node = (ASTNode)malloc(sizeof(struct ASTNode));
    node->dataForPrint = copyField(data);
    node->dataForPrint->t = data->t;
    if (node->dataForPrint->t == TERMINAL)
        node->dataForPrint->u.term = data->u.term;
    else
        node->dataForPrint->u.non_term = data->u.non_term;
    node->info_list = NULL;
    node->num_children = num_children;
    node->children = (ASTNode *)malloc(sizeof(ASTNode) * num_children);
    for (int i = 0; i < num_children; i++)
    {
        node->children[i] = NULL;
    }
    // printf("Exit 1\n");
    node->range_ast[0]=0;
    node->range_ast[1]=0;
    return node;
}

/*
ASTNode getInfoExpr(treeNode root)
{
    treeNode node = root;
    if(node == NULL)
    {
        return NULL;
    }

    if(node->num_children == 1)
    {
        if(strcmp(node->first_child->data,"EPSILON")==0)
        {
            free(node->first_child);
            node->first_child = NULL;
            return node->inhaddr;
        }
        else
        {
            ASTNode temp = createASTNode(node->first_child->data,2);
            free(node->first_child);
            node->first_child = NULL;
            return temp;
        }
    }
    else if(node->num_children == 2)
    {
        node->addr = getInfoExpr(node->first_child);
        node->second_child->inhaddr = node->addr;
        node->synaddr = getInfoExpr(node->second_child);
        free(node->first_child);
        free(node->second_child);
        node->first_child = NULL;
        node->second_child = NULL;
        return node->synaddr;
    }
    else if(node->num_children == 3)
    {
        ASTNode op = createASTNode(node->first_child->data,2);
        free(node->first_child);
        node->first_child = NULL;
        op->children[0] = node->inhaddr;
        op->children[1] = getInfoExpr(node->second_child);

        node->second_child->right->inhaddr = op;
        node->synaddr = getInfoExpr(node->second_child->right);
        free(node->second_child);
        free(node->second_child->right);

        node->second_child->right = NULL;
        node->second_child = NULL;
        return node->synaddr;
    }

}
*/

/*
void displayAST(ASTNode root)
{
    ASTNode node = root;
    if(node == NULL)
    {
        return;
    }
    else
    {
        printf("data = %s\n",node->data);
        for(int i=0;i<node->num_children;i++)
        {
            displayAST(node->children[i]);
        }
    }
}
*/
void displayAST(ASTNode root)
{
    /*
    - Preorder traversal of AST [Without the AST List data as of now]
    */
    ASTNode node = root;
    if (node == NULL)
    {
        return;
    }
    // printf("1\n");
    ast_size1 += sizeof(root);
    ast_count1++;
    // displayParseTreeNodeData(node->dataForPrint);
    // printf("2\n");
    if (node->info_list != NULL)
    {
        // printf("AST List size:%d\n", node->info_list->size);
        ASTListNode t = node->info_list->head;
        while (t != NULL)
        {
            displayAST(t->data);
            t = t->next;
        }
    }
    // if(node->dataForPrint->lineNo==4&&strcmp(node->dataForPrint->currentNodeSymbol,"MINUS")==0)
    //     node->num_children=2;
    // printf("Number of Children: %d\n", node->num_children);
    for (int i = 0; i < node->num_children; i++)
    {
        // printf("Child %d:\n", i + 1);
        displayAST(node->children[i]);
    }
}
void displayASTprint(ASTNode root)
{
    /*
    - Preorder traversal of AST [Without the AST List data as of now]
    */
    ASTNode node = root;
    if (node == NULL)
    {
        return;
    }
    // printf("1\n");
    displayParseTreeNodeData(node->dataForPrint);
    // printf("2\n");
    if (node->info_list != NULL)
    {
        // printf("AST List size:%d\n", node->info_list->size);
        ASTListNode t = node->info_list->head;
        while (t != NULL)
        {
            displayASTprint(t->data);
            t = t->next;
        }
    }
    // if(node->dataForPrint->lineNo==4&&strcmp(node->dataForPrint->currentNodeSymbol,"MINUS")==0)
    //     node->num_children=2;
    // printf("Number of Children: %d\n", node->num_children);
    for (int i = 0; i < node->num_children; i++)
    {
        // printf("Child %d:\n", i + 1);
        displayASTprint(node->children[i]);
    }
}

ASTNode getASTInformation(treeNode root, int rule_num)
{
    /*
    - Get the AST information from the parse tree
    - Selection of semantic rules is done based on the rule number
    - Uses Switch Case
    */
    // printf("Rule Number: %d\n", rule_num);
    switch (rule_num)
    {
    case 0:
    {
        /*
            Grammar: [program : moduleDeclarations otherModules driverModule otherModules]

            Semantics:
            [
                program.addr = CREATE_NODE("PROGRAM", moduleDeclarations.head, otherModules.head, driverModule.addr, otherModules-1.head)
                FREE(moduleDeclarations)
                FREE(otherModules)
                FREE(driverModule)
                FREE(otherModules-1)
            ]
        */

        ASTNode program = createASTNode(root->dataForPrint, 4);
        program->children[0] = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        program->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        program->children[2] = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);
        program->children[3] = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        // printf("Children = %d\n",program->children[3]->info_list->tail->data->num_children);

        return program;
        break;
    }

    case 1:
    {
        /*
            Grammar: [moduleDeclarations : moduleDeclaration moduleDeclarations-1]

            Semantics:
            [
                INSERT_AT_END(moduleDeclarations.head, moduleDeclaration.addr)
                moduleDeclarations-1.head = moduleDeclarations.head
                FREE(moduleDeclaration)
                FREE(moduleDeclarations-1)
            ]
        */

        if (root->addr == NULL)
        {
            root->addr = createASTNode(root->dataForPrint, 0);
            root->addr->info_list = malloc(sizeof(struct ASTList));
            root->addr->info_list->size = 0;
            root->addr->info_list->head = NULL;
            root->addr->info_list->tail = NULL;
        }
        insertAtEnd(root->addr->info_list, getASTInformation(root->leftmostChild, root->leftmostChild->rule_num));
        root->secondLeftmostChild->addr = root->addr;
        getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 2:
    {
        /*
            Grammar: [moduleDeclarations : EPSILON]

            Semantics:
            [
                FREE(EPSILON)
            ]
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return NULL;
        break;
    }

    case 3:
    {
        /*
            Grammar: [moduleDeclaration : DECLARE MODULE ID SEMICOL]

            Semantics:
            [
                moduleDeclaration.addr = &ID
                FREE(DECLARE)
                FREE(MODULE)
                FREE(SEMICOL)
            ]
        */

        ASTNode id = createASTNode(root->secondLeftmostChild->rightSibling->dataForPrint, 0);

        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return id;
        break;
    }

    case 4:
    {
        /*
            Grammar: [otherModules : module otherModules]

            Semantics:
            [
                INSERT_AT_END(otherModules.head, module.addr)
                otherModules-1.head = otherModules.head
                FREE(module)
                FREE(otherModules-1)
            ]
        */

        if (root->addr == NULL)
        {
            root->addr = createASTNode(root->dataForPrint, 0);
            root->addr->info_list = malloc(sizeof(struct ASTList));
            root->addr->info_list->size = 0;

            root->addr->info_list->head = NULL;
            root->addr->info_list->tail = NULL;
        }
        insertAtEnd(root->addr->info_list, getASTInformation(root->leftmostChild, root->leftmostChild->rule_num));
        // displayParseTreeNodeData(root->addr->info_list->tail->data->dataForPrint);
        root->secondLeftmostChild->addr = root->addr;
        getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 5:
    {
        /*
            Grammar: [otherModules : EPSILON]

            Semantics:
            [
                FREE(EPSILON)
            ]
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return NULL;
        break;
    }

    case 6:
    {
        /*
            Grammar: [driverModule : DRIVERDEF DRIVER PROGRAM DRIVERENDDEF moduleDef]

            Semantics:
            [
                driverModule.addr = moduleDef.addr
                FREE(DRIVERDEF)
                FREE(MODULE)
                FREE(PROGRAM)
                FREE(DRIVERENDDEF)
            ]
        */

        root->addr = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 7:
    {
        /*
            Grammar: [module : DEF MODULE ID ENDDEF TAKES INPUT SQBO input_plist SQBC SEMICOL ret moduleDef]

            Semantics:
            [
                module.addr = CREATE_NODE("MODULE", &ID, input_plist.head, ret.addr, moduleDef.addr)
                FREE(DEF)
                FREE(MODULE)
                FREE(SEMICOL)
                FREE(ENDDEF)
                FREE(TAKES)
                FREE(INPUT)
                FREE(SQBO)
                FREE(SQBC)
                FREE(input_plist)
                FREE(ret)
                FREE(moduleDef)
            ]
        */

        ASTNode id = createASTNode(root->secondLeftmostChild->rightSibling->dataForPrint, 0);
        ASTNode inplistNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);
        ASTNode retNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);
        ASTNode moddefNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);

        ASTNode moduleNode = createASTNode(root->dataForPrint, 4);
        moduleNode->children[0] = id;
        moduleNode->children[1] = inplistNode;
        moduleNode->children[2] = retNode;
        moduleNode->children[3] = moddefNode;
        if(moddefNode!=NULL){
            moduleNode->range_ast[0]=moddefNode->range_ast[0];
            moduleNode->range_ast[1]=moddefNode->range_ast[1];
        }
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return moduleNode;
        break;
    }

    case 8:
    {
        /*
            Grammar: [ret : RETURNS SQBO output_plist SQBC SEMICOL]

            Semantics:
            [
                ret.addr = output_plist.head
                FREE(RETURNS)
                FREE(SQBO)
                FREE(SQBC)
                FREE(SEMICOL)
                FREE(output_plist)
            ]
        */

        ASTNode outplistNode = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return outplistNode;
        break;
    }

    case 9:
    {
        /*
            Grammar: [ret : EPSILON]

            Semantics:
            [
                ret.addr = NULL
                FREE(EPSILON)
            ]
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return NULL;
        break;
    }

    case 10:
    {
        /*
            Grammar: [input_plist : ID COLON dataType n1]

            Semantics:
            [
                INSERT_AT_END(input_plist.head, CREATE_NODE("I_PARAM", &ID, dataType.addr))
                n1.head = input_plist.head
                FREE(COLON)
                FREE(dataType)
                FREE(n1)
            ]
        */
        ASTNode inplistNode = createASTNode(root->dataForPrint, 0);
        inplistNode->info_list = malloc(sizeof(struct ASTList));
        inplistNode->info_list->size = 0;

        inplistNode->info_list->head = NULL;
        inplistNode->info_list->tail = NULL;
        parseTreeNodeData t = createParseTreeNodeData("", root->leftmostChild->dataForPrint->lineNo, "ID:TYPE", 0, root->dataForPrint->currentNodeSymbol, root->dataForPrint->currentNodeSymbol);

        ASTNode parameter = createASTNode(t, 2);
        parameter->children[0] = createASTNode(root->leftmostChild->dataForPrint, 0); // getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        parameter->children[1] = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);
        // printf("Here 1\n");
        insertAtEnd(inplistNode->info_list, parameter);
        // printf("Here 2\n");

        root->secondLeftmostChild->rightSibling->rightSibling->addr = inplistNode;
        getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return inplistNode;
        break;
    }

    case 11:
    {
        /*
            Grammar: [n1 : COMMA ID COLON dataType n1]

            Semantics:
            [
                INSERT_AT_END(n1.head, CREATE_NODE("I_PARAM", &ID, dataType.addr))
                n1-1.head = n1.head
                FREE(COLON)
                FREE(dataType)
                FREE(n1-1)
            ]
        */
        parseTreeNodeData t = createParseTreeNodeData("", root->leftmostChild->dataForPrint->lineNo, "ID:TYPE", 0, root->dataForPrint->currentNodeSymbol, root->dataForPrint->currentNodeSymbol);

        ASTNode parameter = createASTNode(t, 2);
        // ASTNode parameter = createASTNode(root->secondLeftmostChild->dataForPrint, 2);
        parameter->children[0] = createASTNode(root->secondLeftmostChild->dataForPrint, 0);
        parameter->children[1] = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);
        insertAtEnd(root->addr->info_list, parameter);

        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->addr = root->addr;
        getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 12:
    {
        /*
            Grammar: [n1 : EPSILON]

            Semantics:
            [
                FREE(EPSILON)
            ]
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return NULL;
        break;
    }

    case 13:
    {
        /*
            Grammar: [output_plist : ID COLON type_ n2]

            Semantics:
            [
                INSERT_AT_END(output_plist.head, CREATE_NODE("O_PARAM", &ID, type_.addr))
                n2.head = output_plist.head
                FREE(COLON)
                FREE(type_)
                FREE(n2)
            ]
        */

        ASTNode outlistNode = createASTNode(root->dataForPrint, 0);
        outlistNode->info_list = malloc(sizeof(struct ASTList));
        outlistNode->info_list->size = 0;
        outlistNode->info_list->head = NULL;
        outlistNode->info_list->tail = NULL;
        parseTreeNodeData t = createParseTreeNodeData("", root->leftmostChild->dataForPrint->lineNo, "ID:TYPE", 0, root->dataForPrint->currentNodeSymbol, root->dataForPrint->currentNodeSymbol);

        ASTNode parameter = createASTNode(t, 2);
        parameter->children[0] = createASTNode(root->leftmostChild->dataForPrint, 0);
        parameter->children[1] = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);
        insertAtEnd(outlistNode->info_list, parameter);

        root->secondLeftmostChild->rightSibling->rightSibling->addr = outlistNode;
        getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return outlistNode;
        break;
    }

    case 14:
    {
        /*
            Grammar: [n2 : COMMA ID COLON type_ n2]

            Semantics:
            [
                INSERT_AT_END(n2.head, CREATE_NODE("O_PARAM", &ID, type_.addr))
                n2-1.head = n2.head
                FREE(COLON)
                FREE(COMMA)
                FREE(type_)
                FREE(n2-1)
            ]

            Note: O Param is no more!!!
        */
        parseTreeNodeData t = createParseTreeNodeData("", root->secondLeftmostChild->dataForPrint->lineNo, "ID:TYPE", 0, root->dataForPrint->currentNodeSymbol, root->dataForPrint->currentNodeSymbol);
        ASTNode parameter = createASTNode(t, 2);
        parameter->children[0] = createASTNode(root->secondLeftmostChild->dataForPrint, 0);
        parameter->children[1] = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);
        insertAtEnd(root->addr->info_list, parameter);

        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->addr = root->addr;
        getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 15:
    {
        /*
            Grammar: [n2 : EPSILON]

            Semantics:
            [
                FREE(EPSILON)
            ]
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return NULL;
        break;
    }

    case 16:
    {
        /*
            Grammar: [dataType : ARRAY SQBO range_arrays SQBC OF type_]

            Semantics:
            [
                dataType.addr = CREATE_NODE("ARRAY", range_arrays.addr, type_.addr)
                FREE(ARRAY)
                FREE(SQBO)
                FREE(SQBC)
                FREE(OF)
                FREE(range_arrays)
                FREE(type_)
            ]
        */
        parseTreeNodeData t = createParseTreeNodeData("", root->leftmostChild->dataForPrint->lineNo, "ARRAY_TYPE", 0, root->dataForPrint->currentNodeSymbol, root->dataForPrint->currentNodeSymbol);

        ASTNode arrayNode = createASTNode(t, 2);
        // ASTNode arrayNode = createASTNode(root->leftmostChild->dataForPrint, 2);
        arrayNode->children[0] = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);
        arrayNode->children[1] = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return arrayNode;
        break;
    }

    case 17:
    {
        /*
            Grammar: [dataType : INTEGER]

            Semantics:
            [
                dataType.addr = &INTEGER
            ]
        */

        ASTNode integerNode = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return integerNode;
        break;
    }

    case 18:
    {
        /*
            Grammar: [dataType : REAL]

            Semantics:
            [
                dataType.addr = &REAL
            ]
        */

        ASTNode realNode = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return realNode;
        break;
    }

    case 19:
    {
        /*
            Grammar: [dataType : BOOLEAN]

            Semantics:
            [
                dataType.addr = &BOOLEAN
            ]
        */

        ASTNode booleanNode = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return booleanNode;
        break;
    }

    case 20:
    {
        /*
            Grammar: [range_arrays : index_arr RANGEOP index_arr]

            Semantics:
            [
                range_arrays.addr = CREATE_NODE("RANGE", index_arr.addr, index_arr-1.addr)
                FREE(RANGEOP)
                FREE(index_arr)
                FREE(index_arr-1)
            ]
        */

        ASTNode rangeNode = createASTNode(root->dataForPrint, 2);
        rangeNode->children[0] = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        rangeNode->children[1] = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return rangeNode;
        break;
    }

    case 21:
    {
        /*
            Grammar: [index_arr : sign new_index]

            Semantics:
            [
                if(sign.addr != NULL) {
                    ADD_RIGHT_CHILD_TO_NODE(sign.addr, new_index.addr)
                    index_arr.addr = sign.addr
                }
                else {
                    index_arr.addr = new_index.addr
                    FREE(sign)
                }
                FREE(new_index)
            ]
        */

        ASTNode signNode = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        ASTNode newindexNode = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        ASTNode indexNode = NULL;

        if (signNode != NULL)
        {
            signNode->children[0] = NULL;
            signNode->children[1] = newindexNode;
            indexNode = signNode;
        }
        else
        {
            indexNode = newindexNode;
        }

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return indexNode;
        break;
    }

    case 22:
    {
        /*
            Grammar: [sign : PLUS]

            Semantics:
            [
                sign.addr = CREATE_NODE("+", NULL, NULL)
                FREE(PLUS)
            ]
        */

        ASTNode plusNode = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return plusNode;
        break;
    }

    case 23:
    {
        /*
            Grammar: [sign : MINUS]

            Semantics:
            [
                sign.addr = CREATE_NODE("-", NULL, NULL)
                FREE(MINUS)
            ]
        */

        ASTNode minusNode = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return minusNode;
        break;
    }

    case 24:
    {
        /*
            Grammar: [sign : EPSILON]

            Semantics:
            [
                sign.addr = NULL
                FREE(EPSILON)
            ]
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return NULL;
        break;
    }

    case 25:
    {
        /*
            Grammar: [new_index : NUM]

            Semantics:
            [
                new_index.addr = &NUM
            ]
        */

        ASTNode newindex = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return newindex;
        break;
    }

    case 26:
    {
        /*
            Grammar: [new_index : ID]

            Semantics:
            [
                new_index.addr = &ID
            ]
        */

        ASTNode newindex = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return newindex;
        break;
    }

    case 27:
    {
        /*
            Grammar: [type_ : INTEGER]
            Semantic Rule(s): [type_.addr = &INTEGER]
        */

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 28:
    {
        /*
            Grammar: [type_ : REAL]
            Semantic Rule(s): [type_.addr = &REAL]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 29:
    {
        /*
        Grammar: [type_ : BOOLEAN]
        Semantic Rule(s): [type_.addr = &BOOLEAN]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 30:
    {
        /*
            Grammar: [moduleDef : START statements END]
            Semantic Rule(s): [
                moduleDef.addr = statements.head
                FREE(START)
                FREE(statements)
                FREE(END)
            ]
        */
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        if(root->synaddr!=NULL){
            root->synaddr->range_ast[0]=root->leftmostChild->dataForPrint->lineNo;
            root->synaddr->range_ast[1]=root->secondLeftmostChild->rightSibling->dataForPrint->lineNo;
        }
        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 31:
    {
        /*
            Grammar: [statements : statement statements]
            Semantic Rule(s): [
                INSERT_AT_END(statements.head, statement.addr)
                FREE(statement)
                FREE(statements)
            ]
        */
        ASTNode t = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        if (root->inhaddr == NULL)
        {
            root->inhaddr = createASTNode(root->dataForPrint, 0);
            root->inhaddr->info_list = malloc(sizeof(struct ASTList));
            root->inhaddr->info_list->size = 0;
            root->inhaddr->info_list->head = NULL;
            root->inhaddr->info_list->tail = NULL;
        }
        insertAtEnd(root->inhaddr->info_list, t);
        root->secondLeftmostChild->inhaddr = root->inhaddr;
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 32:
    {
        /*
            Grammar: [statements : EPSILON]
            Semantic Rule(s): [FREE(EPSILON)]
        */
        root->synaddr = root->inhaddr;
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 33:
    {
        /*
            Grammar: [statement : ioStmt]
            Semantic Rule(s): [
                statement.addr = ioStmt.addr
                FREE(ioStmt)
            ]
        */
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 34:
    {
        /*
            Grammar: [statement : simpleStmt]
            Semantic Rule(s): [
                statement.addr = simpleStmt.addr
                FREE(simpleStmt)
            ]
        */
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 35:
    {
        /*
            Grammar: [statement : declareStmt]
            Semantic Rule(s): [
                statement.addr = declareStmt.addr
                FREE(declareStmt)
            ]
        */
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 36:
    {
        /*
            Grammar: [statement : condionalStmt]
            Semantic Rule(s): [
                statement.addr = conditionalStmt.addr
                FREE(conditionalStmt)
            ]
        */
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 37:
    {
        /*
            Grammar: [statement : iterativeStmt]
            Semantic Rule(s): [
                statement.addr = iterativeStmt.addr
                FREE(iterativeStmt)
            ]
        */
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 38:
    {
        /*
            Grammar: [ioStmt : GET_VALUE BO ID BC SEMICOL]
            Semantic Rule(s): [
                ioStmt.addr = CREATE_NODE("GET_VALUE", &ID)
                FREE(GET_VALUE)
                FREE(BO)
                FREE(BC)
                FREE(SEMICOL)
            ]
        */
        root->synaddr = createASTNode(root->leftmostChild->dataForPrint, 1);
        root->synaddr->children[0] = createASTNode(root->secondLeftmostChild->rightSibling->dataForPrint, 0);
        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 39:
    {
        /*
            Grammar: [ioStmt : PRINT BO var_print BC SEMICOL]
            Semantic Rule(s): [
                ioStmt.addr = CREATE_NODE("PRINT", var_print.addr)
                FREE(PRINT)
                FREE(BO)
                FREE(BC)
                FREE(SEMICOL)
            ]
        */
        root->synaddr = createASTNode(root->leftmostChild->dataForPrint, 1);
        root->synaddr->children[0] = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);
        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 40:
    {
        /*
            Grammar: [boolConstt : TRUE]
            Semantic Rule(s): [boolConstt.addr = &TRUE]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 41:
    {
        /*
            Grammar: [boolConstt : FALSE]
            Semantic Rule(s): [boolConstt.addr = &FALSE]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 42:
    {
        /*
            Grammar: [var_print : ID p1]
            Semantic Rule(s): [
                var_print.addr = &ID
                var_print.indexIfArray = p1.index
                FREE(p1)
            ]
        */
        root->synaddr = createASTNode(root->dataForPrint, 2);
        root->synaddr->children[0] = createASTNode(root->leftmostChild->dataForPrint, 0);
        root->synaddr->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        if (root->synaddr->children[1] != NULL)
        {
            root->synaddr->dataForPrint->tokenName = malloc(sizeof(char) * (strlen("ARRAY_ELEMENT") + 1));
            strcpy(root->synaddr->dataForPrint->tokenName, "ARRAY_ELEMENT");
            return root->synaddr;
        }
        else
            return root->synaddr->children[0];
        break;
    }

    case 43:
    {
        /*
            Grammar: [var_print : NUM]
            Semantic Rule(s): [var_print.addr = &NUM]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 44:
    {
        /*
            Grammar: [var_print : RNUM]
            Semantic Rule(s): [var_print.addr = &RNUM]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 45:
    {
        /*
            Grammar: [var_print : boolConstt]
            Semantic Rule(s): [
                var_print.addr = boolConstt.addr
                FREE(boolConstt)
            ]
        */
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 46:
    {
        /*
            Grammar: [p1 : SQBO index_arr SQBC]
            Semantic Rule(s): [
                p1.index = index_arr.addr
                FREE(SQBO)
                FREE(SQBC)
                FREE(index_arr)
            ]
        */

        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 47:
    {
        /*
            Grammar: [p1 : EPSILON]
            Semantic Rule(s): [
                p1.index = NULL
                FREE(EPSILON)
            ]
        */
        root->synaddr = root->inhaddr;
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return NULL;
        break;
    }

    case 48:
    {
        /*
            Grammar: [simpleStmt : assignmentStmt]
            Semantic Rule(s): [
                simpleStmt.addr = assignmentStmt.addr
                FREE(assignmentStmt)
            ]
        */
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 49:
    {
        /*
            Grammar: [simpleStmt : moduleReuseStmt]
            Semantic Rule(s): [
                simpleStmt.addr = moduleReuseStmt.addr
                FREE(moduleReuseStmt)
            ]
        */
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 50:
    {
        /*
           Grammar: [assignmentStmt : ID whichStmt]
           Semantic Rule(s): [
               assignmentStmt.addr = CREATE_NODE("ASSIGN", &ID, NULL, NULL)
               whichStmt.inh_addr = assignmentStmt.addr
               FREE(whichStmt)
           ]
       */
        root->secondLeftmostChild->inhaddr = createASTNode(root->leftmostChild->dataForPrint, 0);
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 51:
    {
        /*
         Grammar: [whichStmt : lvalueIDStmt]
         Semantic Rule(s): [
             lvalueIDStmt.inh_addr = whichStmt.inh_addr
             FREE(lvalueIDStmt)
         ]
     */
        root->leftmostChild->inhaddr = root->inhaddr;
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 52:
    {
        /*
            Grammar: [whichStmt : lvalueARRStmt]
            Semantic Rule(s): [
                lvalueARRStmt.inh_addr = whichStmt.inh_addr
                FREE(lvalueARRStmt)
            ]
        */
        root->leftmostChild->inhaddr = root->inhaddr;
        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 53:
    {
        /*
            Grammar: [lvalueIDStmt : ASSIGNOP expression SEMICOL]
            Semantic Rule(s): [
                ADD_THIRD_CHILD_TO_NODE(lvalueIDStmt.inh_addr, expression.syn_addr)
                FREE(expression)
                FREE(ASSIGNOP)
                FREE(SEMICOL)
            ]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        t->children[0] = root->inhaddr;
        t->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        // printf("\n\n\n%f\n\n\n",t->children[1]->children[1]->dataForPrint->valueIfNumber);
        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        root->leftmostChild = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        return t;
        break;
    }

    case 54:
    {
        /*
            Grammar: [lvalueARRStmt : SQBO element_index_with_expressions SQBC ASSIGNOP expression SEMICOL]
            Semantic Rule(s): [
                ADD_SECOND_CHILD_TO_NODE(lvalueARRStmt.inh_addr, element_index_with_expressions.addr)
                ADD_THIRD_CHILD_TO_NODE(lvalueARRStmt.inh_addr, expression.syn_addr)
                FREE(ASSIGNOP)
                FREE(SEMICOL)
                FREE(SQBO)
                FREE(SQBC)
                FREE(element_index_with_expressions)
                FREE(expression)
            ]
        */
        ASTNode t = createASTNode(root->secondLeftmostChild->rightSibling->rightSibling->dataForPrint, 2);
        root->secondLeftmostChild->inhaddr = root->inhaddr;
        t->children[0] = createASTNode(root->dataForPrint, 2);
        t->children[0]->children[0] = createASTNode(root->inhaddr->dataForPrint, 0);
        ASTNode temp = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        if (temp != NULL)
        {
            t->children[0]->children[1] = temp;
            t->children[0]->dataForPrint->tokenName = malloc(sizeof(char) * (strlen("ARRAY_ELEMENT") + 1));
            strcpy(t->children[0]->dataForPrint->tokenName, "ARRAY_ELEMENT");
        }
        t->children[1] = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling);
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        return t;
        break;
    }

    case 55:
    {
        /*
            Grammar Rule:
            moduleReuseStmt : optional USE MODULE ID WITH PARAMETERS actual_para_list SEMICOL

            Semantic Rule:
            moduleReuseStmt.addr = CREATE_NODE("MODULE_USE", &ID, actual_para_list.head, optional.head)
            FREE(optional)
            FREE(actual_para_list)
        */

        ASTNode module_use = createASTNode(root->dataForPrint, 3);

        module_use->children[0] = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        module_use->children[1] = createASTNode(root->secondLeftmostChild->rightSibling->rightSibling->dataForPrint, 0); // getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);
        module_use->children[2] = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;

        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;

        return module_use;
        break;
    }

    case 56:
    {
        /*
            Grammar Rule:
            optional : SQBO idList SQBC ASSIGNOP

            Semantic Rule:
            optional.head = idList.head
            FREE(SQBO)
            FREE(SQBC)
            FREE(ASSIGNOP)
            FREE(idList)
        */

        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;

        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;

        return root->synaddr;
        break;
    }

    case 57:
    {
        /*
            Grammar Rule:
            optional : EPSILON

            Semantic Rule:
            optional.head = NULL
            free(EPSILON)
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return NULL;
        break;
    }

    case 58:
    {
        /*
            Grammar Rule:
            actual_para_list : sign par_print n_12

            Semantic Rule:
            if(sign.addr != NULL){
                ADD_RIGHT_CHILD_TO_NODE(sign.addr, par_print.addr)
                INSERT_AT_END(actual_para_list.head, sign.addr)
            }
            else {
                INSERT_AT_END(actual_para_list.head, par_print.addr)
                FREE(sign)
            }
            n_12.head = actual_para_list.head
            FREE(n_12)
            FREE(par_print)
        */

        ASTNode sign = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        ASTNode par_print = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        if (root->addr == NULL)
        {
            root->addr = createASTNode(root->dataForPrint, 0);
            root->addr->info_list = malloc(sizeof(struct ASTList));
            root->addr->info_list->size = 0;
            root->addr->info_list->head = NULL;
            root->addr->info_list->tail = NULL;
        }
        if (sign != NULL)
        {
            sign->children[0] = NULL;
            sign->children[1] = par_print;
            insertAtEnd(root->addr->info_list, sign);
        }
        else
        {
            insertAtEnd(root->addr->info_list, par_print);
            free(root->leftmostChild);
            root->leftmostChild = NULL;
        }

        root->secondLeftmostChild->rightSibling->addr = root->addr;
        getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;

        return root->addr;
        break;
    }

    case 59:
    {
        /*
            Grammar Rule:
            n_12 : COMMA sign par_print n_12

            Semantic Rule:
            if(sign.addr != NULL){
                ADD_RIGHT_CHILD_TO_NODE(sign.addr, par_print.addr)
                INSERT_AT_END(n_12.head, sign.addr)
            }
            else {
                INSERT_AT_END(n_12.head, par_print.addr)
                FREE(sign)
            }
            n_12-1.head = n_12.head
            FREE(n_12-1)
            FREE(COMMA)
            FREE(par_print)
        */

        ASTNode sign = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        ASTNode par_print = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        if (sign != NULL)
        {
            sign->children[0] = NULL;
            sign->children[1] = par_print;
            insertAtEnd(root->addr->info_list, sign);
        }
        else
        {
            insertAtEnd(root->addr->info_list, par_print);
        }

        root->secondLeftmostChild->rightSibling->rightSibling->addr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 60:
    {
        /*
            Grammar Rule:
            n_12 : EPSILON

            Semantic Rule:
            FREE(EPSILON)
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return NULL;
        break;
    }

    case 61:
    {
        /*
            Grammar Rule:
            par_print : ID n_11

            Semantic Rule:
            par_print.addr = &ID
            par_print.indexIfArray = n_11.index
            FREE(n_11)
        */

        root->synaddr = createASTNode(root->dataForPrint, 2);

        root->synaddr->children[0] = createASTNode(root->leftmostChild->dataForPrint, 0);
        ASTNode temp = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        if (temp != NULL)
        {
            root->synaddr->children[1] = temp;
            root->synaddr->dataForPrint->tokenName = malloc(sizeof(char) * (strlen("ARRAY_ELEMENT") + 1));
            strcpy(root->synaddr->dataForPrint->tokenName, "ARRAY_ELEMENT");
            return root->synaddr;
        }
        return root->synaddr->children[0];
        break;
    }

    case 62:
    {
        /*
            Grammar Rule:
            par_print : NUM

            Semantic Rule:
            par_print.addr = &NUM
        */

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 63:
    {
        /*
            Grammar Rule:
            par_print : RNUM

            Semantic Rule:
            par_print.addr = &RNUM
        */

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 64:
    {
        /*
            Grammar Rule:
            par_print : boolConstt

            Semantic Rule:
            par_print.addr = boolConstt.addr
            FREE(boolConstt)
        */

        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 65:
    {
        /*
            Grammar Rule:
            idList : ID n3

            Semantic Rule:
            INSERT_AT_END(idList.head, &ID)
            n3.head = idList.head
            FREE(n3)
        */
        root->addr = createASTNode(root->dataForPrint, 0);
        root->addr->info_list = malloc(sizeof(struct ASTList));
        root->addr->info_list->size = 0;
        root->addr->info_list->head = NULL;
        root->addr->info_list->tail = NULL;
        insertAtEnd(root->addr->info_list, createASTNode(root->leftmostChild->dataForPrint, 0));
        root->secondLeftmostChild->inhaddr = root->addr;

        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;

        return root->addr; // DOUBTCHANGE
        break;
    }

    case 66:
    {
        /*
            Grammar Rule:
            n3 : COMMA ID n3

            Semantic Rule:
            INSERT_AT_END(n3.head, &ID)
            n3-1.head = n3.head
            FREE(COMMA)
            FREE(n3-1)
        */

        insertAtEnd(root->inhaddr->info_list, createASTNode(root->secondLeftmostChild->dataForPrint, 0));
        root->secondLeftmostChild->rightSibling->inhaddr = root->inhaddr;

        root->synaddr = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->synaddr;
    }

    case 67:
    {
        /*
            Grammar Rule:
            n3 : EPSILON

            Semantic Rule:
            FREE(EPSILON)
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return NULL;
        break;
    }

    case 68:
    {
        /*
            Grammar Rule:
            expression : arithmeticOrBooleanExpr

            Semantic Rule:
            expression.syn_addr = arithmeticOrBooleanExpr.syn_addr
            FREE(arithmeticOrBooleanExpr)
        */

        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 69:
    {
        /*
            Grammar Rule:
           expression : u

            Semantic Rule:
            expression.syn_addr = u.addr
            FREE(u)
        */

        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 70:
    {
        /*
            Grammar Rule:
            u : unary_op new_NT

            Semantic Rule:
            u.addr = unary_op.addr
            ADD_RIGHT_CHILD_TO_NODE(unary_op.addr, NULL, new_NT.syn_addr)
            FREE(unary_op)
            FREE(new_NT)
        */

        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        ASTNode new2 = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        // if(root->addr!=NULL){
        root->addr->children[0] = NULL;
        root->addr->children[1] = new2;
        // root->synaddr = root->addr;
        // }
        // printf("\n\n\n%f\n%f\n\n",root->addr->children[1]->dataForPrint->valueIfNumber,new2->dataForPrint->valueIfNumber);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;

        // printf("\n\n\n%f\n%f\n\n",root->addr->children[1]->dataForPrint->valueIfNumber,new2->dataForPrint->valueIfNumber);

        return root->addr;
        break;
    }

    case 71:
    {
        /*
            Grammar Rule:
            new_NT : BO arithmeticExpr BC

            Semantic Rule:
            new_NT.syn_addr = arithmeticExpr.syn_addr
            FREE(BO)
            FREE(BC)
            FREE(arithmeticExpr)
        */

        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 72:
    {
        /*
            Grammar Rule:
            new_NT : var_id_num

            Semantic Rule:
            new_NT.syn_addr = var_id_num.addr
            FREE(var_id_num)
        */

        root->synaddr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->synaddr;
        break;
    }

    case 73:
    {
        /*
            Grammar Rule:
            var_id_num : ID

            Semantic Rule:
            var_id_num.addr = &ID
        */

        root->addr = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->addr;
        break;
    }

    case 74:
    {
        /*
            Grammar Rule:
            var_id_num : NUM

            Semantic Rule:
            var_id_num.addr = &NUM
        */

        root->addr = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->addr;
        break;
    }

    case 75:
    {
        /*
            Grammar Rule:
            var_id_num : RNUM

            Semantic Rule:
            var_id_num.addr = &RNUM
        */

        root->addr = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->addr;
        break;
    }

    case 76:
    {
        /*
            Grammar Rule:
            unary_op : PLUS

            Semantic Rule:
            unary_op.addr = &PLUS
        */

        root->addr = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->addr;
        break;
    }

    case 77:
    {
        /*
            Grammar Rule:
            unary_op : MINUS

            Semantic Rule:
            unary_op.addr = &MINUS
        */

        root->addr = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->addr;
        break;
    }

    case 78:
    {
        /*
            Grammar Rule:
            arithmeticOrBooleanExpr : anyTerm n7

            Semantic Rule:
            arithmeticOrBooleanExpr.syn_addr = n7.syn_addr
            arithmeticOrBooleanExpr.addr = anyTerm.addr
            n7.inh_addr = arithmeticOrBooleanExpr.addr
            FREE(anyTerm)
            FREE(n7)
        */

        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        root->secondLeftmostChild->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 79:
    {
        /*
            Grammar Rule:
            n7 : logicalOp anyTerm n7

            Semantic Rule:
            ADD_LEFT_CHILD_TO_NODE(logicalOp.addr, n7.inh_addr)
            ADD_RIGHT_CHILD_TO_NODE(logicalOp.addr, anyTerm.addr)
            n7-1.inh_addr = logicalOp.addr
            n7.syn_addr = n7-1.syn_addr
            FREE(anyTerm)
            FREE(logicalOp)
            FREE(n7-1)
        */

        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        ASTNode anyTerm = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        root->secondLeftmostChild->rightSibling->inhaddr = anyTerm;
        ASTNode n7_1 = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        root->addr->children[0] = root->inhaddr;
        root->addr->children[1] = n7_1;

        return root->addr;
        break;
    }

    case 80:
    {
        /*
            Grammar Rule:
            n7 : EPSILON

            Semantic Rule:
            n7.syn_addr = n7.inh_addr
            FREE(EPSILON)
        */

        root->synaddr = root->inhaddr;

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 81:
    {
        /*
            Grammar Rule:
            anyTerm : arithmeticExpr n8

            Semantic Rule:
            anyTerm.addr = arithmeticExpr.addr
            n8.inh_addr = anyTerm.addr
            anyTerm.syn_addr = n8.syn_addr
            FREE(arithmeticExpr)
            FREE(n8)
        */

        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        root->secondLeftmostChild->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 82:
    {
        /*
            Grammar Rule:
            n8 : relationalOp arithmeticExpr

            Semantic Rule:
            ADD_LEFT_CHILD_TO_NODE(relationalOp.addr, n8.inh_addr)
            ADD_RIGHT_CHILD_TO_NODE(relationalOp.addr, arithmeticExpr.addr)
            FREE(arithmeticExpr)
            FREE(relationalOp)
        */

        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        root->addr->children[0] = root->inhaddr;
        root->addr->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 83:
    {
        /*
            Grammar: [n8: EPSILON]

            Semantics:
            [n8.syn_addr = n8.inh_addr
            FREE(EPSILON)]
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->inhaddr;
        break;
    }

    case 84:
    {
        /*
            Grammar: [arithmeticExpr : term n4]

            Semantics:
            [arithmeticExpr.addr = term.addr
            n4.inh_addr = arithmeticExpr.addr
            arithmeticExpr.syn_addr = n4.syn_addr
            FREE(term)
            FREE(n4)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        root->secondLeftmostChild->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 85:
    {
        /*
            Grammar: [n4 : op1 term n4]

            Semantics:
            [ADD_LEFT_CHILD_TO_NODE(op1.addr, n4.inh_addr)
            ADD_RIGHT_CHILD_TO_NODE(op1.addr, term.addr)
            n4-1.inh_addr = op1.addr
            n4.syn_addr = n4-1.syn_addr
            FREE(term)
            FREE(op1)
            FREE(n4-1)]
        */

        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        // ASTNode op = createASTNode(root->addr->dataForPrint,2);
        root->addr->children[0] = root->inhaddr;
        root->addr->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        root->secondLeftmostChild->rightSibling->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        root->leftmostChild = NULL;
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 86:
    {
        /*
            Grammar: [n4 : EPSILON]

            Semantics:
            [n4.syn_addr = n4.inh_addr
            FREE(EPSILON)]
        */
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->inhaddr;
        break;
    }

    case 87:
    {
        /*
            Grammar: [term : factor n5]

            Semantics:
            [term.addr = factor.addr
            n5.inh_addr = term.addr
            term.syn_addr = n5.syn_addr
            FREE(factor)
            FREE(n5)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        root->secondLeftmostChild->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 88:
    {
        /*
            Grammar: [n5 : op2 factor n5]

            Semantics:
            [ADD_LEFT_CHILD_TO_NODE(op2.addr, n5.inh_addr)
            ADD_RIGHT_CHILD_TO_NODE(op2.addr, factor.addr)
            n5-1.inh_addr = op2.addr
            n5.syn_addr = n5-1.syn_addr
            FREE(factor)
            FREE(op2)
            FREE(n5-1)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        // ASTNode op = createASTNode(root->addr->dataForPrint,2);
        root->addr->children[0] = root->inhaddr;
        root->addr->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        root->secondLeftmostChild->rightSibling->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        root->leftmostChild = NULL;
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 89:
    {
        /*
            Grammar: [n5 : EPSILON]

            Semantics:
            [n5.syn_addr = n5.inh_addr
            FREE(EPSILON)]
        */
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->inhaddr;
        break;
    }

    case 90:
    {
        /*
            Grammar: [factor : BO arithmeticOrBooleanExpr BC]

            Semantics:
            [factor.addr = arithmeticOrBooleanExpr.syn_addr
            FREE(BO)
            FREE(arithmeticExpr)
            FREE(BC)]
        */
        root->addr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild->rightSibling = NULL;
        root->secondLeftmostChild = NULL;

        return root->addr;
        break;
    }

    case 91:
    {
        /*
            Grammar: [factor : NUM]

            Semantics:
            [factor.addr = &NUM]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 92:
    {
        /*
            Grammar: [factor : RNUM]

            Semantics:
            [factor.addr = &RNUM]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 93:
    {
        /*
            Grammar: [factor : boolConstt]

            Semantics:
            [factor.addr = boolConstt.addr
            FREE(boolConstt)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 94:
    {
        /*
            Grammar: [factor ID n_11]

            Semantics:
            [factor.addr = &ID;
            factor.indexIfArray = n_11.index
            FREE(n_11)]
        */
        ASTNode t = createASTNode(root->secondLeftmostChild->dataForPrint, 2);
        t->children[0] = createASTNode(root->leftmostChild->dataForPrint, 0);
        ASTNode temp = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        free(root->leftmostChild);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;
        if (temp != NULL)
        {
            t->children[1] = temp;
            t->dataForPrint->tokenName = malloc(sizeof(char) * (strlen("ARRAY_ELEMENT") + 1));
            strcpy(t->dataForPrint->tokenName, "ARRAY_ELEMENT");
            return t;
        }

        return t->children[0];
        break;
    }

    case 95:
    {
        /*
            Grammar: [n_11: SQBO element_index_with_expressions SQBC]

            Semantics:
            [n_11.index = element_index_with_expressions.addr
        */
        root->addr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        root->leftmostChild = NULL;
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;

        return root->addr;
        break;
    }

    case 96:
    {
        /*
            Grammar: [n_11: EPSILON]

            Semantics:
            [n_11.index = NULL]
        */
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return NULL;
        break;
    }

    case 97:
    {
        /*
            Grammar: [element_index_with_expressions: arrExpr]

            Semantics:
            [element_index_with_expressions.addr = arrExpr.addr
            FREE(arrExpr)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 98:
    {
        /*
            Grammar: [element_index_with_expressions: uarr]

            Semantics:
            [element_index_with_expressions.addr = uarr.addr
            FREE(uarr)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 99:
    {
        /*
            Grammar: [uarr: unary_op new_NT_arr]

            Semantics:
            [uarr.addr = ADD_RIGHT_CHILD_TO_NODE(unary_op.addr, new_NT_arr.addr)
            FREE(unary_op)
            FREE(new_NT_arr)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        root->addr->children[0] = NULL;
        root->addr->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;

        return root->addr;
        break;
    }

    case 100:
    {
        /*
            Grammar: [new_NT_arr: BO arrExpr BC]

            Semantics:
            [new_NT_arr.addr = arrExpr.addr
            FREE(BO)
            FREE(BC)
            FREE(arrExpr)]
        */
        root->addr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild->rightSibling = NULL;
        root->secondLeftmostChild = NULL;

        return root->addr;
        break;
    }

    case 101:
    {
        /*
            [Grammar: new_NT_arr: var_id_num]

            Semantics:
            [new_NT_arr.addr = var_id_num.addr
            FREE(var_id_num)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 102:
    {
        /*
            Grammar: [arrExpr: arrTerm arr_N4]

            Semantics:
            [arrExpr.syn_addr = arr_N4.syn_addr
            arrExpr.addr = arrTerm.addr
            arr_N4.inh_addr = arrTerm.addr
            FREE(arrTerm)
            FREE(arr_N4)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        root->secondLeftmostChild->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 103:
    {
        /*
            Grammar: [arr_N4: Pop1 arrTerm arr_N4]

            Semantics:
            [ADD_LEFT_CHILD_TO_NODE(op1.addr, arr_N4.inh_addr)
            ADD_RIGHT_CHILD_TO_NODE(op1.addr, arrTerm.addr)
            arr_N4-1.inh_addr = arr_N4.addr
            arr_N4.syn_addr = arr_N4-1.syn_addr
            FREE(arrTerm)
            FREE(op1)
            FREE(arr_N4-1)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        // ASTNode op = createASTNode(root->addr->dataForPrint,2);
        root->addr->children[0] = root->inhaddr;
        root->addr->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        root->secondLeftmostChild->rightSibling->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild->rightSibling = NULL;
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 104:
    {
        /*
            Grammar: [array_N4 : EPSILON]

            Semantics:
            [array_N4.syn_addr = array_N4.inh_addr
            FREE(EPSILON)]
        */
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->inhaddr;
        break;
    }

    case 105:
    {
        /*
            Grammar: [arraTerm: arrFactor arr_N5]

            Semantics:
            [arrTerm.syn_addr = arr_N5.syn_addr
            arrTerm.addr = arrFactor.addr
            arr_N5.inh_addr = arrFactor.addr
            FREE(arrFactor)
            FREE(arr_N5)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        root->secondLeftmostChild->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 106:
    {
        /*
            Grammar: [arr_N5: op2 arrFactor arr_N5]

            Semantics:
            [ADD_LEFT_CHILD_TO_NODE(op2.addr, arr_N5.inh_addr)
            ADD_RIGHT_CHILD_TO_NODE(op2.addr, arrFactor.addr)
            arr_N5-1.inh_addr = arr_N5.addr
            arr_N5.syn_addr = arr_N5-1.syn_addr
            FREE(arrFactor)
            FREE(op2)
            FREE(arr_N5-1)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        // ASTNode op = createASTNode(root->addr->dataForPrint,2);
        root->addr->children[0] = root->inhaddr;
        root->addr->children[1] = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        root->secondLeftmostChild->rightSibling->inhaddr = root->addr;
        root->synaddr = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild->rightSibling = NULL;
        root->secondLeftmostChild = NULL;

        return root->synaddr;
        break;
    }

    case 107:
    {
        /*
            Grammar: [arr_N5: EPSILON]

            Semantics:
            [arr_N5.syn_addr = arr_N5.inh_addr
            FREE(EPSILON)]
        */
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return root->inhaddr;
        break;
    }

    case 108:
    {
        /*
            Grammar: [arrFactor: ID]

            Semantics:
            [arrFactor.addr = &ID]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 109:
    {
        /*
            Grammar: [arrFactor: NUM]

            Semantics:
            [arrFactor.addr = &NUM]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 110:
    {
        /*
            Grammar: [arrFactor: boolConstt]

            Semantics:
            [arrFactor.addr = boolConstt.addr
            FREE(boolConstt)]
        */
        root->addr = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 111:
    {
        /*
            Grammar: [arrFactor: BO arrExpr BC]

            Semantics:
            [arrFactor.addr = arrExpr.addr
            FREE(arrExpr)
            FREE(BO)
            FREE(BC)]
        */
        root->addr = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);

        free(root->leftmostChild);
        free(root->secondLeftmostChild->rightSibling);
        free(root->secondLeftmostChild);
        root->leftmostChild = NULL;
        root->secondLeftmostChild->rightSibling = NULL;
        root->secondLeftmostChild = NULL;

        return root->addr;
        break;
    }

    case 112:
    {
        /*
            Grammar: [op1 : PLUS]

            Semantics:
            [op1.addr = &PLUS]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 113:
    {
        /*
            Grammar: [op1 : MINUS]

            Semantics:
            [op1.addr = &MINUS]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 114:
    {
        /*
            Grammar: [op2 : MUL]

            Semantics:
            [op2.addr = &MUL]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 115:
    {
        /*
            Grammar: [op2 : DIV]

            Semantics:
            [op2.addr = &DIV]
        */
        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }
    case 116:
    {
        // Grammar - logicalOp : AND
        // Semantics - logicalOp.addr = &AND

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 117:
    {
        // Grammar - logicalOp : OR
        // Semantics - logicalOp.addr = &OR

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 118:
    {
        // Grammar - relationalOp : LT
        // Semantics - relationalOp.addr = &LT

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 119:
    {
        // Grammar - relationalOp : LE
        // Semantics - relationalOp.addr = &LE

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 120:
    {
        // Grammar - relationalOp : GT
        // Semantics - relationalOp.addr = &GT

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 121:
    {
        // Grammar - relationalOp : GE
        // Semantics - relationalOp.addr = &GE

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 122:
    {
        // Grammar - relationalOp : EQ
        // Semantics - relationalOp.addr = &EQ

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 123:
    {
        // Grammar - relationalOp : NE
        // Semantics - relationalOp.addr = &NE

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 2);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 124:
    {
        // Grammar - declareStmt : DECLARE idList COLON dataType SEMICOL

        /*
           Semantics -
            declareStmt.addr = CREATE_NODE("DECLARE", idList.head, dataType.addr)
            FREE(DECLARE)
            FREE(COLON)
            FREE(SEMICOL)
            FREE(idList)
            FREE(dataType)
        */

        ASTNode idListNode = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        ASTNode dataTypeNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);

        ASTNode moduleNode = createASTNode(root->dataForPrint, 2);
        moduleNode->children[0] = idListNode;
        moduleNode->children[1] = dataTypeNode;

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return moduleNode;
        break;
    }

    case 125:
    {
        // Grammar - condionalStmt : SWITCH BO ID BC START caseStmts default_ END
        /*
           Semantics -
            conditionalStmt.addr = CREATE_NODE("SWITCH", &ID, caseStmts.head, default_.addr)
            FREE(SWITCH)
            FREE(BO)
            FREE(BC)
            FREE(START)
            FREE(END)
            FREE(caseStmts)
            FREE(default_)
        */

        ASTNode id = createASTNode(root->secondLeftmostChild->rightSibling->dataForPrint, 0);
        ASTNode cstsNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);
        ASTNode default_Node = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);

        ASTNode moduleNode = createASTNode(root->dataForPrint, 3);
        moduleNode->children[0] = id;
        moduleNode->children[1] = cstsNode;
        moduleNode->children[2] = default_Node;

        moduleNode->range_ast[0]=id->dataForPrint->lineNo;
        moduleNode->range_ast[1]=root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->dataForPrint->lineNo;
        // printf("case from %d %d\n",moduleNode->range_ast[0],moduleNode->range_ast[1]);
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return moduleNode;
        break;
    }

    case 126:
    {
        // Grammar - caseStmts : CASE value COLON statements BREAK SEMICOL n9
        /*
           Semantics -
            INSERT_AT_END(caseStmts.head, CREATE_NODE("CASE", value.addr, statements.head))
            n9.head = caseStmts.head
            FREE(CASE)
            FREE(COLON)
            FREE(BREAK)
            FREE(SEMICOL)
            FREE(value)
            FREE(statements)
            FREE(n9)
        */

        ASTNode csmtNode = createASTNode(root->dataForPrint, 0);
        csmtNode->info_list = malloc(sizeof(struct ASTList));
        csmtNode->info_list->size = 0;
        csmtNode->info_list->head = NULL;
        csmtNode->info_list->tail = NULL;
        ASTNode casestmt = createASTNode(root->dataForPrint, 2);
        casestmt->range_ast[0]=root->leftmostChild->dataForPrint->lineNo;
        casestmt->range_ast[1]=root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->dataForPrint->lineNo;
        // printf("case from %d %d\n",casestmt->range_ast[0],casestmt->range_ast[1]);
        ASTNode valueNode = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        ASTNode statementsNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);
        casestmt->children[0] = valueNode;
        casestmt->children[1] = statementsNode;

        insertAtEnd(csmtNode->info_list, casestmt);

        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->addr = csmtNode;
        getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return csmtNode;
        break;
    }

    case 127:
    {
        // Grammar - n9 : CASE value COLON statements BREAK SEMICOL n9
        /*
           Semantics -
            INSERT_AT_END(n9.head, CREATE_NODE("CASE", value.addr, statements.head))
            n9-1.head = n9.head
            FREE(CASE)
            FREE(COLON)
            FREE(BREAK)
            FREE(SEMICOL)
            FREE(value)
            FREE(statements)
            FREE(n9-1)
        */

        ASTNode csmtNode = createASTNode(root->dataForPrint, 2);
        csmtNode->range_ast[0]=root->leftmostChild->dataForPrint->lineNo;
        csmtNode->range_ast[1]=root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->dataForPrint->lineNo;
        // printf("case from %d %d\n",csmtNode->range_ast[0],csmtNode->range_ast[1]);
        ASTNode valueNode = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        ASTNode statementsNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rule_num);
        csmtNode->children[0] = valueNode;
        csmtNode->children[1] = statementsNode;
        insertAtEnd(root->addr->info_list, csmtNode);

        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->addr = root->addr;
        getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return root->addr;
        break;
    }

    case 128:
    {
        // Grammar - n9 : EPSILON
        /*
           Semantics -
            FREE(EPSILON)
        */

        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return NULL;
        break;
    }

    case 129:
    {
        // Grammar - default_ : DEFAULT COLON statements BREAK SEMICOL
        /*
           Semantics -
            default_.addr = statements.head
            FREE(DEFAULT)
            FREE(COLON)
            FREE(BREAK)
            FREE(SEMICOL)
            FREE(statements)
        */

        ASTNode statementsNode = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);
        if(statementsNode!=NULL){
            statementsNode->range_ast[0]=root->leftmostChild->dataForPrint->lineNo;
            statementsNode->range_ast[1]=root->secondLeftmostChild->rightSibling->rightSibling->dataForPrint->lineNo;
        }
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return statementsNode;
        break;
    }

    case 130:
    {
        // Grammar - default_ : EPSILON
        // Semantics - default_.addr = NULL
        // FREE(EPSILON)

        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return NULL;
        break;
    }

    case 131:
    {
        // Grammar - value : NUM
        // Semantics - value.addr = &NUM

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 132:
    {
        // Grammar - value : TRUE
        // Semantics - value.addr = &TRUE

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 133:
    {
        // Grammar - value : FALSE
        // Semantics - value.addr = &FALSE

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }

    case 134:
    {
        // Grammar - [iterativeStmt : FOR BO ID IN range_for_loop BC START statements END]
        /*
           Semantics -
            iterativeStmt.addr = CREATE_NODE("FOR", &ID, range_for_loop.addr, statements.head)
            FREE(FOR)
            FREE(IN)
            FREE(BO)
            FREE(BC)
            FREE(END)
            FREE(START)
            FREE(range_for_loop)
            FREE(statements)
        */

        ASTNode id = createASTNode(root->secondLeftmostChild->rightSibling->dataForPrint, 0);
        ASTNode rangeNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rule_num);
        ASTNode stmtNode = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);

        ASTNode moduleNode = createASTNode(root->leftmostChild->dataForPrint, 3);
        moduleNode->children[0] = id;
        moduleNode->children[1] = rangeNode;
        moduleNode->children[2] = stmtNode;
        moduleNode->range_ast[0]=root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->dataForPrint->lineNo;
        moduleNode->range_ast[1]=root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->dataForPrint->lineNo;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return moduleNode;
        break;
    }

    case 135:
    {
        // Grammar - [iterativeStmt : WHILE BO arithmeticOrBooleanExpr BC START statements END]
        /*
           Semantics -
            iterativeStmt.addr = CREATE_NODE("WHILE", arithmeticOrBooleanExpr.syn_addr, statements.head)
            FREE(WHILE)
            FREE(BO)
            FREE(BC)
            FREE(START)
            FREE(END)
            FREE(statements)
            FREE(arithmeticOrBooleanExpr)
        */

        ASTNode arorbono = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);
        ASTNode statementsNo = getASTInformation(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling, root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rule_num);

        ASTNode moduleNode = createASTNode(root->leftmostChild->dataForPrint, 2);
        moduleNode->children[0] = arorbono;
        moduleNode->children[1] = statementsNo;
        moduleNode->range_ast[0]=root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->dataForPrint->lineNo;
        moduleNode->range_ast[1]=root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->dataForPrint->lineNo;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling->rightSibling);
        root->secondLeftmostChild->rightSibling->rightSibling = NULL;
        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return moduleNode;
        break;
    }

    case 136:
    {
        /*
            Grammar: [range_for_loop : index_for_loop RANGEOP index_for_loop]

            Semantics:
            [
                range_for_loop.addr = CREATE_NODE("RANGE_FOR", index_for_loop.addr, index_for_loop-1.addr)
                FREE(index_for_loop)
                FREE(index_for_loop-1)
                FREE(RANGEOP)
            ]
        */

        ASTNode rangeforloopNode = createASTNode(root->dataForPrint, 2);
        rangeforloopNode->children[0] = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        rangeforloopNode->children[1] = getASTInformation(root->secondLeftmostChild->rightSibling, root->secondLeftmostChild->rightSibling->rule_num);

        free(root->secondLeftmostChild->rightSibling);
        root->secondLeftmostChild->rightSibling = NULL;
        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return rangeforloopNode;
        break;
    }

    case 137:
    {
        /*
            Grammar: [index_for_loop : sign new_index_for_loop]

            Semantics:
            [
                if(sign.addr != NULL) {
                ADD_RIGHT_CHILD_TO_NODE(sign.addr, new_index_for_loop.addr)
                index_for_loop.addr = sign.addr
                }
                else {
                index_for_loop.addr = new_index_for_loop.addr
                FREE(sign)
                }
                FREE(new_index_for_loop)
            ]
        */

        ASTNode signNode = getASTInformation(root->leftmostChild, root->leftmostChild->rule_num);
        ASTNode newindexforLoopNode = getASTInformation(root->secondLeftmostChild, root->secondLeftmostChild->rule_num);
        ASTNode indexforLoopNode = NULL;

        if (signNode != NULL)
        {
            signNode->children[0] = NULL;
            signNode->children[1] = newindexforLoopNode;
            indexforLoopNode = signNode;
        }
        else
        {
            indexforLoopNode = newindexforLoopNode;
        }

        free(root->secondLeftmostChild);
        root->secondLeftmostChild = NULL;
        free(root->leftmostChild);
        root->leftmostChild = NULL;

        return indexforLoopNode;
        break;
    }

    case 138:
    {
        // Grammar - new_index_for_loop : NUM
        // Semantics - new_index_for_loop.addr = &NUM

        ASTNode t = createASTNode(root->leftmostChild->dataForPrint, 0);
        free(root->leftmostChild);
        root->leftmostChild = NULL;
        return t;
        break;
    }
    }
    return NULL;
}


// int main()
// {
//     // Create a parse tree
//     /*
//     parseTree ptree = malloc(sizeof(struct parseTree));
//     ptree->root = NULL;

//     // Create a parse tree node
//     parseTreeNodeData data = createParseTreeNodeData("--", 1, "--", INT_MIN , "artithmeticExpr", "--");
//     treeNode node = createTreeNode(data, 1, 85);
//     ptree->root = node;
    
//     // Create a parse tree node
//     parseTreeNodeData data1 = createParseTreeNodeData("--", 1, "--", INT_MIN , "term", "artithmeticExpr");
//     treeNode node1 = createTreeNode(data1, 1, 88);

//     // Create a parse tree node
//     parseTreeNodeData data2 = createParseTreeNodeData("--", 1, "--", INT_MIN , "n4", "arithmeticExpr");
//     treeNode node2 = createTreeNode(data2, 0, 86);

//     node->leftmostChild = node1;
//     node1->rightSibling = node2;
//     node->secondLeftmostChild = node2;
//     node->numOfChildren = 2;

//     // Create a parse tree node
//     parseTreeNodeData data3 = createParseTreeNodeData("--", 1, "--", INT_MIN , "factor", "term");
//     treeNode node3 = createTreeNode(data3, 1, 92);

//     // Create a parse tree node
//     parseTreeNodeData data4 = createParseTreeNodeData("--", 1, "--", INT_MIN , "n5", "term");
//     treeNode node4 = createTreeNode(data4, 0, 89);

//     node1->leftmostChild = node3;
//     node3->rightSibling = node4;
//     node1->secondLeftmostChild = node4;
//     node1->numOfChildren = 2;

//     // Create a parse tree node
//     parseTreeNodeData data5 = createParseTreeNodeData("3", 1, "NUM", 3 , "NUM", "factor");
//     treeNode node5 = createTreeNode(data5, 1, INT_MIN);

//     node3->leftmostChild = node5;
//     node3->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data6 = createParseTreeNodeData("--", 1, "--", INT_MIN , "op2", "n5");
//     treeNode node6 = createTreeNode(data6, 1, 115);

//     node4->leftmostChild = node6;
//     node4->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data7 = createParseTreeNodeData("*", 1, "MUL", INT_MIN , "MUL", "op2");
//     treeNode node7 = createTreeNode(data7, 1, INT_MIN);

//     node6->leftmostChild = node7;
//     node6->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data8 = createParseTreeNodeData("--", 1, "--", INT_MIN , "factor", "n5");
//     treeNode node8 = createTreeNode(data8, 0, 92);

//     node4->secondLeftmostChild = node8;
//     node6->rightSibling = node8;
//     node4->numOfChildren = 2;

//     // Create a parse tree node
//     parseTreeNodeData data9 = createParseTreeNodeData("4", 1, "NUM", 4 , "NUM", "factor");
//     treeNode node9 = createTreeNode(data9, 1, INT_MIN);

//     node8->leftmostChild = node9;
//     node8->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data10 = createParseTreeNodeData("--", 1, "--", INT_MIN , "n5", "n5");
//     treeNode node10 = createTreeNode(data10, 0, 90);

//     node8->rightSibling = node10;
//     node4->numOfChildren = 3;

//     // Create a parse tree node
//     parseTreeNodeData data11 = createParseTreeNodeData("--", 1, "EPSILON", INT_MIN , "EPSILON", "n5");
//     treeNode node11 = createTreeNode(data11, 1, INT_MIN);

//     node10->leftmostChild = node11;
//     node10->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data12 = createParseTreeNodeData("--", 1, "--", INT_MIN , "op1", "n4");
//     treeNode node12 = createTreeNode(data12, 1, 113);

//     node2->leftmostChild = node12;
//     node2->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data13 = createParseTreeNodeData("+", 1, "PLUS", INT_MIN , "PLUS", "op1");
//     treeNode node13 = createTreeNode(data13, 1, INT_MIN);

//     node12->leftmostChild = node13;
//     node12->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data14 = createParseTreeNodeData("--", 1, "--", INT_MIN , "term", "n4");
//     treeNode node14 = createTreeNode(data14, 0, 88);

//     node2->secondLeftmostChild = node14;
//     node12->rightSibling = node14;
//     node2->numOfChildren = 2;

//     // Create a parse tree node
//     parseTreeNodeData data15 = createParseTreeNodeData("--", 1, "--", INT_MIN , "factor", "term");
//     treeNode node15 = createTreeNode(data15, 1, 92);

//     node14->leftmostChild = node15;
//     node14->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data16 = createParseTreeNodeData("2", 1, "NUM", 2 , "NUM", "factor");
//     treeNode node16 = createTreeNode(data16, 1, INT_MIN);

//     node15->leftmostChild = node16;
//     node15->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data17 = createParseTreeNodeData("--", 1, "--", INT_MIN , "n5", "term");
//     treeNode node17 = createTreeNode(data17, 0, 90);

//     node15->rightSibling = node17;
//     node14->secondLeftmostChild = node17;
//     node14->numOfChildren = 2;

//     // Create a parse tree node
//     parseTreeNodeData data18 = createParseTreeNodeData("--", 1, "EPSILON", INT_MIN , "EPSILON", "n5");
//     treeNode node18 = createTreeNode(data18, 1, INT_MIN);

//     node17->leftmostChild = node18;
//     node17->numOfChildren = 1;

//     // Create a parse tree node
//     parseTreeNodeData data19 = createParseTreeNodeData("--", 1, "--", INT_MIN , "n4", "n4");
//     treeNode node19 = createTreeNode(data19, 0, 87);

//     node14->rightSibling = node19;
//     node2->numOfChildren = 3;

//     // Create a parse tree node
//     parseTreeNodeData data20 = createParseTreeNodeData("--", 1, "EPSILON", INT_MIN , "EPSILON", "n4");
//     treeNode node20 = createTreeNode(data20, 1, INT_MIN);

//     node19->leftmostChild = node20;
//     node19->numOfChildren = 1;

//     printf("Parse Tree: \n");
//     printParseTree(ptree, "tree_out.txt");

//     AST ast = (AST)malloc(sizeof(struct AST));
//     ast->root = NULL;

//     ast->root = getASTInformation(ptree->root, ptree->root->rule_num);
//     printf("\nAST: \n");
//     displayAST(ast->root);
//     */

    

//     return 0;
// }
     