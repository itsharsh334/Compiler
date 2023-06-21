/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#include "Hashing.c"
#include <time.h>

int GLOBAL_COUNTER = 1;
int UID_COUNTER = 0;   // counter to generate unique ids for all function symbol tables
char **module_calls;   // string array to store the module calls in this fashion: {caller_1, callee_1, caller_2, callee_2, ...}
char **module_names;   // string array to store the module names in this fashion: {module_1, module_2, ...}
int curr_size = 0;     // current size of the module_calls array
int curr_ptr = 0;      // current pointer to the module_calls array
int cycle_origin = -1; // stores the index of the module that is the origin of the cycle
int *path;
int i_path = 0;
int *visited;                        // stores the visited modules while checking for cycles (or recursion)
int *visited_on_current_path;        // stores the visited modules on the current path while checking for cycles (or recursion)
freeze_state *pending_module_checks; // stores the modules that are pending to be checked while reuse
int freeze_state_size = 1;           // current size of the pending_module_checks array

int num_semantic_errors = 0;

hash_table_holder BIG_ROOT = NULL;

enum AST_State
{
    ROOT_STATE,     //[The Starting Point]
    DECLARE_STATE,  //[The State when we have a Declare Statement] // Antriksh
    IP_STATE,       //[The State when we have an Input Parameter List]
    OP_STATE,       //[The State when we have an Output Parameter List]
    FUNCTION_STATE, //[The State when we have an Individual Function/driver module]
    DRIVER_STATE,   //[The State when we have a Driver Module]
    // ITERATIVE_STATE,    //[The State when we have an Iterative Statement] // Kaustab
    STATEMENTS_STATE //[The State when we have a List of Statements] // Kaustab
    // CASE_STATE        //[The State when we have a Switch Statement] // Antriksh
};
typedef enum AST_State AST_State;

enum type_check_state
{
    CONDITIONAL_CHECK,       // send this state when you are sending conditionalStmt
    ASSIGN_CHECK,            // for simplestmt, assignmentstmt
    MODULE_REUSE_CHECK,      // for simplestmt , moduleReusestmt
    WHILE_EXPRESSION_CHECK,  // type matching in while expression
    FOR_CHECK,               // checks to ensure loop parameters are not reassigned value
    MODULE_DEFINITION_CHECK, // to ensure O/P parameters are assigned values inside function, call after creating the symbol table for moduleDefiniton.
    // following options to be used by semanticCheck function for easy usage
    PRINT_VALUE_CHECK, // to be called from IO_CHECK
    GET_VALUE_CHECK,   // to be called from IO_CHECK

};
typedef enum type_check_state type_check_state;

type_pair createErrorType(char *errormsg)
{
    all_errors[semantic_error_count] = (collection_error)malloc(sizeof(struct collection_error));
    char ln[10];
    int cnt = 0;
    for (int i = 0; i < strlen(errormsg); i++)
    {
        if (errormsg[i] == '|')
        {
            break;
        }
        if (errormsg[i] >= '0' && errormsg[i] <= '9')
        {
            ln[cnt] = errormsg[i];
            cnt++;
        }
    }
    ln[cnt] = '\0';
    all_errors[semantic_error_count]->line_num = atoi(ln);

    num_semantic_errors++;
    type_pair t, treturn;
    t = malloc(sizeof(struct type_pair));
    t->type_name = ERROR_TYPE;
    t->type_expr.error_message = malloc(sizeof(char) * (strlen(errormsg) + 1));
    strcpy(t->type_expr.error_message, errormsg);
    treturn = malloc(sizeof(struct type_pair));
    treturn->type_name = ERROR_TYPE;
    treturn->type_expr.error_message = malloc(sizeof(char) * (strlen(errormsg) + 1));
    strcpy(treturn->type_expr.error_message, errormsg);
    all_errors[semantic_error_count]->err = t;
    semantic_error_count++;
    return treturn;
}

void displayErrorType(type_pair t)
{
    if (t->type_expr.error_message != NULL)
        printf("%s\n", t->type_expr.error_message);
    else
        printf("Error Message is NULL\n");
    return;
}

hash_component searchScopes(hash_table_holder root, char *identifier)
{
    hash_table_holder temp = root;
    hash_component t = NULL;
    while (temp != NULL)
    {
        t = searchHashTable(temp->hash_table, identifier);
        if (t != NULL)
            break;
        temp = temp->parent;
    }
    return t;
}

void insertInList(ip_op_list list, ip_op_info node)
{
    /*
        - Just another simple function to insert a node in the ip_op_list at the end
    */
    if (node == NULL)
        return;

    if (list->size == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
        list->tail->next = NULL;
    }
    list->size++;
}

