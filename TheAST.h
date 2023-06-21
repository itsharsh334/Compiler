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

parseTreeNodeData copyField(parseTreeNodeData data);

void displayParseTreeNodeData(parseTreeNodeData data);

treeNode createTreeNode(parseTreeNodeData data, int whetherFirstChild, int rule_num);

parseTreeNodeData createParseTreeNodeData(char *lexeme, int lineNo, char *tokenName, int valueIfNumber, char *currentNodeSymbol, char *parentNodeSymbol);

ASTNode createASTNode(parseTreeNodeData data, int num_children);

void displayAST(ASTNode root);

void displayASTprint(ASTNode root);

ASTNode getASTInformation(treeNode root, int rule_num);

