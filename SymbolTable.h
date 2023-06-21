/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#include "SymbolTableDef.h"

type_pair createErrorType(char *errormsg);

hash_component searchScopes(hash_table_holder root, char *identifier);

void insertInList(ip_op_list list, ip_op_info node);

void create_while_para(ASTNode node, ip_op_list list);

void displayErrorType(type_pair t);

type_pair getTypeExpression(ASTNode node, hash_table_holder l);

void swap(int i, int j);

int greaterIndex(int i, int j);

int leftChild(int i);

int rightChild(int i);

int getParent(int i);

void heapify(int i, int size);

void build_heap(int size);

void heapSort(int size);

type_pair array_element_expression_check(ASTNode node, hash_table_holder root);

type_pair array_element_check(ASTNode node, hash_table_holder root);

type_pair getTypeExpression(ASTNode node, hash_table_holder l);

type_pair loopCounterAssignChecker(ASTNode node, char *loop_counter_variable);

type_pair MDChecker(ASTNode node, char *MD_variable);

int getOffsetArrayElement(char *array_name, int index, hash_table_holder root);

void displayModuleCallPairs(char **moduleCallPairs, int size);

type_pair semanticCheck(ASTNode node, type_check_state state, hash_table_holder root);

void callSemanticCheck(ASTNode node, type_check_state state, hash_table_holder root);

int detectRecursion(int curr, int directed_graph[]);

int getOffset(char *identifier, hash_table_holder current);

int isInSet(Set *s, int x);

void insertIntoSet(Set *s, int x);

void removeFromSet(Set *s, int x);

Set *createIntersection(Set *s1, Set *s2);

Set *createUnion(Set *s1, Set *s2);

void printSet(Set *s);