void create_while_para(ASTNode node, ip_op_list list)
{

    if (node == NULL)
        return;
    else if (node->num_children == 0 && node->dataForPrint->u.term == ID)
    {
        ip_op_info t = (ip_op_info)malloc(sizeof(struct ip_op_info));
        t->type = NULL;
        strcpy(t->identifier, node->dataForPrint->lexeme);
        insertInList(list, t);
        return;
    }
    else if (node->num_children == 2 && strcmp(node->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
    {
        ip_op_info t = (ip_op_info)malloc(sizeof(struct ip_op_info));
        t->type = NULL;
        strcpy(t->identifier, node->children[0]->dataForPrint->lexeme);
        insertInList(list, t);
        return;
    }
    if (node->children != NULL && node->num_children == 2)
    {
        create_while_para(node->children[0], list);
        create_while_para(node->children[1], list);
    }
}

type_pair getTypeExpression(ASTNode node, hash_table_holder l);

void swap(int i, int j)
{
    collection_error t = all_errors[i];
    all_errors[i] = all_errors[j];
    all_errors[j] = t;
}

int greaterIndex(int i, int j)
{
    if (all_errors[i]->line_num < all_errors[j]->line_num)
        return j;
    else
        return i;
}

int leftChild(int i)
{
    return 2 * i + 1;
}

int rightChild(int i)
{
    return 2 * i + 2;
}

int getParent(int i)
{
    if (i % 2 == 0)
        return i / 2;
    else
        return (i - 1) / 2;
}

void heapify(int i, int size)
{
    int left = leftChild(i);
    int right = rightChild(i);

    if (right < size)
    {
        if (all_errors[left]->line_num > all_errors[i]->line_num ||
            all_errors[right]->line_num > all_errors[i]->line_num) //(a[l]>a[i] || a[r]>a[i])
        {
            int max = greaterIndex(left, right);
            swap(i, max);
            heapify(max, size);
        }
        else
            return;
    }
    else if (right >= size && left < size)
    {
        if (all_errors[left]->line_num > all_errors[i]->line_num)
        {
            swap(i, left);
            return;
        }
        else
            return;
    }
    else if (right >= size && left >= size)
        return;

    return;
}

void build_heap(int size)
{
    for (int i = size / 2 - 1; i >= 0; i--)
        heapify(i, size);

    return;
}

void heapSort(int size)
{
    for (int i = size - 1; i >= 1; i--)
    {
        swap(0, i);
        heapify(0, i);
    }
    // swap(&a[0],&a[1]);
    return;
}

type_pair array_element_expression_check(ASTNode node, hash_table_holder root)
{
    type_pair type_return = (type_pair)malloc(sizeof(struct type_pair));
    hash_table_holder temp = root;
    hash_component t;
    ASTNode arr_ID_print = node->children[0];
    // replacebysearch
    t = searchScopes(temp, arr_ID_print->dataForPrint->lexeme);
    if (t == NULL)
    {
        char errormsg[100];
        sprintf(errormsg, "ERROR in Line %d | Array ID is not declared previously\n", node->children[0]->dataForPrint->lineNo);
        type_return = createErrorType(errormsg);
        // displayErrorType(type_return);
        return type_return;
    }
    else
    {
        ASTNode index_print = node->children[1];
        // check if this index print is whithin array bound, if its a variable then check it should be of int type
        if (strcmp(index_print->dataForPrint->tokenName, "ID") == 0)
        {
            hash_component index_hash_component = searchScopes(temp, index_print->dataForPrint->lexeme);
            if (index_hash_component == NULL)
            {
                // index not declared previously
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | ID %s is not declared previously\n", index_print->dataForPrint->lineNo, index_print->dataForPrint->lexeme);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
            else if (index_hash_component->content.type != INTEGER)
            {
                // index not of INT datatype
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | ID %s is not of INT datatype\n", index_print->dataForPrint->lineNo, index_print->dataForPrint->lexeme);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }
        else if (strcmp(index_print->dataForPrint->tokenName, "NUM") == 0)
        {
            // index is num, doing bound checking
            int num_index_print = index_print->dataForPrint->valueIfNumber;
            if (t->content.array_info->is_static == 3)
            {
                int left_bound = t->content.array_info->range.static_range[0];
                int right_bound = t->content.array_info->range.static_range[1];
                if ((right_bound >= left_bound && (num_index_print < left_bound || num_index_print > right_bound)) || (right_bound <= left_bound && (num_index_print < right_bound || num_index_print > left_bound)))
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Index %d out of bounds\n", index_print->dataForPrint->lineNo, num_index_print);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
        }
        else
        {
            // give error, index not of valid Type
            type_return = getTypeExpression(index_print, root);
            if (type_return->type_name == ERROR_TYPE)
            {
                return type_return;
            }
            char errormsg[100];
            if (type_return->type_name != PRIMITIVE_TYPE || type_return->type_expr.data_type != INTEGER)
            {
                sprintf(errormsg, "ERROR in Line %d | Array Index is not a valid value\n", index_print->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }
    }
    type_return->type_name = ARRAY_TYPE;
    type_return->type_expr.array_info = t->content.array_info;
    return type_return;
}

type_pair array_element_check(ASTNode node, hash_table_holder root)
{
    type_pair type_return = (type_pair)malloc(sizeof(struct type_pair));
    hash_table_holder temp = root;
    hash_component t;
    ASTNode arr_ID_print = node->children[0];
    // replacebysearch
    t = searchScopes(temp, arr_ID_print->dataForPrint->lexeme);
    if (t == NULL)
    {
        char errormsg[100];
        sprintf(errormsg, "ERROR in Line %d | Array ID is not declared previously\n", node->children[0]->dataForPrint->lineNo);
        type_return = createErrorType(errormsg);
        // displayErrorType(type_return);
        return type_return;
    }
    else
    {

        ASTNode index_print = node->children[1];
        if (strcmp(index_print->dataForPrint->tokenName, "MINUS") == 0)
        {
            if (index_print->children[0] == NULL)
                index_print = index_print->children[1];
        }
        // check if this index print is whithin array bound, if its a variable then check it should be of int type
        if (strcmp(index_print->dataForPrint->tokenName, "ID") == 0)
        {
            hash_component index_hash_component = searchScopes(temp, index_print->dataForPrint->lexeme);
            if (index_hash_component == NULL)
            {
                // index not declared previously
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | ID %s is not declared previously\n", index_print->dataForPrint->lineNo, index_print->dataForPrint->lexeme);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
            else if (index_hash_component->content.type != INTEGER)
            {
                // index not of INT datatype
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | ID %s is not of INT datatype\n", index_print->dataForPrint->lineNo, index_print->dataForPrint->lexeme);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }
        else if (strcmp(index_print->dataForPrint->tokenName, "NUM") == 0)
        {
            // index is num, doing bound checking
            int num_index_print = index_print->dataForPrint->valueIfNumber;
            if (strcmp(node->children[1]->dataForPrint->tokenName, "MINUS") == 0)
            {
                num_index_print = -1 * num_index_print;
            }
            if (t->content.array_info->is_static == 3)
            {
                int left_bound = t->content.array_info->range.static_range[0];
                int right_bound = t->content.array_info->range.static_range[1];
                if ((right_bound >= left_bound && (num_index_print < left_bound || num_index_print > right_bound)) || (right_bound <= left_bound && (num_index_print < right_bound || num_index_print > left_bound)))
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Index %d out of bounds\n", index_print->dataForPrint->lineNo, num_index_print);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
        }
        else
        {
            // give error, index not of valid Type
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | Array Index is not a valid value\n", index_print->dataForPrint->lineNo);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            return type_return;
        }
    }
    type_return->type_name = ARRAY_TYPE;
    type_return->type_expr.array_info = t->content.array_info;
    return type_return;
}

type_pair getTypeExpression(ASTNode node, hash_table_holder l)
{
    type_pair type_return = (type_pair)malloc(sizeof(struct type_pair));
    int child_count = node->num_children;

    switch (child_count)
    {
    case 0:
    {
        if (node->dataForPrint->u.term == NUM)
        {
            type_return->type_name = PRIMITIVE_TYPE;
            type_return->type_expr.data_type = INTEGER;
        }

        else if (node->dataForPrint->u.term == RNUM)
        {
            type_return->type_name = PRIMITIVE_TYPE;
            type_return->type_expr.data_type = REAL;
        }

        else if (node->dataForPrint->u.term == TRUE || node->dataForPrint->u.term == FALSE)
        {
            type_return->type_name = PRIMITIVE_TYPE;
            type_return->type_expr.data_type = BOOLEAN;
        }

        else if (node->dataForPrint->u.term == ID)
        {
            hash_component t = searchScopes(l, node->dataForPrint->lexeme);
            if (t)
            {
                if (t->info == PRIMITIVE_TYPE || t->info == IP_PRIMITIVE_TYPE || t->info == OP_PRIMITIVE_TYPE)
                {
                    type_return->type_name = PRIMITIVE_TYPE;
                    type_return->type_expr.data_type = t->content.type;
                }

                else if (t->info == ARRAY_TYPE || t->info == IP_ARRAY_TYPE)
                {
                    type_return->type_name = ARRAY_TYPE;
                    type_return->type_expr.array_info = t->content.array_info;
                }
            }
            else
            {
                // Error message:
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | Undeclared variable used %s\n", node->dataForPrint->lineNo, node->dataForPrint->lexeme);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }

        else
        {
            // Error message:
            char errormsg[70];
            sprintf(errormsg, "ERROR in Line %d | Incompatible Data Types used\n", node->dataForPrint->lineNo);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            return type_return;
        }

        break;
    }

    case 2:
    {
        if (strcmp(node->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
        {
            hash_component t = searchScopes(l, node->children[0]->dataForPrint->lexeme);
            if (!t)
            {
                // Error message:
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | Undeclared Array %s used\n", node->dataForPrint->lineNo, node->children[0]->dataForPrint->lexeme);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
            type_return = array_element_expression_check(node, l);
            if (type_return->type_name == ERROR_TYPE)
                return type_return;
            type_return->type_name = PRIMITIVE_TYPE;
            type_return->type_expr.data_type = t->content.array_info->data_type_e;
        }

        else if (node->children[0] == NULL) // handling Unary OP
        {
            type_return = getTypeExpression(node->children[1], l);

            if (type_return->type_name == PRIMITIVE_TYPE)
            {
                if (type_return->type_expr.data_type != INTEGER && type_return->type_expr.data_type != REAL)
                {
                    // Error message:
                    char errormsg[70];
                    sprintf(errormsg, "ERROR in Line %d | Incompatible Data Type used\n", node->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
        }

        else if (node->dataForPrint->u.term == PLUS || node->dataForPrint->u.term == MINUS || node->dataForPrint->u.term == MUL)
        {
            type_pair left_component = getTypeExpression(node->children[0], l);
            type_pair right_component = getTypeExpression(node->children[1], l);
            if (left_component->type_name == ERROR_TYPE)
            {
                return left_component;
            }
            if (right_component->type_name == ERROR_TYPE)
            {
                return right_component;
            }
            if (left_component->type_name == PRIMITIVE_TYPE && right_component->type_name == PRIMITIVE_TYPE)
            {
                if (left_component->type_expr.data_type == right_component->type_expr.data_type && (left_component->type_expr.data_type == INTEGER || left_component->type_expr.data_type == REAL))
                {
                    type_return->type_name = PRIMITIVE_TYPE;
                    type_return->type_expr.data_type = left_component->type_expr.data_type;
                }

                else
                {
                    // Error message:
                    char errormsg[70];
                    sprintf(errormsg, "ERROR in Line %d | Incompatible Data Types used in expression\n", node->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }

            else
            {
                // Error message:
                char errormsg[70];
                sprintf(errormsg, "ERROR in Line %d | Incompatible Data Types used\n", node->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }

        else if (node->dataForPrint->u.term == DIV)
        {
            type_pair left_component = getTypeExpression(node->children[0], l);
            type_pair right_component = getTypeExpression(node->children[1], l);
            if (left_component->type_name == ERROR_TYPE)
            {
                return left_component;
            }
            if (right_component->type_name == ERROR_TYPE)
            {
                return right_component;
            }
            if (left_component->type_name == right_component->type_name && left_component->type_name == PRIMITIVE_TYPE)
            {
                if (left_component->type_name == PRIMITIVE_TYPE)
                {
                    if ((left_component->type_expr.data_type == INTEGER || left_component->type_expr.data_type == REAL) && (right_component->type_expr.data_type == INTEGER || right_component->type_expr.data_type == REAL))
                    {
                        type_return->type_name = PRIMITIVE_TYPE;
                        type_return->type_expr.data_type = REAL;
                    }

                    else
                    {
                        // Error message:
                        char errormsg[78];
                        sprintf(errormsg, "ERROR in Line %d | Incompatible Data Types used in Division operation\n", node->dataForPrint->lineNo);
                        type_return = createErrorType(errormsg);
                        // displayErrorType(type_return);
                        return type_return;
                    }
                }
                else
                {
                    // Error message:
                    char errormsg[78];
                    sprintf(errormsg, "ERROR in Line %d | Incompatible Data Types used in Division operation\n", node->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }

            else
            {
                // Error message:
                char errormsg[78];
                sprintf(errormsg, "ERROR in Line %d | Incompatible Data Types used in Division operation\n", node->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }

        else if (node->dataForPrint->u.term == LT || node->dataForPrint->u.term == LE || node->dataForPrint->u.term == EQ || node->dataForPrint->u.term == NE || node->dataForPrint->u.term == GT || node->dataForPrint->u.term == GE)
        {
            type_pair left_component = getTypeExpression(node->children[0], l);
            type_pair right_component = getTypeExpression(node->children[1], l);
            if (left_component->type_name == ERROR_TYPE)
            {
                return left_component;
            }
            if (right_component->type_name == ERROR_TYPE)
            {
                return right_component;
            }
            if (left_component->type_name == right_component->type_name && (left_component->type_expr.data_type == INTEGER || left_component->type_expr.data_type == REAL))
            {
                type_return->type_name = PRIMITIVE_TYPE;
                type_return->type_expr.data_type = BOOLEAN;
            }

            else
            {
                // Error message:
                char errormsg[70];
                sprintf(errormsg, "ERROR in Line %d | Incompatible Data Types used in expression\n", node->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }
        else if (node->dataForPrint->u.term == AND || node->dataForPrint->u.term == OR)
        {
            type_pair left_component = getTypeExpression(node->children[0], l);
            type_pair right_component = getTypeExpression(node->children[1], l);
            if (left_component->type_name == ERROR_TYPE)
            {
                return left_component;
            }
            if (right_component->type_name == ERROR_TYPE)
            {
                return right_component;
            }
            if (left_component->type_name == right_component->type_name && (left_component->type_expr.data_type == BOOLEAN) && (right_component->type_expr.data_type == BOOLEAN))
            {
                type_return->type_name = PRIMITIVE_TYPE;
                type_return->type_expr.data_type = BOOLEAN;
            }

            else
            {
                // Error message:
                char errormsg[70];
                sprintf(errormsg, "ERROR in Line %d | Incompatible Data Types used in expression\n", node->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }
        break;
    }
    }
    return type_return;
}

type_pair loopCounterAssignChecker(ASTNode node, char *loop_counter_variable)
{
    /*
        This is a recursive helper function that will check whether the loop counter variable (for which this was
        called) has been assigned a value in any assignment statement within this block of statements, or in any
        of the sub-blocks within this block (recursively). If it has been assigned a value, it will return an
        error_type. Otherwise it returns NULL.
    */

    type_pair type_return;
    ASTListNode statement = node->children[node->num_children - 1]->info_list->head;
    int is_outside = 1;

    while (statement != NULL)
    {
        if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "declareStmt") == 0 && is_outside == 0)
        {
            ASTListNode id_in_list = statement->data->children[0]->info_list->head;
            while (id_in_list != NULL)
            {
                if (strcmp(id_in_list->data->dataForPrint->lexeme, loop_counter_variable) == 0)
                {
                    break;
                }
                id_in_list = id_in_list->next;
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "declareStmt") == 0 && is_outside == 1)
        {
            ASTListNode id_in_list = statement->data->children[0]->info_list->head;
            while (id_in_list != NULL)
            {
                if (strcmp(id_in_list->data->dataForPrint->lexeme, loop_counter_variable) == 0)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in line %d | Loop counter variable %s cannot be re-declared inside the loop.\n", id_in_list->data->dataForPrint->lineNo, loop_counter_variable);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
                id_in_list = id_in_list->next;
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "ASSIGNOP") == 0)
        {
            if (statement->data->children[0]->num_children == 0)
            {
                if (strcmp(statement->data->children[0]->dataForPrint->lexeme, loop_counter_variable) == 0)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Loop counter variable %s cannot be assigned a value within the loop\n", statement->data->children[0]->dataForPrint->lineNo, loop_counter_variable);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "GET_VALUE") == 0)
        {
            if (strcmp(statement->data->children[0]->dataForPrint->lexeme, loop_counter_variable) == 0)
            {
                char errormsg[100];
                sprintf(errormsg, "ERROR in line %d | Loop counter variable %s cannot be assigned a value within the loop.\n", statement->data->children[0]->dataForPrint->lineNo, loop_counter_variable);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "moduleReuseStmt") == 0)
        {
            ASTListNode id_in_list = statement->data->children[0]->info_list->head;
            while (id_in_list != NULL)
            {
                if (strcmp(id_in_list->data->dataForPrint->lexeme, loop_counter_variable) == 0)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in line %d | Loop counter variable %s cannot be assigned a value within the loop.\n", id_in_list->data->dataForPrint->lineNo, loop_counter_variable);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
                id_in_list = id_in_list->next;
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "FOR") == 0)
        {
            type_return = loopCounterAssignChecker(statement->data, loop_counter_variable);
            if (type_return->type_name == ERROR_TYPE)
                return type_return;
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "WHILE") == 0)
        {
            type_return = loopCounterAssignChecker(statement->data, loop_counter_variable);
            if (type_return->type_name == ERROR_TYPE)
                return type_return;
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "condionalStmt") == 0)
        {
            ASTListNode case_statement = statement->data->children[1]->info_list->head;
            while (case_statement != NULL)
            {
                type_return = loopCounterAssignChecker(case_statement->data, loop_counter_variable);
                if (type_return->type_name == ERROR_TYPE)
                    return type_return;
                case_statement = case_statement->next;
            }
        }

        statement = statement->next;
    }
    type_return = (type_pair)malloc(sizeof(struct type_pair));
    type_return->type_name = SCOPE_POINTER;
    type_return->type_expr.data_type = FOR;
    return type_return;
}

type_pair MDChecker(ASTNode node, char *MD_variable)
{
    /*
        This is a recursive helper function that will check whether module declaration output parameter (for which this was
        called) has been assigned a value in any assignment statement within this block of statements, or in any
        of the sub-blocks within this block (recursively). If it has been assigned a value, it will return
        positively. Otherwise it returns error_type. If it is redeclared inside the module, it will return error_type and respective line number.
    */

    int flag_declared = 0;
    int flag_error = 0;
    type_pair type_return = (type_pair)malloc(sizeof(struct type_pair));
    if (node->children[node->num_children - 1] == NULL || node->children[node->num_children - 1]->info_list == NULL)
    {
        type_return->type_name = ERROR_TYPE;
        type_return->type_expr.data_type = FALSE;
        return type_return;
    }
    ASTListNode statement = node->children[node->num_children - 1]->info_list->head;

    if (statement == NULL)
    {
        type_return->type_name = ERROR_TYPE;
        type_return->type_expr.data_type = FALSE;
        return type_return;
    }

    while (statement != NULL)
    {
        if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "declareStmt") == 0)
        {
            ASTListNode id_in_list = statement->data->children[0]->info_list->head;
            while (id_in_list != NULL)
            {
                if (strcmp(id_in_list->data->dataForPrint->lexeme, MD_variable) == 0) // and flag!=1
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in line %d | Output parameter %s cannot be re-declared inside the function.\n", id_in_list->data->dataForPrint->lineNo, MD_variable);
                    flag_error = 1;
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    //  return type_return;
                } // else flag=2 if declared inside loop
                id_in_list = id_in_list->next;
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "GET_VALUE") == 0)
        {
            if (strcmp(statement->data->children[0]->dataForPrint->lexeme, MD_variable) == 0) // and flag!=2
            {
                type_return->type_name = FUNCTION_HASH_POINTER;
                type_return->type_expr.data_type = MODULE;
                // return type_return;
                flag_declared = 1;
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "moduleReuseStmt") == 0 && statement->data->children[0] != NULL)
        {

            ASTListNode id_in_list = statement->data->children[0]->info_list->head;
            while (id_in_list != NULL)
            {
                if (strcmp(id_in_list->data->dataForPrint->lexeme, MD_variable) == 0) // and flag!=2
                {
                    type_return->type_name = FUNCTION_HASH_POINTER;
                    type_return->type_expr.data_type = MODULE;
                    // return type_return;
                    flag_declared = 1;
                }
                id_in_list = id_in_list->next;
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "ASSIGNOP") == 0)
        {
            if (strcmp(statement->data->children[0]->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
            {
                ;
            }
            else if (strcmp(statement->data->children[0]->dataForPrint->lexeme, MD_variable) == 0) // and flag!=2
            {
                type_return->type_name = FUNCTION_HASH_POINTER;
                type_return->type_expr.data_type = MODULE;
                // return type_return;
                flag_declared = 1;
            }
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "FOR") == 0)
        {
            // Flag 1
            type_return = MDChecker(statement->data, MD_variable);
            // Flag 0
            if (type_return->type_name == ERROR_TYPE)
            {
                ;
            }
            else if (type_return->type_name == FUNCTION_HASH_POINTER)
                flag_declared = 1;
        }
        else if (strcmp(statement->data->dataForPrint->currentNodeSymbol, "WHILE") == 0)
        {
            type_return = MDChecker(statement->data, MD_variable);
            if (type_return->type_name == ERROR_TYPE)
                ;
            else if (type_return->type_name == FUNCTION_HASH_POINTER)
                flag_declared = 1;
        }

        statement = statement->next;
    }

    if (flag_declared == 0)
    {
        type_return->type_name = ERROR_TYPE;
        type_return->type_expr.data_type = FALSE;
        flag_error = 0;
        return type_return;
    }
    else if (flag_declared == 1)
    {
        if (flag_error == 1)
        {
            type_return->type_name = ERROR_TYPE;
            type_return->type_expr.data_type = TRUE;
            flag_error = 0;
            flag_declared = 0;
            return type_return;
        }
        else
        {
            flag_declared = 0;
            type_return->type_name = FUNCTION_HASH_POINTER;
            type_return->type_expr.data_type = MODULE;
            return type_return;
        }
    }
    type_return->type_name = ERROR_TYPE;
    type_return->type_expr.data_type = FALSE;
    return type_return;
}

int getOffsetArrayElement(char *array_name, int index, hash_table_holder root)
{
    int offset = -1;

    hash_component t = searchScopes(root, array_name);
    if (t == NULL)
    {
        // error, array not found
        return offset;
    }
    int base_offset = t->offset;
    int type = t->content.array_info->data_type_e;
    int lower_bound = t->content.array_info->range.static_range[0];

    switch (type)
    {
    case INTEGER:
        offset = base_offset + abs(index - lower_bound) * 4;
        break;
    case REAL:
        offset = base_offset + abs(index - lower_bound) * 8;
        break;
    case BOOLEAN:
        offset = base_offset + abs(index - lower_bound);
        break;
    }
    return offset;
}

void displayModuleCallPairs(char **moduleCallPairs, int size)
{
    int i;
    for (i = 0; i < size; i = i + 2)
    {
        printf("%s -> %s\n", moduleCallPairs[i], moduleCallPairs[i + 1]);
    }
    return;
}

type_pair semanticCheck(ASTNode node, type_check_state state, hash_table_holder root)
{
    hash_table_holder temp;
    type_pair type_return;
    type_return = (type_pair)malloc(sizeof(struct type_pair));
    hash_component t;
    switch (state)
    {
    case CONDITIONAL_CHECK:
    {
        temp = root;
        t = searchScopes(temp, node->children[0]->dataForPrint->lexeme);
        if (t == NULL)
        {
            // error, switch ID not found, return error token
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | Switch ID not declared previously\n", node->children[0]->dataForPrint->lineNo);
            type_return = createErrorType(errormsg);
            switch_error_flag = 1;
            // displayErrorType(type_return);
            return type_return;
        }
        else if (t->info == PRIMITIVE_TYPE && (t->content.type == NUM || t->content.type == INTEGER))
        {
            if (node->children[2] == NULL)
            {
                // error, default necessary for num control in switch
                char errormsg[150];
                sprintf(errormsg, "ERROR in Line %d | Line Range: [%d,%d] | Default must be present while using int as switch control variable\n", node->range_ast[1], node->range_ast[0], node->range_ast[1]);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
            ASTListNode cstmts = node->children[1]->info_list->head;
            // int flag_switch_error = 0;
            while (cstmts != NULL)
            {
                if (cstmts->data->children[0]->dataForPrint->u.term != NUM)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Case value must be of type NUM\n", cstmts->data->children[0]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                }
                cstmts = cstmts->next;
            }

            // check for uniqueness??
        }
        else if (t->info == PRIMITIVE_TYPE && t->content.type == BOOLEAN)
        {
            if (node->children[2] != NULL)
            {
                // error, default must not be present for boolean
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | Line Range [%d,%d] | Default not allowed for boolean switch control\n", node->children[2]->dataForPrint->lineNo, node->range_ast[0], node->range_ast[1]);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
            ASTListNode cstmts = node->children[1]->info_list->head;
            int true_flag = 0, false_flag = 0;
            while (cstmts != NULL)
            {
                if (cstmts->data->children[0]->dataForPrint->u.term == TRUE)
                {
                    if (true_flag == 0)
                    {
                        true_flag = 1;
                    }
                    else
                    {
                        char errormsg[100];
                        sprintf(errormsg, "ERROR in Line %d | True repeated\n", cstmts->data->children[0]->dataForPrint->lineNo);
                        type_return = createErrorType(errormsg);
                        // displayErrorType(type_return);
                    }
                }
                else if (cstmts->data->children[0]->dataForPrint->u.term == FALSE)
                {
                    if (false_flag == 0)
                    {
                        false_flag = 1;
                    }
                    else
                    {
                        char errormsg[100];
                        sprintf(errormsg, "ERROR in Line %d | False repeated\n", cstmts->data->children[0]->dataForPrint->lineNo);
                        type_return = createErrorType(errormsg);
                        // displayErrorType(type_return);
                    }
                }
                else
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Invalid Value for case, only boolean allowed\n", cstmts->data->children[0]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                }
                cstmts = cstmts->next;
            }
            // if (node->children[1]->info_list->size != 2 && node->children[1]->info_list->tail != NULL)
            // {
            //     char errormsg[100];
            //     sprintf(errormsg, "ERROR in Line %d | Only two cases allowed for boolean switch control type, true and false\n", node->children[1]->info_list->tail->data->dataForPrint->lineNo);
            //     type_return = createErrorType(errormsg);
            //     //displayErrorType(type_return);
            // }
            // else if (node->children[1]->info_list->head->data->children[0]->dataForPrint->u.term == TRUE)
            // {
            //     if (node->children[1]->info_list->head->next->data->children[0]->dataForPrint->u.term == FALSE)
            //     {
            //         type_return->type_name = PRIMITIVE_TYPE;
            //         type_return->type_expr.data_type = BOOLEAN;
            //     }
            //     else
            //     {
            //         char errormsg[100];
            //         sprintf(errormsg, "ERROR in Line %d | true and false are the only valid data types, without duplication \n", node->children[1]->info_list->tail->data->dataForPrint->lineNo);
            //         type_return = createErrorType(errormsg);
            //         //displayErrorType(type_return);
            //     }
            // }
            // else if (node->children[1]->info_list->head->data->children[0]->dataForPrint->u.term == FALSE)
            // {
            //     if (node->children[1]->info_list->head->next->data->children[0]->dataForPrint->u.term == TRUE)
            //     {
            //         type_return->type_name = PRIMITIVE_TYPE;
            //         type_return->type_expr.data_type = BOOLEAN;
            //     }
            //     else
            //     {
            //         char errormsg[150];
            //         sprintf(errormsg, "ERROR in Line %d |True and False are only valid types, each occuring only once without default. \n", node->children[1]->info_list->tail->data->dataForPrint->lineNo);
            //         type_return = createErrorType(errormsg);
            //         //displayErrorType(type_return);
            //     }
            // }
            // else
            // {
            //     // there are two children, but they are not true and false, or they are repeated, type/duplication error
            //     char errormsg[100];
            //     sprintf(errormsg, "ERROR in Line %d |True and False are only valid types, each occuring only once without default. \n", node->children[1]->info_list->tail->data->dataForPrint->lineNo);
            //     type_return = createErrorType(errormsg);
            //     //displayErrorType(type_return);
            // }
            // check for casestmts type and uniqueness
        }
        else
        {
            // invalid type of switch ID
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | Switch ID not of valid Type, only int and boolean allowed\n", node->children[0]->dataForPrint->lineNo);
            type_return = createErrorType(errormsg);
            switch_error_flag = 1;
            // displayErrorType(type_return);
        }
        return type_return;
        break;
    }

    case ASSIGN_CHECK:
    {
        hash_component left_val;
        if (node == NULL)
        {
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | null node passed\n", -1);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            return type_return;
        }
        int check_flag1 = 0;
        if (node->children[0]->dataForPrint->u.term == ID)
        {
            left_val = searchScopes(root, node->children[0]->dataForPrint->lexeme);
        }

        else if (strcmp(node->children[0]->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
        {
            type_pair type_return = array_element_expression_check(node->children[0], root);
            if (type_return->type_name == ERROR_TYPE)
                check_flag1 = 1;
            else
                left_val = searchScopes(root, node->children[0]->children[0]->dataForPrint->lexeme);
        }

        else
        {
            // Error message
            char errormsg[70];
            sprintf(errormsg, "ERROR in Line %d | Invalid left hand side\n", node->children[0]->dataForPrint->lineNo);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            // return type_return;
        }

        type_pair lhs = (type_pair)malloc(sizeof(struct type_pair));
        type_pair rhs = getTypeExpression(node->children[1], root);
        if (type_return->type_name == ERROR_TYPE || check_flag1 == 1)
        {
            check_flag1 = 0;
            return type_return;
        }
        if (rhs->type_name == ERROR_TYPE)
        {
            return rhs;
        }
        if (left_val)
        {
            if (strcmp(node->children[0]->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
            {
                lhs->type_name = PRIMITIVE_TYPE;
                lhs->type_expr.data_type = left_val->content.array_info->data_type_e;
            }
            else if (left_val->info == ARRAY_TYPE)
            {
                lhs->type_name = ARRAY_TYPE;
                lhs->type_expr.array_info = left_val->content.array_info;
            }
            else if (node->children[0]->num_children == 0)
            {
                lhs->type_name = PRIMITIVE_TYPE;
                lhs->type_expr.data_type = left_val->content.type;
            }
        }
        else
        {
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | Undeclared variable %s assigned value\n", node->children[0]->dataForPrint->lineNo, node->children[0]->dataForPrint->lexeme);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            return type_return;
        }

        if (lhs->type_name != rhs->type_name)
        {
            // Error message
            char errormsg[150];
            sprintf(errormsg, "ERROR in Line %d | Type mismatch between left and right hand side\n", node->children[0]->dataForPrint->lineNo);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            return type_return;
        }

        else if (lhs->type_name == PRIMITIVE_TYPE)
        {
            if (lhs->type_expr.data_type != rhs->type_expr.data_type)
            {
                // Error message
                char errormsg[70];
                sprintf(errormsg, "ERROR in Line %d | Type mismatch\n", node->children[0]->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }

        else
        {
            if (lhs->type_expr.array_info->is_static == 3 && rhs->type_expr.array_info->is_static == 3)
            {
                if (lhs->type_expr.array_info->range.static_range[0] != rhs->type_expr.array_info->range.static_range[0] || lhs->type_expr.array_info->range.static_range[1] != rhs->type_expr.array_info->range.static_range[1])
                {
                    // Error message
                    char errormsg[70];
                    sprintf(errormsg, "ERROR in Line %d | Unequal arrays equated\n", node->children[0]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
            else if (lhs->type_expr.array_info->is_static == 2 && rhs->type_expr.array_info->is_static == 2)
            {
                if (lhs->type_expr.array_info->range.static_range[1] != rhs->type_expr.array_info->range.static_range[1])
                {
                    // Error message
                    char errormsg[70];
                    sprintf(errormsg, "ERROR in Line %d | Unequal arrays equated\n", node->children[0]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
            else if (lhs->type_expr.array_info->is_static == 1 && rhs->type_expr.array_info->is_static == 1)
            {
                if (lhs->type_expr.array_info->range.static_range[0] != rhs->type_expr.array_info->range.static_range[0])
                {
                    // Error message
                    char errormsg[70];
                    sprintf(errormsg, "ERROR in Line %d | Unequal arrays equated\n", node->children[0]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
        }
        return type_return;
        break;
    }

    case MODULE_REUSE_CHECK:
    {
        char *module_name = node->children[1]->dataForPrint->lexeme;
        hash_component t = searchHashTable(BIG_ROOT->hash_table, module_name);
        if (t != NULL && t->content.hash_table_unit->isInitialised == 0)
        {
            pending_module_checks[freeze_state_size - 1] = malloc(sizeof(struct freeze_state));
            pending_module_checks[freeze_state_size - 1]->curr_root = root;
            pending_module_checks[freeze_state_size - 1]->node = node;

            freeze_state_size++;
            pending_module_checks = realloc(pending_module_checks, sizeof(struct freeze_state *) * freeze_state_size);
            pending_module_checks[freeze_state_size - 1] = NULL;
            type_pair ret_type = malloc(sizeof(struct type_pair));
            ret_type->type_name = FUNCTION_HASH_POINTER;
            return ret_type;
        }
        /*
            - check if module is declared
        */
        if (t == NULL)
        {
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | Module %s not declared\n", node->children[1]->dataForPrint->lineNo, module_name);
            type_pair er = createErrorType(errormsg);
            // displayErrorType(er);
            return er;
        }
        /*
            - Check if module has no output parameters
        */
        hash_table_holder func_unit = t->content.hash_table_unit;
        if (node->children[0] != NULL)
        {
            if (func_unit->op_list == NULL)
            {
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | Module %s has no output parameters\n", node->children[1]->dataForPrint->lineNo, module_name);
                type_pair er = createErrorType(errormsg);
                // displayErrorType(er);
                return er;
            }
        }

        /*
            - Check if module has output parameters
        */
        if (node->children[0] == NULL)
        {
            if (func_unit->op_list != NULL)
            {
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | Module %s has output parameters\n", node->children[1]->dataForPrint->lineNo, module_name);
                type_pair er = createErrorType(errormsg);
                // displayErrorType(er);
                return er;
            }
        }

        /*
            - Check no. of formal output parameters match with module reuse statement
        */
        ASTNode o_list = node->children[0];
        if (o_list != NULL)
        {

            if (o_list->info_list->size != func_unit->op_list->size)
            {
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | No. of return parameters do not match\n", node->children[1]->dataForPrint->lineNo);
                type_pair er = createErrorType(errormsg);
                // displayErrorType(er);
                return er;
            }

            /*
                - Check the type of output parameters match with module reuse statement and their previous declarations
            */
            ASTListNode temp = o_list->info_list->head;
            ip_op_info temp2 = func_unit->op_list->head;

            while (temp != NULL)
            {
                ASTNode o_param = temp->data;
                hash_component instance = searchScopes(root, o_param->dataForPrint->lexeme);
                /* Check if the LHS parameters have been declared before */
                if (instance == NULL)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Variable %s not declared\n", o_param->dataForPrint->lineNo, o_param->dataForPrint->lexeme);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                    // return er;
                }

                /* Check if the LHS parameters type matches formal parameters type */
                //  printf("temp->type->type_name = %d | instance->content.type = %d\n", temp2->type->type_name, instance->content.type);
                if (temp2->type->type_name == OP_PRIMITIVE_TYPE && instance->content.type != temp2->type->type_expr.data_type)
                {

                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Type mismatch in return parameters\n", o_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                    // return er;
                }

                temp = temp->next;
                temp2 = temp2->next;
            }
        }

        /*
            - Check no. of formal input parameters match with module reuse statement
        */
        ASTNode i_list = node->children[2];
        if (i_list->info_list->size != func_unit->ip_list->size)
        {
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | No. of input parameters do not match\n", node->children[1]->dataForPrint->lineNo);
            type_pair er = createErrorType(errormsg);
            // displayErrorType(er);
            return er;
        }

        /*
            - Check the type of input parameters match with module reuse statement and their previous declarations
        */
        ASTListNode temp;
        ip_op_info temp2;

        temp = i_list->info_list->head;
        temp2 = func_unit->ip_list->head;

        while (temp != NULL)
        {
            ASTNode i_param = temp->data;
            /* Case when the whole thing comes with a sign */
            if ((strcmp(i_param->dataForPrint->tokenName, "MINUS") == 0) ||
                (strcmp(i_param->dataForPrint->tokenName, "PLUS") == 0))
            {
                i_param = i_param->children[1];
            }

            /* Case when there's an ID */
            if (strcmp(i_param->dataForPrint->tokenName, "ID") == 0)
            {
                hash_component instance = searchScopes(root, i_param->dataForPrint->lexeme);
                /* Check if the RHS parameters have been declared before */
                if (instance == NULL)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Variable %s not declared\n", i_param->dataForPrint->lineNo, i_param->dataForPrint->lexeme);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                    // return er;
                }

                /* Check if the RHS parameters type matches formal parameters type */

                /* Case if both are Primitive Types */
                if ((temp2->type->type_name == IP_PRIMITIVE_TYPE && instance->info == ARRAY_TYPE) ||
                    (temp2->type->type_name == IP_ARRAY_TYPE && instance->info == PRIMITIVE_TYPE))
                {
                    /* One has ARRAY_TYPE and the other has PRIMITIVE_TYPE */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Expected Type different (ARRAY-PRIMITIVE mismatch) from input parameters\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                }
                else if (temp2->type->type_name == IP_PRIMITIVE_TYPE && instance->info == PRIMITIVE_TYPE)
                {
                    if (instance->content.type != temp2->type->type_expr.data_type)
                    {
                        /* Types don't match */
                        char errormsg[100];
                        sprintf(errormsg, "ERROR in Line %d | Type mismatch in input parameters\n", i_param->dataForPrint->lineNo);
                        type_pair er = createErrorType(errormsg);
                        // displayErrorType(er);
                        temp = temp->next;
                        temp2 = temp2->next;
                        continue;
                        // return er;
                    }
                    else
                    {
                        /*
                            - All good here
                            - Both are PRIMITIVE_TYPE and their types match
                        */
                    }
                }
                /* Case when both are Array Types */
                else if (temp2->type->type_name == IP_ARRAY_TYPE && instance->info == ARRAY_TYPE)
                {
                    if (instance->content.array_info->data_type_e != temp2->type->type_expr.array_info->data_type_e)
                    {
                        /* Types don't match */
                        char errormsg[100];
                        sprintf(errormsg, "ERROR in Line %d | Type mismatch in input parameters\n", i_param->dataForPrint->lineNo);
                        type_pair er = createErrorType(errormsg);
                        // displayErrorType(er);
                        temp = temp->next;
                        temp2 = temp2->next;
                        continue;
                        // return er;
                    }
                    else
                    {
                        /* Case when the actual parameter array is static */
                        if (instance->content.array_info->is_static == 3)
                        {
                            if (instance->content.array_info->range.static_range[0] == temp2->type->type_expr.array_info->range.static_range[0] &&
                                instance->content.array_info->range.static_range[1] == temp2->type->type_expr.array_info->range.static_range[1])
                            {
                                /*
                                    - All good here
                                    - Both are ARRAY_TYPE and their types match
                                    - Their ranges match
                                */
                            }
                            else
                            {
                                /* Types don't match */
                                char errormsg[100];
                                sprintf(errormsg, "ERROR in Line %d | Range mismatch between formal and actual parameters array\n", i_param->dataForPrint->lineNo);
                                type_pair er = createErrorType(errormsg);
                                // displayErrorType(er);
                                temp = temp->next;
                                temp2 = temp2->next;
                                continue;
                                // return er;
                            }
                        }
                        else
                        {
                            /*
                                - Can't do much here if the actual parameter array is dynamic
                            */
                        }
                    }
                }
            }
            /* Case when there's an ARRAY_ELEMENT like A[k] */
            else if (strcmp(i_param->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
            {
                if (temp2->type->type_name != IP_PRIMITIVE_TYPE)
                {
                    /* Formal parameters has ARRAY_TYPE */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Expected ARRAY_TYPE but got PRIMTIVE_TYPE\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                }

                type_pair arr_type_expression = array_element_check(i_param, root);
                if (arr_type_expression->type_name == ERROR_TYPE)
                {
                    // displayErrorType(arr_type_expression);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                }

                if (temp2->type->type_expr.data_type != arr_type_expression->type_expr.array_info->data_type_e)
                {
                    /* Types don't match */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Data Type mismatch in input parameters\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                    // return er;
                }
            }
            /* Case when there's a NUM like 35 */
            else if (strcmp(i_param->dataForPrint->tokenName, "NUM") == 0)
            {
                if (temp2->type->type_name != IP_PRIMITIVE_TYPE)
                {
                    /* Formal parameters has ARRAY_TYPE */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Expected ARRAY_TYPE but got PRIMTIVE_TYPE\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                }

                if (temp2->type->type_expr.data_type != INTEGER)
                {
                    /* Types don't match */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Data Type mismatch in input parameters\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                    // return er;
                }
            }
            /* Case when there's a RNUM like 35.5 */
            else if (strcmp(i_param->dataForPrint->tokenName, "RNUM") == 0)
            {
                if (temp2->type->type_name != IP_PRIMITIVE_TYPE)
                {
                    /* Formal parameters has ARRAY_TYPE */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Expected ARRAY_TYPE but got PRIMTIVE_TYPE\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                }

                if (temp2->type->type_expr.data_type != REAL)
                {
                    /* Types don't match */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Data Type mismatch in input parameters\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                    // return er;
                }
            }
            /* Case when there's a BOOLEAN like TRUE or FALSE */
            else if (i_param->dataForPrint->u.term == FALSE || i_param->dataForPrint->u.term == TRUE)
            {
                if (temp2->type->type_name != IP_PRIMITIVE_TYPE)
                {
                    /* Formal parameters has ARRAY_TYPE */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Expected ARRAY_TYPE but got PRIMTIVE_TYPE\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                }

                if (temp2->type->type_expr.data_type != BOOLEAN)
                {
                    /* Types don't match */
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Data Type mismatch in input parameters\n", i_param->dataForPrint->lineNo);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);
                    temp = temp->next;
                    temp2 = temp2->next;
                    continue;
                    // return er;
                }
            }

            temp = temp->next;
            temp2 = temp2->next;
        }

        /*Check if there is an immediate recursion*/
        hash_table_holder temp1 = root;
        while (temp1 != NULL)
        {
            if (temp1->type == FUNCTION_SYMBOL_TABLE)
            {
                if (strcmp(temp1->self_name, module_name) == 0)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Recursive function call (to %s) is not allowed.\n", node->children[1]->dataForPrint->lineNo, module_name);
                    num_semantic_errors--;
                    type_pair type_return = createErrorType(errormsg);
                    return type_return;
                }
                else
                {
                    break;
                }
            }
            temp1 = temp1->parent;
        }

        type_pair ret_type = malloc(sizeof(struct type_pair));
        ret_type->type_name = FUNCTION_HASH_POINTER;
        return ret_type;

        break;
    }

    case WHILE_EXPRESSION_CHECK:
    {
        ;
        temp = root;
        ASTNode while_expression_node = node->children[0];
        // use assign_check for type checking
        type_pair while_expression_type = getTypeExpression(while_expression_node, root);
        if (while_expression_type->type_expr.data_type != BOOLEAN)
        {
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | the variables in while expression should evaluate to Boolean.\n", while_expression_node->dataForPrint->lineNo);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            return type_return;
        }
        // after type checking, we check for semantic errors
        ip_op_list while_parameters = (ip_op_list)malloc(sizeof(struct ip_op_list));
        while_parameters->size = 0;
        while_parameters->head = NULL;
        while_parameters->tail = NULL;
        create_while_para(while_expression_node, while_parameters);
        ASTListNode temp_stmt_node = node->children[1]->info_list->head;
        int flag_while = 0;
        while (temp_stmt_node != NULL)
        {
            if (temp_stmt_node->data->dataForPrint->u.term == ASSIGNOP)
            {
                ASTNode lhs_variable = temp_stmt_node->data->children[0];
                if (strcmp(lhs_variable->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
                {
                    lhs_variable = lhs_variable->children[0];
                }
                ip_op_info while_para_node = while_parameters->head;
                while (while_para_node != NULL)
                {
                    if (strcmp(while_para_node->identifier, lhs_variable->dataForPrint->lexeme) == 0)
                    {
                        flag_while = 1;
                        break;
                    }
                    while_para_node = while_para_node->next;
                }
            }
            else if (strcmp(temp_stmt_node->data->dataForPrint->currentNodeSymbol, "moduleReuseStmt") == 0)
            {
                // DOUBTCHANGEARRAYELEMENT
                ASTNode lhs_variable = temp_stmt_node->data->children[0];
                if (lhs_variable != NULL)
                {
                    ASTListNode idlist_para_node = lhs_variable->info_list->head;
                    while (idlist_para_node)
                    {
                        lhs_variable = idlist_para_node->data;
                        if (strcmp(lhs_variable->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
                        {
                            lhs_variable = lhs_variable->children[0];
                        }
                        ip_op_info while_para_node = while_parameters->head;
                        while (while_para_node != NULL)
                        {
                            if (strcmp(while_para_node->identifier, lhs_variable->dataForPrint->lexeme) == 0)
                            {
                                flag_while = 1;
                                break;
                            }
                            while_para_node = while_para_node->next;
                        }

                        if (flag_while == 1)
                        {
                            break;
                        }
                        idlist_para_node = idlist_para_node->next;
                    }
                }
            }
            else if (strcmp(temp_stmt_node->data->dataForPrint->currentNodeSymbol, "GET_VALUE") == 0)
            {
                ASTNode lhs_variable = temp_stmt_node->data->children[0];
                if (strcmp(lhs_variable->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
                {
                    lhs_variable = lhs_variable->children[0];
                }
                ip_op_info while_para_node = while_parameters->head;
                while (while_para_node != NULL)
                {
                    if (strcmp(while_para_node->identifier, lhs_variable->dataForPrint->lexeme) == 0)
                    {
                        flag_while = 1;
                        break;
                    }
                    while_para_node = while_para_node->next;
                }
            }
            if (flag_while == 1)
            {
                break;
            }
            temp_stmt_node = temp_stmt_node->next;
        }
        if (flag_while == 1)
        {
            flag_while = 0;
            type_return->type_name = SCOPE_POINTER;
            type_return->type_expr.data_type = WHILE;
        }
        else
        {
            char errormsg[150];
            sprintf(errormsg, "ERROR in Line %d | Line range: [%d,%d] |the variables in while expressions are not assigned a value inside while function.\n", node->range_ast[1], node->range_ast[0], node->range_ast[1]);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            return type_return;
        }
        return type_return;
        break;
    }

    case FOR_CHECK:
    {
        if ((node->children[1]->children[0]->dataForPrint->t == TERMINAL && node->children[1]->children[0]->dataForPrint->u.term == REAL) || (node->children[1]->children[1]->dataForPrint->t == TERMINAL && node->children[1]->children[1]->dataForPrint->u.term == REAL))
        {
            char errormsg[100];
            sprintf(errormsg, "ERROR in Line %d | range in a for loop cannot comprise of floats.\n", node->children[1]->children[1]->dataForPrint->lineNo);
            type_return = createErrorType(errormsg);
            // displayErrorType(type_return);
            return type_return;
        }
        if ((node->children[1]->children[0]->dataForPrint->t == TERMINAL && node->children[1]->children[0]->dataForPrint->u.term == ID))
        {
            hash_component id_type = searchHashTable(root->hash_table, node->children[1]->children[0]->dataForPrint->lexeme);
            if (id_type->info != PRIMITIVE_TYPE || (id_type->info == PRIMITIVE_TYPE && id_type->content.type != INTEGER))
            {
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | range in a for loop cannot comprise of identifiers that are floats.\n", node->children[1]->children[0]->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }
        if ((node->children[1]->children[1]->dataForPrint->t == TERMINAL && node->children[1]->children[1]->dataForPrint->u.term == ID))
        {
            hash_component id_type = searchHashTable(root->hash_table, node->children[1]->children[1]->dataForPrint->lexeme);
            if (id_type->info != PRIMITIVE_TYPE || (id_type->info == PRIMITIVE_TYPE && id_type->content.type != INTEGER))
            {
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | range in a for loop cannot comprise of identifiers that are floats.\n", node->children[1]->children[1]->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
        }
        if (strcmp(node->children[1]->children[0]->dataForPrint->lexeme, "MINUS") == 0)
        {
            if (node->children[1]->children[0]->children[1]->dataForPrint->t == TERMINAL && node->children[1]->children[0]->children[1]->dataForPrint->u.term == REAL)
            {
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | range in a for loop cannot comprise of floats.\n", node->children[1]->children[0]->children[1]->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
            if ((node->children[1]->children[0]->children[1]->dataForPrint->t == TERMINAL && node->children[1]->children[0]->children[1]->dataForPrint->u.term == ID))
            {
                hash_component id_type = searchHashTable(root->hash_table, node->children[1]->children[0]->children[1]->dataForPrint->lexeme);
                if (id_type->info != PRIMITIVE_TYPE || (id_type->info == PRIMITIVE_TYPE && id_type->content.type != INTEGER))
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | range in a for loop cannot comprise of identifiers that are floats.\n", node->children[1]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
        }
        if (strcmp(node->children[1]->children[1]->dataForPrint->lexeme, "MINUS") == 0)
        {
            if (node->children[1]->children[1]->children[1]->dataForPrint->t == TERMINAL && node->children[1]->children[1]->children[1]->dataForPrint->u.term == REAL)
            {
                char errormsg[100];
                sprintf(errormsg, "ERROR in Line %d | range in a for loop cannot comprise of floats.\n", node->children[1]->dataForPrint->lineNo);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
                return type_return;
            }
            if ((node->children[1]->children[1]->children[1]->dataForPrint->t == TERMINAL && node->children[1]->children[1]->children[1]->dataForPrint->u.term == ID))
            {
                hash_component id_type = searchHashTable(root->hash_table, node->children[1]->children[1]->children[1]->dataForPrint->lexeme);
                if (id_type->info != PRIMITIVE_TYPE || (id_type->info == PRIMITIVE_TYPE && id_type->content.type != INTEGER))
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | range in a for loop cannot comprise of identifiers that are floats.\n", node->children[1]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
            }
        }
        char *loop_counter_variable;
        loop_counter_variable = malloc(sizeof(char) * (strlen(node->children[0]->dataForPrint->lexeme) + 1));
        strcpy(loop_counter_variable, node->children[0]->dataForPrint->lexeme);
        type_return = loopCounterAssignChecker(node, loop_counter_variable);
        return type_return;
        break;
    }

    case MODULE_DEFINITION_CHECK:
    {
        /*
            Grammar:
            module : DEF MODULE ID ENDDEF TAKES INPUT SQBO input_plist SQBC SEMICOL ret moduleDef
            ret : RETURNS SQBO output_plist SQBC SEMICOL

            AST:
            module.addr = CREATE_NODE("MODULE", &ID, input_plist.head, ret.addr, moduleDef.addr)

                MODULE-------------------------------
                /     \				\		  		  \
            ID    INPUT_PLIST  O_PARAM-->O_PARAM   s1-->s2-->s3-->s4
                                /   \
                                ID   REAL

            Task:
            I have to ensure that the output parameters (O_PARAM) are all assigned a value inside the module.
            For this, I have to only check left side of assingment statements, as long as we find one assigment statement for all the output parameters, we are good to go.

            1. Check for get_value()
            2. Check for modulereuse statement
            3. Check for assignment statements
            4. Check even inside for/while etc statements, if the variable is assigned a value (without getting redeclared) then we would accept.
            If the variable is redeclared, then assigned (and there is no other instance of this variable) then we would be giving an error.

            Approach:

            Method 1: Take one ID and check if it is present in the left side of any assignment statement inside the module.
                    Repeat this for all the output parameters.
            Method 2: Consider all ID at once and check if they are present in the left side of any assignment statement inside the module.
                    Traverse one_by_one and check if all the output parameters are present in the left side of any assignment statement inside the module after comparing
                    each of them individually.

            Doubts:
            1. When should we call this function

        */

        ASTNode IDList = node->children[2];

        if (IDList == NULL)
        {
            type_return->type_name = FUNCTION_HASH_POINTER;
            type_return->type_expr.data_type = MODULE;
            return type_return;
        }
        else
        {
            ASTList id_list = IDList->info_list;
            ASTListNode t = id_list->head;
            int flag_error = 0;
            while (t != NULL)
            {
                ASTNode x = t->data;
                char *MD_variable;
                MD_variable = malloc(sizeof(char) * (strlen(x->children[0]->dataForPrint->lexeme) + 1));
                strcpy(MD_variable, x->children[0]->dataForPrint->lexeme);
                type_return = MDChecker(node, MD_variable);

                if (type_return->type_name == ERROR_TYPE && type_return->type_expr.data_type != TRUE)
                {
                    // DOUBTCHANGE
                    char errormsg[150];
                    sprintf(errormsg, "ERROR in Line %d | Line Range: [%d,%d] |Output parameter %s not assigned a value inside function.\n", node->range_ast[1], node->range_ast[0], node->range_ast[1], MD_variable);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    // return type_return;
                    flag_error = 1;
                }

                t = t->next;
            }
            if (flag_error == 1)
            {
                flag_error = 0;
                type_return->type_name = ERROR_TYPE;
                return type_return;
            }
        }
        type_return->type_name = FUNCTION_HASH_POINTER;
        type_return->type_expr.data_type = MODULE;
        return type_return;
        break;
    }

    case PRINT_VALUE_CHECK:;
        {
            temp = root;
            if (strcmp(node->children[0]->dataForPrint->tokenName, "ARRAY_ELEMENT") == 0)
            {
                // handle array element print
                type_return = array_element_check(node->children[0], root);
                if (type_return->type_name == ERROR_TYPE)
                {
                    return type_return;
                }
            }
            else if (strcmp(node->children[0]->dataForPrint->tokenName, "ID") == 0)
            {
                t = searchScopes(temp, node->children[0]->dataForPrint->lexeme);
                if (t == NULL)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | in print(x), ID is not declared previously.\n", node->children[0]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
                type_return->type_name = PRIMITIVE_TYPE;
                type_return->type_expr.data_type = node->children[0]->dataForPrint->u.term;
            }

            else
            {
                t = searchScopes(temp, node->children[0]->dataForPrint->lexeme);
                if (t == NULL)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | in print(x), ID not declared previously.\n", node->children[0]->dataForPrint->lineNo);
                    type_return = createErrorType(errormsg);
                    // displayErrorType(type_return);
                    return type_return;
                }
                else if (t->info == PRIMITIVE_TYPE)
                {
                    type_return->type_name = PRIMITIVE_TYPE;
                    type_return->type_expr.data_type = node->children[0]->dataForPrint->u.term;
                }
                else if (t->info == ARRAY_TYPE)
                {
                    type_return->type_name = ARRAY_TYPE;
                    type_return->type_expr.array_info = t->content.array_info;
                }
            }
            // handle print(A) where A is array
            return type_return;
            break;
        }

    case GET_VALUE_CHECK:;
        {
            temp = root;
            t = searchScopes(temp, node->children[0]->dataForPrint->lexeme);
            if (t == NULL)
            {
                char errormsg[120];
                sprintf(errormsg, "ERROR in Line %d | in get_value(), ID %s is not declared previously.\n", node->children[0]->dataForPrint->lineNo, node->children[0]->dataForPrint->lexeme);
                type_return = createErrorType(errormsg);
                // displayErrorType(type_return);
            }
            // else if (t->info != PRIMITIVE_TYPE)
            // {
            //     char errormsg[100];
            //     sprintf(errormsg, "ERROR in Line %d | get_value() allows only primitive data types.\n", node->children[0]->dataForPrint->lineNo);
            //     type_return = createErrorType(errormsg);
            //     return type_return;
            // }
            else
            {
                type_return->type_name = PRIMITIVE_TYPE;
                type_return->type_expr.data_type = node->children[0]->dataForPrint->u.term;
            }
            return type_return;
            break;
        }
    }
    return NULL;
}

void callSemanticCheck(ASTNode node, type_check_state state, hash_table_holder root)
{
    /*
        Call the semantic check function (recursive) and print the appropriate message
    */
    type_pair res = semanticCheck(node, state, root);
    if (res == NULL)
    {
        printf("Semantic Error\n");
        return;
    }
    // else if (res->type_name == ERROR_TYPE)
    // {
    //     printf("%s\n", res->type_expr.error_message);
    //     return;
    // }
    return;
}

void populateSymbolTable(hash_table_holder root, ASTNode node, AST_State state)
{
    /*
        - Initially, at first call, the root will be the global function table, and the node will be the root node of the AST
        - The state will be ROOT_STATE
    */
    if (node == NULL)
        return;
    switch (state)
    {
    case ROOT_STATE:
    {
        /*
            - The root node will have 4 children
        */
        ASTNode curr = node->children[0];
        ASTList list;
        if (curr != NULL)
        {
            list = curr->info_list;
            ASTListNode t = list->head;

            /*
                - Going to traverse the list of modules and insert them into the global function table
                - For each module, we will create a new hash_table_holder which will be value for the key of the module name
                - The value will be the symbol table of the module
                - Offset is intialized to -1 as the modules do not have any offset
            */

            while (t != NULL)
            {
                ASTNode x = t->data;

                hash_component res = searchHashTable(root->hash_table, x->dataForPrint->lexeme);
                if (res != NULL)
                {
                    char errormsg[100];
                    sprintf(errormsg, "ERROR in Line %d | Module %s has been declared previously\n", x->dataForPrint->lineNo, x->dataForPrint->lexeme);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);

                    t = t->next;
                    continue;
                }

                hash_table_holder temp = createHashTableHolder(IP_OP_SCOPE);
                hash_table_holder temp_child = createHashTableHolder(FUNCTION_SYMBOL_TABLE);
                strcpy(temp->self_name, x->dataForPrint->lexeme);
                strcpy(temp_child->self_name, x->dataForPrint->lexeme);
                temp->UID = UID_COUNTER; // UID is the unique identifier for each module
                temp_child->UID = UID_COUNTER++;
                temp->nesting_level = 0;
                temp_child->nesting_level = 1;
                curr_size += 2;                                                   // Increases by 2 as we store in pair-wise manner
                module_calls = realloc(module_calls, sizeof(char *) * curr_size); // Reallocating the memory for the module_calls array
                for (int i = curr_size - 2; i < curr_size; i++)
                {
                    module_calls[i] = NULL;
                }
                module_names = realloc(module_names, sizeof(char *) * (curr_size / 2));                       // Reallocating the memory for the module_names array
                module_names[UID_COUNTER - 1] = malloc(sizeof(char) * (strlen(x->dataForPrint->lexeme) + 1)); // Allocating memory for the module name
                strcpy(module_names[UID_COUNTER - 1], x->dataForPrint->lexeme);                               // Storing the module name in the index corresponding to the UID

                temp_child->parent = temp;
                temp->children = realloc(temp->children, sizeof(hash_table_holder) * (++temp->num_children));
                temp->children[temp->num_children - 1] = temp_child;

                node_content content;
                content.hash_table_unit = temp;
                insertIntoHashTable(root->hash_table, x->dataForPrint->lexeme, -1, FUNCTION_HASH_POINTER, content);

                t = t->next;
            }
        }

        curr = node->children[1];
        if (curr != NULL)
        {
            list = curr->info_list;
            ASTListNode t = list->head;
            // printf("1\n");

            /*
                - Going to traverse the list of modules (with well defined bodies)
                - If they are already present in the global function table, we will just populate the symbol table of the module
                - Initialise the offset of the module to the GLOBAL_COUNTER
                - Recusrively call the populateSymbolTable function with the module's symbol table
            */

            while (t != NULL)
            {
                // printf("2\n");
                ASTNode x = t->data;
                hash_component res = searchHashTable(root->hash_table, x->children[0]->dataForPrint->lexeme);
                if (res != NULL && res->content.hash_table_unit->isInitialised == 1)
                {
                    char errormsg[150];
                    sprintf(errormsg, "ERROR in Line %d | Module %s has been declared previously |\nFunction overloading not allowed | First instance of this module considered\n", x->children[0]->dataForPrint->lineNo, x->children[0]->dataForPrint->lexeme);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);

                    t = t->next;
                    continue;
                }

                if (res == NULL)
                {
                    // printf("3\n");
                    hash_table_holder temp = createHashTableHolder(IP_OP_SCOPE);
                    hash_table_holder temp_child = createHashTableHolder(FUNCTION_SYMBOL_TABLE);
                    strcpy(temp->self_name, x->children[0]->dataForPrint->lexeme);
                    strcpy(temp_child->self_name, x->children[0]->dataForPrint->lexeme);

                    temp->line_range[0] = x->range_ast[0];
                    temp_child->line_range[0] = x->range_ast[0];
                    temp->line_range[1] = x->range_ast[1];
                    temp_child->line_range[1] = x->range_ast[1];

                    temp->isInitialised = 1;
                    temp_child->isInitialised = 1;
                    temp->global_offset = GLOBAL_COUNTER;

                    temp->UID = UID_COUNTER; // UID is the unique identifier for each module
                    temp_child->UID = UID_COUNTER++;

                    temp->nesting_level = 0;
                    temp_child->nesting_level = 1;

                    curr_size += 2;                                                   // Increases by 2 as we store in pair-wise manner
                    module_calls = realloc(module_calls, sizeof(char *) * curr_size); // Reallocating the memory for the module_calls array
                    for (int i = curr_size - 2; i < curr_size; i++)
                    {
                        module_calls[i] = NULL;
                    }
                    module_names = realloc(module_names, sizeof(char *) * (curr_size / 2));                                    // Reallocating the memory for the module_names array
                    module_names[UID_COUNTER - 1] = malloc(sizeof(char) * (1 + strlen(x->children[0]->dataForPrint->lexeme))); // Allocating memory for the module name
                    strcpy(module_names[UID_COUNTER - 1], x->children[0]->dataForPrint->lexeme);                               // Storing the module name in the index corresponding to the UID

                    temp_child->parent = temp;
                    temp->children = realloc(temp->children, sizeof(hash_table_holder) * (++temp->num_children));
                    temp->children[temp->num_children - 1] = temp_child;

                    node_content content;
                    content.hash_table_unit = temp;
                    insertIntoHashTable(root->hash_table, x->children[0]->dataForPrint->lexeme, -1, FUNCTION_HASH_POINTER, content);
                    // strcpy(temp->self_name, x->children[0]->dataForPrint->lexeme);
                    populateSymbolTable(temp, x, FUNCTION_STATE);
                    // printf("4\n");
                }
                else if (res != NULL && res->content.hash_table_unit->isInitialised == 0)
                {
                    res->content.hash_table_unit->isInitialised = 1;
                    res->content.hash_table_unit->children[0]->isInitialised = 1;

                    res->content.hash_table_unit->line_range[0] = x->range_ast[0];
                    res->content.hash_table_unit->children[0]->line_range[0] = x->range_ast[0];

                    res->content.hash_table_unit->line_range[1] = x->range_ast[1];
                    res->content.hash_table_unit->children[0]->line_range[1] = x->range_ast[1];
                    res->content.hash_table_unit->global_offset = GLOBAL_COUNTER;
                    populateSymbolTable(res->content.hash_table_unit, x, FUNCTION_STATE);
                }
                t = t->next;
            }
        }

        curr = node->children[3];
        if (curr != NULL)
        {
            list = curr->info_list;
            ASTListNode t = list->head;
            // printf("1\n");

            /*
                - Going to traverse the list of modules (with well defined bodies)
                - If they are already present in the global function table, we will just populate the symbol table of the module
                - Initialise the offset of the module to the GLOBAL_COUNTER
                - Recusrively call the populateSymbolTable function with the module's symbol table
            */
            while (t != NULL)
            {
                // printf("2\n");
                ASTNode x = t->data;
                hash_component res = searchHashTable(root->hash_table, x->children[0]->dataForPrint->lexeme);
                if (res != NULL && res->content.hash_table_unit->isInitialised == 1)
                {
                    char errormsg[150];
                    sprintf(errormsg, "ERROR in Line %d | Module %s has been declared previously |\nFunction overloading not allowed | First instance of this module considered\n", x->children[0]->dataForPrint->lineNo, x->children[0]->dataForPrint->lexeme);
                    type_pair er = createErrorType(errormsg);
                    // displayErrorType(er);

                    t = t->next;
                    continue;
                }

                if (res == NULL)
                {
                    // printf("3\n");
                    hash_table_holder temp = createHashTableHolder(IP_OP_SCOPE);
                    hash_table_holder temp_child = createHashTableHolder(FUNCTION_SYMBOL_TABLE);
                    strcpy(temp->self_name, x->children[0]->dataForPrint->lexeme);
                    strcpy(temp_child->self_name, x->children[0]->dataForPrint->lexeme);

                    temp->line_range[0] = x->range_ast[0];
                    temp_child->line_range[0] = x->range_ast[0];
                    temp->line_range[1] = x->range_ast[1];
                    temp_child->line_range[1] = x->range_ast[1];

                    temp->isInitialised = 1;
                    temp_child->isInitialised = 1;
                    temp->global_offset = GLOBAL_COUNTER;

                    temp->UID = UID_COUNTER; // UID is the unique identifier for each module
                    temp_child->UID = UID_COUNTER++;

                    temp->nesting_level = 0;
                    temp_child->nesting_level = 1;

                    curr_size += 2;                                                   // Increases by 2 as we store in pair-wise manner
                    module_calls = realloc(module_calls, sizeof(char *) * curr_size); // Reallocating the memory for the module_calls array
                    for (int i = curr_size - 2; i < curr_size; i++)
                    {
                        module_calls[i] = NULL;
                    }
                    module_names = realloc(module_names, sizeof(char *) * (curr_size / 2));                                    // Reallocating the memory for the module_names array
                    module_names[UID_COUNTER - 1] = malloc(sizeof(char) * (1 + strlen(x->children[0]->dataForPrint->lexeme))); // Allocating memory for the module name
                    strcpy(module_names[UID_COUNTER - 1], x->children[0]->dataForPrint->lexeme);                               // Storing the module name in the index corresponding to the UID

                    temp_child->parent = temp;
                    temp->children = realloc(temp->children, sizeof(hash_table_holder) * (++temp->num_children));
                    temp->children[temp->num_children - 1] = temp_child;

                    node_content content;
                    content.hash_table_unit = temp;
                    insertIntoHashTable(root->hash_table, x->children[0]->dataForPrint->lexeme, -1, FUNCTION_HASH_POINTER, content);
                    // strcpy(temp->self_name, x->children[0]->dataForPrint->lexeme);
                    populateSymbolTable(temp, x, FUNCTION_STATE);
                    // printf("4\n");
                }
                else if (res != NULL && res->content.hash_table_unit->isInitialised == 0)
                {
                    // printf("UID = %d\n",res->content.hash_table_unit->UID);
                    res->content.hash_table_unit->isInitialised = 1;
                    res->content.hash_table_unit->children[0]->isInitialised = 1;

                    res->content.hash_table_unit->line_range[0] = x->range_ast[0];
                    res->content.hash_table_unit->children[0]->line_range[0] = x->range_ast[0];

                    res->content.hash_table_unit->line_range[1] = x->range_ast[1];
                    res->content.hash_table_unit->children[0]->line_range[1] = x->range_ast[1];

                    res->content.hash_table_unit->global_offset = GLOBAL_COUNTER;
                    populateSymbolTable(res->content.hash_table_unit, x, FUNCTION_STATE);
                }
                t = t->next;
            }
        }

        curr = node->children[2];
        hash_table_holder temp = createHashTableHolder(IP_OP_SCOPE);
        hash_table_holder temp_child = createHashTableHolder(FUNCTION_SYMBOL_TABLE);

        /*
            - Explicitly create a symbol table for the driver module
            - Initialise the offset of the module to the GLOBAL_COUNTER
            - Recusrively call the populateSymbolTable function with the module's symbol table
        */

        temp->global_offset = GLOBAL_COUNTER;
        temp->UID = UID_COUNTER; // UID is the unique identifier for each module
        temp_child->UID = UID_COUNTER++;

        temp->nesting_level = 0;
        temp_child->nesting_level = 1;

        temp->line_range[0] = curr->range_ast[0];
        temp_child->line_range[0] = curr->range_ast[0];
        temp->line_range[1] = curr->range_ast[1];
        temp_child->line_range[1] = curr->range_ast[1];
        strcpy(temp->self_name, "driver_module");
        strcpy(temp_child->self_name, "driver_module");
        temp->isInitialised = 1;
        temp_child->isInitialised = 1;

        // printf("UID = %d\n",temp->UID);
        // temp->parent = root;

        temp_child->parent = temp;
        temp->children = realloc(temp->children, sizeof(hash_table_holder) * (++temp->num_children));
        temp->children[temp->num_children - 1] = temp_child;

        node_content content;
        content.hash_table_unit = temp;
        insertIntoHashTable(root->hash_table, "driver_module", -1, FUNCTION_HASH_POINTER, content);
        populateSymbolTable(temp, curr, DRIVER_STATE);

        break;
    }

    case FUNCTION_STATE:
    {
        /*
            - Every module (other than the driver module) will have 4 children
            - The first child will be the module name
            - We need to populate the hash_table_holder of current module with the ip_list and op_list
            - We also need to put them in the symbol table of the module
            - We will also traverse the statements of the module and populate the symbol table of the module
        */
        // printf("Function State\n");
        populateSymbolTable(root, node->children[1], IP_STATE);
        populateSymbolTable(root, node->children[2], OP_STATE);
        root->children[0]->ip_list = root->ip_list;
        root->children[0]->op_list = root->op_list;
        root->children[0]->global_offset = GLOBAL_COUNTER;
        populateSymbolTable(root->children[0], node->children[3], STATEMENTS_STATE);
        root->line_range[1] = root->children[0]->line_range[1];
        callSemanticCheck(node, MODULE_DEFINITION_CHECK, root);
        // printf("At end of FUNCTION_STATE\n");
        break;
    }

    case DRIVER_STATE:
    {
        /*
            - Driver module doesn't have any ip_list or op_list
            - We will also traverse the statements of the module and populate the symbol table of the module
        */
        root->children[0]->global_offset = GLOBAL_COUNTER;
        populateSymbolTable(root->children[0], node, STATEMENTS_STATE);
        break;
    }

    case DECLARE_STATE:
    {
        /*
            Grammar -

                declareStmt : DECLARE idList COLON dataType SEMICOL
                This rule is reponsible for declaring variables in the current scope.

                DECLARE A,B : INTEGER;
                DECLARE C,D : ARRAY [1..10] OF INTEGER;
                DECLARE E,F : ARRAY [-a..b] OF INTEGER;

                 || ------------------ ||
                 || |   A   |INTEGER | ||
                 || |   B   |INTEGER | ||
                 || |   C   | ARRAY  | ||  - Static
                 || |   D   | ARRAY  | ||  - Static
                 || |   E   | ARRAY  | ||  - Dynamic
                 || |   F   | ARRAY  | ||  - Dynamic
                 || |                | ||
                 || |                | ||
                 || |                | ||
                 || ------------------ ||

            Semantics -

                declareStmt.addr = CREATE_NODE("DECLARE", idList.head, dataType.addr)

                AST Node (Diagram)

                DECLARE
                /     \
               A->B   INTEGER


                DECLARE
                /     \
              C->D    ARRAY
                    /     \
                  RANGE   INTEGER
                 /    \
                1     10


                DECLARE
                /     \
               E->F   ARRAY
                     /     \
                  RANGE   INTEGER
                 /    \
                -     b
               / \
            NULL  a

            Task(s) -
                - We have to traverse the idList and insert the ID's along with their datatype in the symbol table [Three Different Methods]

                - Ensure that in case of array, store these information in the array record - | ARRAY | INTEGER | 1 .. 10 | Static |

                - We have to check before insertion that the id is not already present in the symbol table, if it is present then give and error and proceed
                Note 1: We will check only the ID because overloading is not allowed
                Note 2: We will check after every time we populate the symbol table, to account for | DECLARE A,A: INTEGER; | case

                - We will also update the offset

                - We have to populate the information in the current hash table, so there is no need to create a new hashtable holder

                - In case of dynamic arrays, check if the range variables have already been declared or not. If declared, check if the type is INTEGER or not. If not, give an error.

                - Check if ID is not matching with a module name, if it is, give an error.

        */

        //  : Primitive Data Type (INTEGER | REAL | BOOLEAN)

        if (node->children[1]->num_children == 0)
        {
            ASTNode IDList = node->children[0];

            if (IDList != NULL)
            {
                ASTList id_list = IDList->info_list;
                ASTListNode t = id_list->head;

                while (t != NULL)
                {
                    ASTNode x = t->data;
                    hash_component check = searchHashTable(root->hash_table, x->dataForPrint->lexeme);
                    hash_component check_b = searchHashTable(BIG_ROOT->hash_table, x->dataForPrint->lexeme);

                    int additional_flag_check = 0;
                    if (check == NULL && check_b == NULL && root->type == FUNCTION_SYMBOL_TABLE)
                    {
                        hash_component additional_check = searchHashTable(root->parent->hash_table, x->dataForPrint->lexeme);
                        if (additional_check != NULL && additional_check->info == OP_PRIMITIVE_TYPE)
                        {
                            check = additional_check;
                            additional_flag_check = 1;
                        }
                    }

                    if (check == NULL && check_b == NULL)
                    {
                        ip_op_info info = malloc(sizeof(struct ip_op_info));
                        info->type = malloc(sizeof(struct type_pair));
                        strcpy(info->identifier, x->dataForPrint->lexeme);
                        info->type->type_name = PRIMITIVE_TYPE;
                        info->type->type_expr.data_type = node->children[1]->dataForPrint->u.term;

                        node_content content;
                        content.type = info->type->type_expr.data_type;

                        insertIntoHashTable(root->hash_table, info->identifier, GLOBAL_COUNTER, PRIMITIVE_TYPE, content);

                        if (info->type->type_expr.data_type == INTEGER)
                        {
                            GLOBAL_COUNTER += 4;
                        }
                        else if (info->type->type_expr.data_type == REAL)
                        {
                            GLOBAL_COUNTER += 16;
                        }
                        else if (info->type->type_expr.data_type == BOOLEAN)
                        {
                            GLOBAL_COUNTER += 4;
                        }
                    }
                    else if (additional_flag_check == 1)
                    {
                        additional_flag_check = 0;
                    }
                    else
                    {
                        char errormsg[100];
                        sprintf(errormsg, "ERROR in Line %d | ID %s already used in symbol table\n", x->dataForPrint->lineNo, x->dataForPrint->lexeme);
                        type_pair er = createErrorType(errormsg);
                        // displayErrorType(er);
                    }
                    t = t->next;
                }
            }
        }

        // Case 2 : Array Data Type (INTEGER | REAL | BOOLEAN)

        if (node->children[1]->num_children == 2)
        {
            ASTNode IDList = node->children[0];

            if (IDList != NULL)
            {
                ASTList id_list = IDList->info_list;
                ASTListNode t = id_list->head;

                while (t != NULL)
                {
                    ASTNode x = t->data;
                    hash_component check = searchHashTable(root->hash_table, x->dataForPrint->lexeme);
                    hash_component check_b = searchHashTable(BIG_ROOT->hash_table, x->dataForPrint->lexeme);

                    int additional_flag_check = 0;
                    if (check == NULL && check_b == NULL && root->type == FUNCTION_SYMBOL_TABLE)
                    {
                        hash_component additional_check = searchHashTable(root->parent->hash_table, x->dataForPrint->lexeme);
                        if (additional_check != NULL && additional_check->info == OP_PRIMITIVE_TYPE)
                        {
                            check = additional_check;
                            additional_flag_check = 1;
                        }
                    }
                    if (check == NULL && check_b == NULL)
                    {
                        ip_op_info info = malloc(sizeof(struct ip_op_info));
                        info->type = malloc(sizeof(struct type_pair));
                        strcpy(info->identifier, x->dataForPrint->lexeme);

                        info->type->type_name = ARRAY_TYPE;
                        info->type->type_expr.array_info = malloc(sizeof(struct array_record));
                        // info->type->type_expr.array_info->range = NULL;

                        int left, right;
                        int eorrr = 0;

                        if (node->children[1]->children[0]->children[0]->dataForPrint->u.term == ID ||
                            node->children[1]->children[0]->children[1]->dataForPrint->u.term == ID ||
                            (node->children[1]->children[0]->children[0]->num_children == 2 && node->children[1]->children[0]->children[0]->children[1]->dataForPrint->u.term == ID) ||
                            (node->children[1]->children[0]->children[1]->num_children == 2 && node->children[1]->children[0]->children[1]->children[1]->dataForPrint->u.term == ID))
                        {
                            info->type->type_expr.array_info->is_static = 0;
                        }
                        else
                        {
                            info->type->type_expr.array_info->is_static = 3;
                        }

                        if (info->type->type_expr.array_info->is_static == 3)
                        {
                            if (node->children[1]->children[0]->children[0]->num_children == 2)
                            {
                                if (node->children[1]->children[0]->children[0]->dataForPrint->u.term == MINUS)
                                    left = -1 * (int)(node->children[1]->children[0]->children[0]->children[1]->dataForPrint->valueIfNumber);
                                else
                                    left = (int)(node->children[1]->children[0]->children[0]->children[1]->dataForPrint->valueIfNumber);
                            }
                            else
                            {
                                left = (int)(node->children[1]->children[0]->children[0]->dataForPrint->valueIfNumber);
                            }
                            if (node->children[1]->children[0]->children[1]->num_children == 2)
                            {
                                if (node->children[1]->children[0]->children[1]->dataForPrint->u.term == MINUS)
                                    right = -1 * (int)(node->children[1]->children[0]->children[1]->children[1]->dataForPrint->valueIfNumber);
                                else
                                    right = (int)(node->children[1]->children[0]->children[0]->children[1]->dataForPrint->valueIfNumber);
                            }
                            else
                            {
                                right = (int)(node->children[1]->children[0]->children[1]->dataForPrint->valueIfNumber);
                            }

                            info->type->type_expr.array_info->range.static_range[0] = left;
                            info->type->type_expr.array_info->range.static_range[1] = right;

                            info->type->type_expr.array_info->data_type_e = node->children[1]->children[1]->dataForPrint->u.term;
                            strcpy(info->type->type_expr.array_info->data_type_s, node->children[1]->children[1]->dataForPrint->tokenName);

                            node_content content;
                            content.array_info = info->type->type_expr.array_info;
                            insertIntoHashTable(root->hash_table, info->identifier, GLOBAL_COUNTER, ARRAY_TYPE, content);
                        }
                        else
                        {
                            /*
                                For Dynamic Array

                                There are many cases possible
                                DECLARE C,D : ARRAY [a..b] OF INTEGER;
                                DECLARE G,H : ARRAY [-a..b] OF INTEGER;
                                DECLARE E,F : ARRAY [1..b] OF INTEGER;
                                DECLARE G,H : ARRAY [-1..b] OF INTEGER;

                                case 1: a .. b

                                Here, we need to store two characters 'a' and 'b' in the range array. Currently, range array is of type int. So, we need to change it to char
                                But, we also have to store the range incase they are integer, so we need two different range(s) leftRange and rightRange.
                                They are going to be of union type which will contain int and char both, depending on our requirements.

                                case 2: -a .. b

                                After solving the problem of storing the range information, we now need to store -a in the leftRange and b in the rightRange.
                                We will have to concatenate - with a and store it in the range array. I don't think we can store -a in char, so we need a different data structure.
                                We can probably create a flag which stores the information about negative values for each leftRange and rightRange.

                                case 3: 1 .. b and case 4: -1 .. b

                                We can use the previous ideas to solve this problem as well.

                                Similar to above if statement, we need multiple cases for dynamic array as well.
                                (MINUS,PLUS etc.)
                            */

                            // RANGE IS NULL

                            // b, d, -a etc (or the ID used should be declared atleast once and should be of integer type)

                            /*
                            ID..ID
                            ID..NUM
                            NUM..ID

                            -ID..ID
                            -ID..NUM
                            -NUM..ID

                            ID..-ID
                            ID..-NUM
                            NUM..-ID

                            -ID..-ID
                            -ID..-NUM
                            -NUM..-ID

                            +ID..ID
                            +ID..NUM
                            +NUM..ID

                            ID..+ID
                            ID..+NUM
                            NUM..+ID

                            +ID..+ID
                            +ID..+NUM
                            +NUM..+ID
                            */
                            char *leftRange = NULL;
                            char *rightRange = NULL;

                            if (node->children[1]->children[0]->children[0]->dataForPrint->u.term == ID)
                            {
                                info->type->type_expr.array_info->is_static = 1;
                                hash_component t = searchScopes(root, node->children[1]->children[0]->children[0]->dataForPrint->lexeme);
                                if (t)
                                {
                                    if (t->content.type != INTEGER)
                                    {
                                        char errormsg[150];
                                        sprintf(errormsg, "ERROR in Line %d | Array Index not of a valid Data Type |\nIndex array must be of type Integer\n", node->children[1]->children[0]->children[0]->dataForPrint->lineNo);
                                        type_pair er = createErrorType(errormsg);
                                        // displayErrorType(er);
                                        eorrr = 1;
                                    }
                                    else
                                    {
                                        leftRange = malloc(sizeof(char) * (LEXEME_SIZE + 1));
                                        strcpy(leftRange, node->children[1]->children[0]->children[0]->dataForPrint->lexeme);
                                    }
                                }
                                else
                                {
                                    // Error message:
                                    char errormsg[150];
                                    sprintf(errormsg, "ERROR in Line %d | Array Index not declared before use\n", node->children[1]->children[0]->children[0]->dataForPrint->lineNo);
                                    type_pair er = createErrorType(errormsg);
                                    // displayErrorType(er);
                                    eorrr = 1;
                                }
                            }

                            if (node->children[1]->children[0]->children[1]->dataForPrint->u.term == ID)
                            {
                                if (info->type->type_expr.array_info->is_static == 1)
                                    info->type->type_expr.array_info->is_static = 0;
                                else
                                    info->type->type_expr.array_info->is_static = 2;
                                hash_component t = searchScopes(root, node->children[1]->children[0]->children[1]->dataForPrint->lexeme);
                                if (t)
                                {
                                    if (t->content.type != INTEGER)
                                    {
                                        char errormsg[150];
                                        sprintf(errormsg, "ERROR in Line %d | Array Index not of a valid Data Type |\nIndex array must be of type Integer\n", node->children[1]->children[0]->children[1]->dataForPrint->lineNo);
                                        type_pair er = createErrorType(errormsg);
                                        // displayErrorType(er);
                                        eorrr = 1;
                                    }
                                    else
                                    {
                                        rightRange = malloc(sizeof(char) * (LEXEME_SIZE + 1));
                                        strcpy(rightRange, node->children[1]->children[0]->children[1]->dataForPrint->lexeme);
                                    }
                                }
                                else
                                {
                                    // Error message:
                                    char errormsg[150];
                                    sprintf(errormsg, "ERROR in Line %d | Array Index not declared before use\n", node->children[1]->children[0]->children[1]->dataForPrint->lineNo);
                                    type_pair er = createErrorType(errormsg);
                                    // displayErrorType(er);
                                    eorrr = 1;
                                }
                            }

                            if (node->children[1]->children[0]->children[0]->num_children == 2 && node->children[1]->children[0]->children[0]->children[1]->dataForPrint->u.term == ID)
                            {
                                printf("LEFT MINUS ID CASE : %d", info->type->type_expr.array_info->is_static);
                                if (info->type->type_expr.array_info->is_static == 2)
                                    info->type->type_expr.array_info->is_static = 0;
                                else if (info->type->type_expr.array_info->is_static == 1)
                                    info->type->type_expr.array_info->is_static = 0;
                                else
                                    info->type->type_expr.array_info->is_static = 2;

                                hash_component t = searchScopes(root, node->children[1]->children[0]->children[0]->children[1]->dataForPrint->lexeme);
                                if (t)
                                {
                                    if (t->content.type != INTEGER)
                                    {
                                        char errormsg[150];
                                        sprintf(errormsg, "ERROR in Line %d | Array Index not of a valid Data Type |\nIndex array must be of type Integer\n", node->children[1]->children[0]->children[0]->children[1]->dataForPrint->lineNo);
                                        type_pair er = createErrorType(errormsg);
                                        // displayErrorType(er);
                                        eorrr = 1;
                                    }
                                    else
                                    {
                                        leftRange = malloc(sizeof(char) * (LEXEME_SIZE + 1));
                                        if (node->children[1]->children[0]->children[0]->dataForPrint->u.term == MINUS)
                                        {
                                            strcpy(leftRange, "-");
                                            strcat(leftRange, node->children[1]->children[0]->children[0]->children[1]->dataForPrint->lexeme);
                                        }
                                        else
                                        {
                                            strcpy(leftRange, node->children[1]->children[0]->children[0]->children[1]->dataForPrint->lexeme);
                                        }
                                    }
                                }
                                else
                                {
                                    // Error message:
                                    char errormsg[150];
                                    sprintf(errormsg, "ERROR in Line %d | Array Index not declared before use\n", node->children[1]->children[0]->children[0]->children[1]->dataForPrint->lineNo);
                                    type_pair er = createErrorType(errormsg);
                                    // displayErrorType(er);
                                    eorrr = 1;
                                }
                            }

                            if (node->children[1]->children[0]->children[1]->num_children == 2 && node->children[1]->children[0]->children[1]->children[1]->dataForPrint->u.term == ID)
                            {
                                printf("RIGHT MINUS ID CASE : %d", info->type->type_expr.array_info->is_static);
                                if (info->type->type_expr.array_info->is_static == 1)
                                    info->type->type_expr.array_info->is_static = 0;
                                else
                                    info->type->type_expr.array_info->is_static = 2;
                                hash_component t = searchScopes(root, node->children[1]->children[0]->children[1]->children[1]->dataForPrint->lexeme);
                                if (t)
                                {
                                    if (t->content.type != INTEGER)
                                    {
                                        char errormsg[150];
                                        sprintf(errormsg, "ERROR in Line %d | Array Index not of a valid Data Type |\nIndex array must be of type Integer\n", node->children[1]->children[0]->children[1]->children[1]->dataForPrint->lineNo);
                                        type_pair er = createErrorType(errormsg);
                                        // displayErrorType(er);
                                        eorrr = 1;
                                    }
                                    else
                                    {
                                        rightRange = malloc(sizeof(char) * (LEXEME_SIZE + 1));
                                        if (node->children[1]->children[0]->children[1]->dataForPrint->u.term == MINUS)
                                        {
                                            strcpy(rightRange, "-");
                                            strcat(rightRange, node->children[1]->children[0]->children[1]->children[1]->dataForPrint->lexeme);
                                        }
                                        else
                                        {
                                            strcpy(rightRange, node->children[1]->children[0]->children[1]->children[1]->dataForPrint->lexeme);
                                        }
                                    }
                                }
                                else
                                {
                                    // Error message:
                                    char errormsg[150];
                                    sprintf(errormsg, "ERROR in Line %d | Array Index not declared before use\n", node->children[1]->children[0]->children[1]->children[1]->dataForPrint->lineNo);
                                    type_pair er = createErrorType(errormsg);
                                    // displayErrorType(er);
                                    eorrr = 1;
                                }
                            }

                            if (eorrr == 0)
                            {
                                if (leftRange == NULL)
                                {
                                    leftRange = malloc(sizeof(char) * (LEXEME_SIZE + 1));
                                    if (node->children[1]->children[0]->children[0]->num_children == 2)
                                    {
                                        if (node->children[1]->children[0]->children[0]->dataForPrint->u.term == MINUS)
                                        {
                                            strcpy(leftRange, "-");
                                            strcat(leftRange, node->children[1]->children[0]->children[0]->children[1]->dataForPrint->lexeme);
                                        }
                                        else
                                        {
                                            strcpy(leftRange, node->children[1]->children[0]->children[0]->children[1]->dataForPrint->lexeme);
                                        }
                                    }
                                    else
                                    {
                                        strcpy(leftRange, node->children[1]->children[0]->children[0]->dataForPrint->lexeme);
                                    }
                                    info->type->type_expr.array_info->is_static = 2;
                                }

                                if (rightRange == NULL)
                                {
                                    rightRange = malloc(sizeof(char) * (LEXEME_SIZE + 1));
                                    if (node->children[1]->children[0]->children[1]->num_children == 2)
                                    {
                                        if (node->children[1]->children[0]->children[1]->dataForPrint->u.term == MINUS)
                                        {
                                            strcpy(rightRange, "-");
                                            strcat(rightRange, node->children[1]->children[0]->children[1]->children[1]->dataForPrint->lexeme);
                                        }
                                        else
                                        {
                                            strcpy(rightRange, node->children[1]->children[0]->children[1]->children[1]->dataForPrint->lexeme);
                                        }
                                    }
                                    else
                                    {
                                        strcpy(rightRange, node->children[1]->children[0]->children[1]->dataForPrint->lexeme);
                                    }
                                    info->type->type_expr.array_info->is_static = 1;
                                }

                                info->type->type_expr.array_info->range.dynamic_range[0] = leftRange;
                                info->type->type_expr.array_info->range.dynamic_range[1] = rightRange;

                                info->type->type_expr.array_info->data_type_e = node->children[1]->children[1]->dataForPrint->u.term;
                                strcpy(info->type->type_expr.array_info->data_type_s, node->children[1]->children[1]->dataForPrint->tokenName);

                                node_content content;
                                content.array_info = info->type->type_expr.array_info;
                                insertIntoHashTable(root->hash_table, info->identifier, GLOBAL_COUNTER, ARRAY_TYPE, content);
                            }
                        }

                        if (info->type->type_expr.array_info->data_type_e == INTEGER && info->type->type_expr.array_info->is_static == 3)
                        {
                            GLOBAL_COUNTER += (abs(right - left + 1) * 4 + 2);
                        }
                        else if (info->type->type_expr.array_info->data_type_e == REAL && info->type->type_expr.array_info->is_static == 3)
                        {
                            GLOBAL_COUNTER += (abs(right - left + 1) * 16 + 2);
                        }
                        else if (info->type->type_expr.array_info->data_type_e == BOOLEAN && info->type->type_expr.array_info->is_static == 3)
                        {
                            GLOBAL_COUNTER += (abs(right - left + 1) * 4 + 2);
                        }
                        else
                        {
                            GLOBAL_COUNTER += 4;
                        }
                    }
                    else if (additional_flag_check == 1)
                    {
                        additional_flag_check = 0;
                    }
                    else
                    {
                        char errormsg[100];
                        sprintf(errormsg, "ERROR in Line %d | ID %s already used in symbol table\n", x->dataForPrint->lineNo, x->dataForPrint->lexeme);
                        type_pair er = createErrorType(errormsg);
                        // displayErrorType(er);
                    }
                    t = t->next;
                }
            }
        }

        // displayHashUnit(root);

        // Just Return
        return;
        break;
    }

    case IP_STATE:
    {
        ASTListNode t = node->info_list->head;
        root->ip_list = malloc(sizeof(struct ip_op_list));
        root->ip_list->head = NULL;
        root->ip_list->tail = NULL;
        root->ip_list->size = 0;

        /*
            - Going to traverse the list of input parameters
            - For each input parameter, we will create an ip_op_info struct
            - We will also create a type_pair struct
            - We will also create a hash_component struct
            - We will insert the hash_component struct into the symbol table of the module
            - We will insert the ip_op_info struct into the ip_list of the module
        */

        while (t != NULL)
        {
            ASTNode x = t->data;

            ip_op_info info = malloc(sizeof(struct ip_op_info));
            info->type = malloc(sizeof(struct type_pair));
            strcpy(info->identifier, x->children[0]->dataForPrint->lexeme); /* Copy the Name of the Parameter */

            if (x->children[1]->num_children > 0) /* Condition if it has an array type record stored */
            {
                info->type->type_name = IP_ARRAY_TYPE;
                info->type->type_expr.array_info = malloc(sizeof(struct array_record));
                // info->type->type_expr.array_info->range = NULL;
                info->type->type_expr.array_info->is_static = 3;

                int left, right;

                if (x->children[1]->children[0]->children[0]->num_children > 1) /* Condition if the Left_bound has a MINUS/PLUS sign */
                {
                    if (x->children[1]->children[0]->children[0]->dataForPrint->u.term == MINUS)
                        left = -1 * (int)(x->children[1]->children[0]->children[0]->children[1]->dataForPrint->valueIfNumber);
                    else
                        left = (int)(x->children[1]->children[0]->children[0]->children[1]->dataForPrint->valueIfNumber);
                }
                else /* Condition if there is no MINUS/PLUS sign */
                {
                    left = (int)(x->children[1]->children[0]->children[0]->dataForPrint->valueIfNumber);
                }
                if (x->children[1]->children[0]->children[1]->num_children > 1) /* Condition if the Right_bound has a MINUS/PLUS sign */
                {
                    if (x->children[1]->children[0]->children[1]->dataForPrint->u.term == MINUS)
                        right = -1 * (int)(x->children[1]->children[0]->children[1]->children[1]->dataForPrint->valueIfNumber);
                    else
                        right = (int)(x->children[1]->children[0]->children[0]->children[1]->dataForPrint->valueIfNumber);
                }
                else /* Condition if there is no MINUS/PLUS sign */
                {
                    right = (int)(x->children[1]->children[0]->children[1]->dataForPrint->valueIfNumber);
                }

                info->type->type_expr.array_info->range.static_range[0] = left;
                info->type->type_expr.array_info->range.static_range[1] = right;
                // info->type->type_expr.array_info->range[0] = left;
                // info->type->type_expr.array_info->range[1] = right;

                info->type->type_expr.array_info->data_type_e = x->children[1]->children[1]->dataForPrint->u.term; /* DataType of the Array */
                strcpy(info->type->type_expr.array_info->data_type_s, x->children[1]->children[1]->dataForPrint->tokenName);

                node_content content;
                content.array_info = info->type->type_expr.array_info;
                insertIntoHashTable(root->hash_table, info->identifier, GLOBAL_COUNTER, IP_ARRAY_TYPE, content); /* Inserting into the Symbol Table */

                /* Next 3 conditions update the GLOBAL_COUNTER based on size and type of array */
                if (info->type->type_expr.array_info->data_type_e == INTEGER)
                {
                    GLOBAL_COUNTER += (abs(right - left + 1) * 4 + 2);
                }
                else if (info->type->type_expr.array_info->data_type_e == REAL)
                {
                    GLOBAL_COUNTER += (abs(right - left + 1) * 16 + 2);
                }
                else if (info->type->type_expr.array_info->data_type_e == BOOLEAN)
                {
                    GLOBAL_COUNTER += (abs(right - left + 1) * 4 + 2);
                }
            }
            else /* Condition if it has a primitive type record stored */
            {
                // printf("Here 1\n");
                info->type->type_name = IP_PRIMITIVE_TYPE;
                // printf("%s,%d\n",x->children[1]->dataForPrint->lexeme,x->children[1]->dataForPrint->u.term);
                info->type->type_expr.data_type = x->children[1]->dataForPrint->u.term;
                // printf("Here 2\n");
                node_content content;
                content.type = info->type->type_expr.data_type;
                insertIntoHashTable(root->hash_table, info->identifier, GLOBAL_COUNTER, IP_PRIMITIVE_TYPE, content); /* Inserting into the Symbol Table */

                /* Next 3 conditions update the GLOBAL_COUNTER based on size and type of primitive type */
                if (info->type->type_expr.data_type == INTEGER)
                {
                    GLOBAL_COUNTER += 4;
                }
                else if (info->type->type_expr.data_type == REAL)
                {
                    GLOBAL_COUNTER += 16;
                }
                else if (info->type->type_expr.data_type == BOOLEAN)
                {
                    GLOBAL_COUNTER += 4;
                }
            }

            insertInList(root->ip_list, info); /* Inserting into the Input Parameter List */
            t = t->next;
        }
        // displayHashUnit(root);
        break;
    }

    case OP_STATE:
    {
        ASTListNode t = node->info_list->head;
        root->op_list = malloc(sizeof(struct ip_op_list));
        root->op_list->head = NULL;
        root->op_list->tail = NULL;
        root->op_list->size = 0;

        /*
            - Going to traverse the list of output parameters
            - For each output parameter, we will create an ip_op_info struct
            - We will also create a type_pair struct
            - We will also create a hash_component struct
            - We will insert the hash_component struct into the symbol table of the module
            - We will insert the ip_op_info struct into the op_list of the module
        */

        while (t != NULL)
        {
            ASTNode x = t->data;

            ip_op_info info = malloc(sizeof(struct ip_op_info));
            info->type = malloc(sizeof(struct type_pair));
            strcpy(info->identifier, x->children[0]->dataForPrint->lexeme);

            /* Fortunately, output_list can't have Array type */
            info->type->type_name = OP_PRIMITIVE_TYPE;
            info->type->type_expr.data_type = x->children[1]->dataForPrint->u.term;

            node_content content;
            content.type = info->type->type_expr.data_type;
            insertIntoHashTable(root->hash_table, info->identifier, GLOBAL_COUNTER, OP_PRIMITIVE_TYPE, content); /* Inserting into the Symbol Table */

            /* Next 3 conditions update the GLOBAL_COUNTER based on size and type of primitive type */
            if (info->type->type_expr.data_type == INTEGER)
            {
                GLOBAL_COUNTER += 4;
            }
            else if (info->type->type_expr.data_type == REAL)
            {
                GLOBAL_COUNTER += 16;
            }
            else if (info->type->type_expr.data_type == BOOLEAN)
            {
                GLOBAL_COUNTER += 4;
            }

            insertInList(root->op_list, info); /* Inserting into the Output Parameter List */
            t = t->next;
        }
        break;
    }

    case STATEMENTS_STATE:
    {
        /*
            - Traverse all types of statements
            - For each statement that is Declaration or Iteration or Conditional, call the function recursively adjusting the parameters
            - For each statement where there needs to be a type check/semantic check, call the semantic_check function
        */

        if (node->info_list == NULL)
            break;
        ASTListNode statement_node = node->info_list->head;
        int i = 0;
        while (statement_node != NULL)
        {
            ASTNode statement_data = statement_node->data;

            if (strcmp(statement_data->dataForPrint->currentNodeSymbol, "declareStmt") == 0)
            {
                populateSymbolTable(root, statement_data, DECLARE_STATE);
            }
            else if (strcmp(statement_data->dataForPrint->currentNodeSymbol, "GET_VALUE") == 0)
            {
                callSemanticCheck(statement_data, GET_VALUE_CHECK, root);
            }
            else if (strcmp(statement_data->dataForPrint->currentNodeSymbol, "PRINT") == 0)
            {
                callSemanticCheck(statement_data, PRINT_VALUE_CHECK, root);
            }
            else if (strcmp(statement_data->dataForPrint->currentNodeSymbol, "ASSIGNOP") == 0)
            {
                callSemanticCheck(statement_data, ASSIGN_CHECK, root);
            }
            else if (strcmp(statement_data->dataForPrint->currentNodeSymbol, "moduleReuseStmt") == 0)
            {
                char *callee = statement_data->children[1]->dataForPrint->lexeme;           /* Module Name of callee */
                hash_component callee_unit = searchHashTable(BIG_ROOT->hash_table, callee); /* Search for the callee in the BIG_ROOT Symbol Table */
                if (callee_unit != NULL)                                                    /* Proceed only if the module exists */
                {
                    hash_table_holder t = root;
                    /* Basically we want the caller's name from the hash_table_unit of the over-arching function
                        and not any hash_table_unit of any scope within it */
                    while (t->type != FUNCTION_SYMBOL_TABLE)
                    {
                        t = t->parent;
                    }
                    char *caller = t->self_name; /* Module Name of caller */

                    /* Inserting the caller-callee pair into the module_calls array */
                    // printf("\n\ncurr_ptr: %d curr_size:%d\n\n",curr_ptr,curr_size);
                    if (curr_ptr >= curr_size)
                    {
                        curr_size = 2 * curr_size;
                        module_calls = realloc(module_calls, sizeof(char *) * curr_size);
                        for (int i = curr_size / 2; i < curr_size; i++)
                        {
                            module_calls[i] = NULL;
                        }
                    }
                    module_calls[curr_ptr] = malloc((LEXEME_SIZE + 1) * sizeof(char));
                    strcpy(module_calls[curr_ptr], caller);
                    curr_ptr++;
                    module_calls[curr_ptr] = malloc((LEXEME_SIZE + 1) * sizeof(char));
                    strcpy(module_calls[curr_ptr], callee);
                    curr_ptr++;
                }

                callSemanticCheck(statement_data, MODULE_REUSE_CHECK, root);
            }
            else if (strcmp(statement_data->dataForPrint->currentNodeSymbol, "FOR") == 0 ||
                     strcmp(statement_data->dataForPrint->currentNodeSymbol, "WHILE") == 0)
            {
                hash_table_holder child = createHashTableHolder(BLOCK_SCOPE);
                child->parent = root;
                strcpy(child->self_name, "FOR_WHILE_SCOPE");
                root->children = realloc(root->children, sizeof(hash_table_holder) * (++root->num_children));
                root->children[root->num_children - 1] = child;
                child->global_offset = GLOBAL_COUNTER;
                child->line_range[0] = statement_data->range_ast[0];
                child->line_range[1] = statement_data->range_ast[1];
                child->nesting_level = root->nesting_level + 1;

                if (strcmp(statement_data->dataForPrint->currentNodeSymbol, "FOR") == 0)
                {
                    node_content content;
                    content.type = INTEGER;
                    insertIntoHashTable(child->hash_table, statement_data->children[0]->dataForPrint->lexeme, child->global_offset, PRIMITIVE_TYPE, content);
                    // child->global_offset += 4;
                    GLOBAL_COUNTER += 4;
                    callSemanticCheck(statement_data, FOR_CHECK, root);
                    populateSymbolTable(child, statement_data->children[2], STATEMENTS_STATE);
                }
                else
                {
                    callSemanticCheck(statement_data, WHILE_EXPRESSION_CHECK, root);
                    populateSymbolTable(child, statement_data->children[1], STATEMENTS_STATE);
                }

                // displayHashUnit(child);     // For testing purposes
            }
            else if (strcmp(statement_data->dataForPrint->currentNodeSymbol, "condionalStmt") == 0)
            {

                callSemanticCheck(statement_data, CONDITIONAL_CHECK, root);
                if (switch_error_flag == 1)
                {
                    switch_error_flag = 0;
                }
                else
                {

                    ASTListNode case_node = statement_data->children[1]->info_list->head;
                    while (case_node != NULL)
                    {
                        hash_table_holder child = createHashTableHolder(BLOCK_SCOPE);
                        child->parent = root;
                        root->num_children++;
                        root->children = realloc(root->children, sizeof(hash_table_holder) * (root->num_children));
                        root->children[root->num_children - 1] = child;
                        child->global_offset = GLOBAL_COUNTER;
                        child->line_range[0] = case_node->data->range_ast[0];
                        child->line_range[1] = case_node->data->range_ast[1];
                        child->nesting_level = root->nesting_level + 1;
                        populateSymbolTable(child, case_node->data->children[1], STATEMENTS_STATE);
                        case_node = case_node->next;
                    }
                    if (statement_data->children[2] != NULL)
                    {
                        hash_table_holder child = createHashTableHolder(BLOCK_SCOPE);
                        child->parent = root;
                        root->num_children++;
                        root->children = realloc(root->children, sizeof(hash_table_holder) * (root->num_children));
                        root->children[root->num_children - 1] = child;
                        child->global_offset = GLOBAL_COUNTER;
                        child->line_range[0] = statement_data->children[2]->range_ast[0];
                        child->line_range[1] = statement_data->children[2]->range_ast[1];
                        child->nesting_level = root->nesting_level + 1;
                        populateSymbolTable(child, statement_data->children[2], STATEMENTS_STATE);
                    }
                }
            }
            // i++;
            // if (i == node->info_list->size)
            // {
            //     root->line_range[1] = statement_node->data->dataForPrint->lineNo;
            // }
            statement_node = statement_node->next;
        }
        // if (i == 0)
        //     root->line_range[1] = node->dataForPrint->lineNo;
        break;
    }
    }
    return;
}

int detectRecursion(int curr, int directed_graph[][UID_COUNTER])
{
    if (visited[curr] == 1 && visited_on_current_path[curr] == 1)
    {
        cycle_origin = curr;
        path[i_path++] = curr;
        return 0;
    }

    visited[curr] = 1;
    visited_on_current_path[curr] = 1;
    for (int i = 0; i < UID_COUNTER; i++)
    {
        int r = -1;
        if (directed_graph[curr][i] == 1)
        {
            r = detectRecursion(i, directed_graph);
        }

        if (r == 0)
        {
            path[i_path++] = curr;
            return 0;
        }
    }
    visited_on_current_path[curr] = 0;
    return 1;
}

int getOffset(char *identifier, hash_table_holder current)
{
    hash_table_holder temp = current;
    hash_component t = NULL;
    int offset = -1;
    while (temp != NULL)
    {
        t = searchHashTable(temp->hash_table, identifier);
        if (t != NULL)
            return t->offset;
        temp = temp->parent;
    }
    return offset;
}

int isInSet(Set *s, int x)
{
    return s->array[x];
}

void insertIntoSet(Set *s, int x)
{
    if (!isInSet(s, x))
    {
        s->array[x] = 1;
        s->size++;
    }
}

void removeFromSet(Set *s, int x)
{
    if (isInSet(s, x))
    {
        s->array[x] = 0;
        s->size--;
    }
}

Set *createIntersection(Set *s1, Set *s2)
{
    Set *s3 = (Set *)malloc(sizeof(Set));
    s3->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
    s3->size = 0;
    for (int i = 0; i < NUM_TERMINALS; i++)
    {
        if (s1->array[i] == 1 && s2->array[i] == 1)
        {
            s3->array[i] = 1;
            s3->size++;
        }
        else
        {
            s3->array[i] = 0;
        }
    }
    return s3;
}

Set *createUnion(Set *s1, Set *s2)
{
    Set *s3 = (Set *)malloc(sizeof(Set));
    s3->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
    s3->size = 0;
    for (int i = 0; i < NUM_TERMINALS; i++)
    {
        if (s1->array[i] == 1 || s2->array[i] == 1)
        {
            s3->array[i] = 1;
            s3->size++;
        }
        else
        {
            s3->array[i] = 0;
        }
    }
    return s3;
}

void printSet(Set *s)
{
    printf("Set: ");
    for (int i = 0; i < NUM_TERMINALS; i++)
    {
        if (s->array[i] == 1)
        {
            printf("%d ", i);
        }
    }
    printf("\n");
}