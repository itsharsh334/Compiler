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
#include <math.h>
#include "TheAST.c"

# define SYMBOL_TABLE_SIZE 128
# define LEXEME_SIZE 20


enum unit_type
{
    ROOT,
    IP_OP_SCOPE,
    FUNCTION_SYMBOL_TABLE,
    BLOCK_SCOPE
};
typedef enum unit_type unit_type;

enum info_type
{
    IP_PRIMITIVE_TYPE,
    OP_PRIMITIVE_TYPE,
    PRIMITIVE_TYPE,
    IP_ARRAY_TYPE,
    ARRAY_TYPE,
    FUNCTION_HASH_POINTER,
    SCOPE_POINTER,
    ERROR_TYPE
};
typedef enum info_type info_type;

struct type_pair
{
    info_type type_name;
    union
    {
        terminal_token data_type;
        struct array_record *array_info;
        char *error_message;
    } type_expr;
};
typedef struct type_pair *type_pair;

struct ip_op_info
{
    char identifier[LEXEME_SIZE + 1];
    type_pair type;
    struct ip_op_info *next;
};
typedef struct ip_op_info *ip_op_info;

struct ip_op_list
{
    struct ip_op_info *head;
    struct ip_op_info *tail;
    int size;
};
typedef struct ip_op_list *ip_op_list;

union array_range
{
    /*
        is_static = 0: pure dynamic | [x..y] | dynamic_range[0] = "x", dynamic_range[1] = "y"
        is_static = 1: left dynamic | [-x..-15] | dynamic_range[0] = "-x", dynamic_range[1] = "-15"
        is_static = 2: right dynamic | [1..y] | dynamic_range[0] = "1", dynamic_range[1] = "y"
        is_static = 3: pure static | [1..15] | static_range[0] = 1, static_range[1] = 15
    */
    int static_range[2];
    char *dynamic_range[2];
};
typedef union array_range array_range;

struct array_record
{
    int is_static;
    array_range range;
    terminal_token data_type_e;
    char data_type_s[LEXEME_SIZE + 1];
};
struct array_record *array_record;

union node_content
{
    terminal_token type;
    struct array_record *array_info;
    struct hash_table_holder *hash_table_unit;
};
typedef union node_content node_content;

struct hash_component
{
    char subject[LEXEME_SIZE + 1];
    info_type info;
    int offset;
    node_content content;
    struct hash_component *next;
};
typedef struct hash_component *hash_component;

struct hash_list
{
    hash_component head;
    int size;
};
typedef struct hash_list *hash_list;

struct hash_table_holder
{

    hash_list *hash_table;
    char self_name[LEXEME_SIZE + 1];
    int UID; // Unique ID only for the functions/modules
    int nesting_level;
    int num_children;
    int global_offset;
    int isInitialised;
    int line_range[2];
    unit_type type;

    struct ip_op_list *ip_list;
    struct ip_op_list *op_list;
    struct hash_table_holder *parent;
    struct hash_table_holder **children;
};
typedef struct hash_table_holder *hash_table_holder;

struct freeze_state
{
    struct hash_table_holder *curr_root;
    struct ASTNode *node;
};
typedef struct freeze_state *freeze_state;

struct collection_array
{
    char module_name[LEXEME_SIZE + 1];
    int line_range[2];
    hash_component the_array;
};
typedef struct collection_array *collection_array;

struct collection_error
{
    int line_num;
    type_pair err;
};
typedef struct collection_error *collection_error;

struct collection_func
{
    char module_name[LEXEME_SIZE + 1];
    int occupied_space;
};
typedef struct collection_func *collection_func;

collection_func all_functions[200];
int function_count = 0;
int function_space = 0;

collection_error all_errors[500];
int semantic_error_count = 0;

collection_array all_arrays[200];
int collector_count = 0;

int get_key(char *subject)
{
    /*Self explanatory*/
    long key = 7;
    int scale_factor = 31;
    int i = 0;
    while (i < strlen(subject))
    {
        char c = *(subject + i);
        key = key * scale_factor + c;
        i++;
    }
    if (key < 0)
        key = -key;
    return key % 64;
}

hash_table_holder createHashTableHolder(unit_type type)
{
    hash_table_holder hash_table_unit = (hash_table_holder)malloc(sizeof(struct hash_table_holder));
    hash_table_unit->hash_table = (hash_list *)malloc(sizeof(hash_list) * SYMBOL_TABLE_SIZE);
    hash_table_unit->self_name[0] = '\0';
    hash_table_unit->isInitialised = 0;
    hash_table_unit->nesting_level = -1;
    hash_table_unit->UID = -1;
    hash_table_unit->line_range[0] = -1;
    hash_table_unit->line_range[1] = -1;
    hash_table_unit->num_children = 0;
    hash_table_unit->global_offset = 0;
    hash_table_unit->type = type;
    hash_table_unit->ip_list = NULL;
    hash_table_unit->op_list = NULL;
    hash_table_unit->children = NULL;
    hash_table_unit->parent = NULL;
    int i = 0;
    for (i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        hash_table_unit->hash_table[i] = (hash_list)malloc(sizeof(struct hash_list));
        hash_table_unit->hash_table[i]->head = NULL;
        hash_table_unit->hash_table[i]->size = 0;
    }
    return hash_table_unit;
}

hash_component searchHashTable(hash_list *hash_table, char *subject)
{
    int key = get_key(subject);
    hash_list hash_list_unit = hash_table[key];
    if (hash_list_unit->head == NULL)
        return NULL;
    else
    {
        hash_component hash_component_unit = hash_list_unit->head;
        while (hash_component_unit != NULL)
        {
            if (strcmp(hash_component_unit->subject, subject) == 0)
                return hash_component_unit;
            hash_component_unit = hash_component_unit->next;
        }
        return NULL;
    }
}

void insertIntoHashTable(hash_list *hash_table, char *subject, int offset, info_type info, node_content content)
{
    int key = get_key(subject);
    hash_list hash_list_unit = hash_table[key];

    hash_component hash_component_unit = (hash_component)malloc(sizeof(struct hash_component));
    strcpy(hash_component_unit->subject, subject);
    hash_component_unit->offset = offset;
    // hash_component_unit->UID = -1;
    hash_component_unit->info = info;
    hash_component_unit->content = content;
    hash_component_unit->next = NULL;

    if (hash_list_unit->head == NULL)
    {
        hash_list_unit->head = hash_component_unit;
        hash_list_unit->size = 1;
    }
    else
    {
        hash_component_unit->next = hash_list_unit->head;
        hash_list_unit->head = hash_component_unit;
        hash_list_unit->size++;
    }
}

void displayIP_OP_List(ip_op_list list)
{
    if (list == NULL)
    {
        printf("- - - - -\n");
        return;
    }
    ip_op_info t = list->head;
    while (t != NULL)
    {
        printf("| Identifier: %s| ", t->identifier);
        printf("Type: %d| ", t->type->type_name);
        if (t->type->type_name == IP_ARRAY_TYPE)
        {
            printf("Static[1,4]: %d| ", t->type->type_expr.array_info->is_static);
            if (t->type->type_expr.array_info->is_static == 3)
                printf("Left = %d, Right = %d| ", t->type->type_expr.array_info->range.static_range[0], t->type->type_expr.array_info->range.static_range[1]);
            else
                printf("Left = %s, Right = %s| ", t->type->type_expr.array_info->range.dynamic_range[0], t->type->type_expr.array_info->range.dynamic_range[1]);
            // printf("Array Info: %d\n", t->type->type_expr.array_info->range[1]);
            // printf("Array : %d\n", t->type->type_expr.array_info->data_type_e);
            printf("Data Type: %s|\n", t->type->type_expr.array_info->data_type_s);
        }
        else
        {
            // printf("%d|\n", t->type->type_expr.data_type);
            printf("Data Type: %s|\n", terminals_string[t->type->type_expr.data_type]);
        }
        t = t->next;
    }
}

void displayAllArrays()
{
    for (int i = 0; i < collector_count; i++)
    {
        printf("| %s\t  ", all_arrays[i]->module_name);
        printf("| %d - %d\t  ", all_arrays[i]->line_range[0], all_arrays[i]->line_range[1]);
        printf("| %s\t  ", all_arrays[i]->the_array->subject);
        if (all_arrays[i]->the_array->content.array_info->is_static == 3)
        {
            printf("| Static\t  ");
            printf("| [%d,%d]\t  ", all_arrays[i]->the_array->content.array_info->range.static_range[0], all_arrays[i]->the_array->content.array_info->range.static_range[1]);
        }
        else
        {
            printf("| Dynamic\t  ");
            printf("| [%s,%s]\t  ", all_arrays[i]->the_array->content.array_info->range.dynamic_range[0], all_arrays[i]->the_array->content.array_info->range.dynamic_range[1]);
        }
        printf("| %s\n\n", all_arrays[i]->the_array->content.array_info->data_type_s);
    }
}

void displayFunctionSymbolTable(hash_table_holder function_symbol_table, char *func_name, int is_collecting)
{
    if (is_collecting != 1)
    {
        printf("| Function Name: %s |\n", func_name);
        printf("| Unit_Type: %d |\n", function_symbol_table->type);
        if (function_symbol_table->type == FUNCTION_SYMBOL_TABLE)
            printf("| UID: %d |\n", function_symbol_table->UID);
    }

    hash_list *hash_table = function_symbol_table->hash_table;

    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        hash_list hash_list_unit = hash_table[i];
        if (hash_list_unit->head != NULL)
        {
            hash_component hash_component_unit = hash_list_unit->head;
            while (hash_component_unit != NULL)
            {
                if (is_collecting != 1)
                {
                    printf("\n| Variable_name: %s |\n", hash_component_unit->subject);
                    printf("| Scope [Module]: %s |\n", func_name);
                    printf("| Scope Range: [%d,%d] |\n", function_symbol_table->line_range[0], function_symbol_table->line_range[1]);
                }

                switch (hash_component_unit->info)
                {
                    case IP_PRIMITIVE_TYPE:
                    {
                        if(is_collecting!=1)
                        {
                            printf("| Type [IP]: %s |\n", terminals_string[hash_component_unit->content.type]);
                            printf("| isArray: NO |\n");
                            if(hash_component_unit->content.type==INTEGER)
                            {
                                printf("| Width: 4 |\n");
                                function_space+=4;
                            }
                            else if(hash_component_unit->content.type==REAL)
                            {
                                printf("| Width: 8 |\n");
                                function_space+=16;
                            }
                            else if(hash_component_unit->content.type==BOOLEAN)
                            {
                                printf("| Width: 1 |\n");
                                function_space+=4;
                            }
                        }
                        break;
                    }
                    case IP_ARRAY_TYPE:
                    {
                        if(is_collecting!=1)
                        {
                            printf("| Type [IP]: %s |\n", hash_component_unit->content.array_info->data_type_s);
                            printf("| isArray: YES |\n");
                            printf("| Static Status: PURE STATIC |\n");
                            printf("| Range: [%d,%d] |\n", hash_component_unit->content.array_info->range.static_range[0], hash_component_unit->content.array_info->range.static_range[1]);
                            if(hash_component_unit->content.array_info->data_type_e==INTEGER)
                            {
                                printf("| Width: %d |\n", 4*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2);
                                function_space+=4*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2;
                            }
                            else if(hash_component_unit->content.array_info->data_type_e==REAL)
                            {
                                printf("| Width: %d |\n", 8*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2);
                                function_space+=16*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2;
                            }
                            else if(hash_component_unit->content.array_info->data_type_e==BOOLEAN)
                            {
                                printf("| Width: %d |\n", 1*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2);
                                function_space+=4*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2;
                            }
                        }
                        else 
                        {
                            all_arrays[collector_count] = (collection_array)malloc(sizeof(struct collection_array));
                            all_arrays[collector_count]->line_range[0] = function_symbol_table->line_range[0];
                            all_arrays[collector_count]->line_range[1] = function_symbol_table->line_range[1];
                            strcpy(all_arrays[collector_count]->module_name,func_name);
                            all_arrays[collector_count]->the_array = hash_component_unit;
                            collector_count++;
                        }
                        break;
                    }
                    case OP_PRIMITIVE_TYPE:
                    {
                        if(is_collecting!=1)
                        {
                            printf("| Type [OP]: %s |\n", terminals_string[hash_component_unit->content.type]);
                            printf("| isArray: NO |\n");
                            if(hash_component_unit->content.type==INTEGER)
                            {
                                printf("| Width: 4 |\n");
                                function_space+=4;
                            }
                            else if(hash_component_unit->content.type==REAL)
                            {
                                printf("| Width: 8 |\n");
                                function_space+=16;
                            }
                            else if(hash_component_unit->content.type==BOOLEAN)
                            {
                                printf("| Width: 1 |\n");
                                function_space+=4;
                            }
                        }
                        break;
                    }
                    case PRIMITIVE_TYPE:
                    {
                        if(is_collecting!=1)
                        {
                            printf("| Type: %s |\n", terminals_string[hash_component_unit->content.type]);
                            printf("| isArray: NO |\n");
                            if(hash_component_unit->content.type==INTEGER)
                            {
                                printf("| Width: 4 |\n");
                                function_space+=4;
                            }
                            else if(hash_component_unit->content.type==REAL)
                            {
                                printf("| Width: 8 |\n");
                                function_space+=16;
                            }
                            else if(hash_component_unit->content.type==BOOLEAN)
                            {
                                printf("| Width: 1 |\n");
                                function_space+=4;
                            }
                        }
                        break;
                    }
                    case ARRAY_TYPE:
                    {
                        if(is_collecting!=1)
                        {
                            printf("| Type: %s |\n", hash_component_unit->content.array_info->data_type_s);
                            printf("| isArray: YES |\n");
                            if(hash_component_unit->content.array_info->is_static==3)
                            {
                                printf("| Static Status: PURE STATIC |\n");
                                printf("| Range: [%d,%d] |\n", hash_component_unit->content.array_info->range.static_range[0], hash_component_unit->content.array_info->range.static_range[1]);
                                if(hash_component_unit->content.array_info->data_type_e==INTEGER)
                                {
                                    printf("| Width: %d |\n", 4*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2);
                                    function_space+=4*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2;
                                }
                                else if(hash_component_unit->content.array_info->data_type_e==REAL)
                                {
                                    printf("| Width: %d |\n", 8*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2);
                                    function_space+=16*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2;
                                }
                                else if(hash_component_unit->content.array_info->data_type_e==BOOLEAN)
                                {
                                    printf("| Width: %d |\n", 1*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2);
                                    function_space+=4*(hash_component_unit->content.array_info->range.static_range[1]-hash_component_unit->content.array_info->range.static_range[0])+2;
                                }

                            }
                            else if(hash_component_unit->content.array_info->is_static==2)
                            {
                                printf("| Static Status: LEFT STATIC,RIGHT DYNAMIC |\n");
                                printf("| Range: [%s,%s] |\n", hash_component_unit->content.array_info->range.dynamic_range[0], hash_component_unit->content.array_info->range.dynamic_range[1]);
                                printf("| Width [For Pointer to Starting Location]: 2 (Width determined at Runtime) |\n");
                            }
                            else if(hash_component_unit->content.array_info->is_static==1)
                            {
                                printf("| Static Status: LEFT DYNAMIC,RIGHT STATIC |\n");
                                printf("| Range: [%s,%s] |\n", hash_component_unit->content.array_info->range.dynamic_range[0], hash_component_unit->content.array_info->range.dynamic_range[1]);
                                printf("| Width [For Pointer to Starting Location]: 2 (Width determined at Runtime) |\n");
                            }
                            else 
                            {
                                printf("| Static Status: PURE DYNAMIC |\n");
                                printf("| Range: [%s,%s] |\n", hash_component_unit->content.array_info->range.dynamic_range[0], hash_component_unit->content.array_info->range.dynamic_range[1]);
                                printf("| Width [For Pointer to Starting Location]: 2 (Width determined at Runtime) |\n");
                            }
                        }
                        else 
                        {
                            all_arrays[collector_count] = (collection_array)malloc(sizeof(struct collection_array));
                            all_arrays[collector_count]->line_range[0] = function_symbol_table->line_range[0];
                            all_arrays[collector_count]->line_range[1] = function_symbol_table->line_range[1];
                            strcpy(all_arrays[collector_count]->module_name,func_name);
                            all_arrays[collector_count]->the_array = hash_component_unit;
                            collector_count++;
                        }
                    }
                }

                if (is_collecting != 1)
                {
                    printf("| Offset: %d |\n", hash_component_unit->offset);
                    printf("| Nesting Level: %d |\n", function_symbol_table->nesting_level);
                }

                hash_component_unit = hash_component_unit->next;
            }
        }
    }

    for (int i = 0; i < function_symbol_table->num_children; i++)
    {
        displayFunctionSymbolTable(function_symbol_table->children[i], func_name, is_collecting);
    }
    return;
}

void displaySymbolTables(hash_table_holder root, int is_collecting)
{
    if (is_collecting != 1)
    {
        printf("\n--------------------------------------------------------------------------------\n");
        printf("| FUNCTION ROOT TABLE  |\n");
    }

    hash_list *hash_table = root->hash_table;

    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        hash_list hash_list_unit = hash_table[i];
        if (hash_list_unit->head != NULL)
        {
            hash_component hash_component_unit = hash_list_unit->head;
            while (hash_component_unit != NULL)
            {
                if(is_collecting!=1)
                printf("\n--------------------------------------------------------------------------------\n");
                if(hash_component_unit->content.hash_table_unit->isInitialised==1)
                    function_space=0;
                displayFunctionSymbolTable(hash_component_unit->content.hash_table_unit, hash_component_unit->subject,is_collecting);
                all_functions[function_count] = (collection_func)malloc(sizeof(struct collection_func));
                strcpy(all_functions[function_count]->module_name,hash_component_unit->subject);
                all_functions[function_count]->occupied_space = function_space;
                function_count++;

                if(is_collecting!=1)
                printf("\n--------------------------------------------------------------------------------\n");
                hash_component_unit = hash_component_unit->next;
            }
        }
    }
}

void displayHashUnit(hash_table_holder hash_table_unit)
{
    printf("\n--------------------------------------------------------------------------------\n");
    if (hash_table_unit == NULL)
    {
        printf("NULL\n");
        return;
    }
    printf("| Unit Type: %d| ", hash_table_unit->type);
    printf("Self Name: %s| ", hash_table_unit->self_name);
    printf("UID: %d| ", hash_table_unit->UID);
    if (hash_table_unit->parent != NULL)
        printf("Parent Name: %s| ", hash_table_unit->parent->self_name);
    printf("Children: %d| ", hash_table_unit->num_children);
    printf("Line Range: [%d - %d]| ", hash_table_unit->line_range[0], hash_table_unit->line_range[1]);
    printf("Nested Level: %d| ", hash_table_unit->nesting_level);
    printf("Offset: %d|\n\n", hash_table_unit->global_offset);

    printf("[Input Parameters:]\n");
    displayIP_OP_List(hash_table_unit->ip_list);
    printf("\n[Output Parameters:]\n");
    displayIP_OP_List(hash_table_unit->op_list);

    hash_list *hash_table = hash_table_unit->hash_table;
    // int i = 0;
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        hash_list hash_list_unit = hash_table[i];
        if (hash_list_unit->head != NULL)
        {
            hash_component hash_component_unit = hash_list_unit->head;
            while (hash_component_unit != NULL)
            {
                printf("\n| Key = %d| ", i);
                printf("Subject: %s| ", hash_component_unit->subject);
                printf("Info: %d| ", hash_component_unit->info);
                printf("Offset: %d| ", hash_component_unit->offset);
                if (hash_component_unit->info == PRIMITIVE_TYPE ||
                    hash_component_unit->info == IP_PRIMITIVE_TYPE ||
                    hash_component_unit->info == OP_PRIMITIVE_TYPE)
                {
                    printf("Data Type: %d| ", hash_component_unit->content.type);
                }
                else if (hash_component_unit->info == ARRAY_TYPE ||
                         hash_component_unit->info == IP_ARRAY_TYPE)
                {
                    printf("Static[1,4]: %d| ", hash_component_unit->content.array_info->is_static);
                    if (hash_component_unit->content.array_info->is_static == 3)
                        printf("Left = %d, Right = %d| ", hash_component_unit->content.array_info->range.static_range[0], hash_component_unit->content.array_info->range.static_range[1]);
                    else
                        printf("Left = %s, Right = %s| ", hash_component_unit->content.array_info->range.dynamic_range[0], hash_component_unit->content.array_info->range.dynamic_range[1]);
                    printf("Data Type: %s| ", hash_component_unit->content.array_info->data_type_s);
                }
                else if (hash_component_unit->info == FUNCTION_HASH_POINTER)
                {
                    printf("FUNCTION HASH POINTER| ");
                }
                else if (hash_component_unit->info == SCOPE_POINTER)
                {
                    printf("SCOPE POINTER| ");
                }
                printf("\n");
                hash_component_unit = hash_component_unit->next;
            }
        }
    }
    printf("\n--------------------------------------------------------------------------------\n");
    return;
}

/*
int main()
{
    
    hash_table_holder hash_table_unit = (hash_table_holder)malloc(sizeof(struct hash_table_holder));
    hash_table_unit->hash_table = (hash_linked_list*)malloc(sizeof(hash_linked_list)*TABLE_SIZE);
    hash_table_unit->num_children = 0;
    hash_table_unit->ip_list = NULL;
    hash_table_unit->op_list = NULL;
    hash_table_unit->children = NULL;
    // hash_linked_list *hash_table = (hash_linked_list*)malloc(sizeof(hash_linked_list)*TABLE_SIZE);
    int i = 0;
    while(i < TABLE_SIZE)
    {
        hash_table_unit->hash_table[i] = (hash_linked_list)malloc(sizeof(struct hash_linked_list));
        hash_table_unit->hash_table[i]->head = NULL;
        hash_table_unit->hash_table[i]->size = 0;
        i++;
    }

    hash_table_unit->hash_table[0]->head = (hash_node)malloc(sizeof(struct hash_node));
    hash_table_unit->hash_table[0]->head->next = NULL;
    strcpy(hash_table_unit->hash_table[0]->head->subject, "var_1");
    hash_table_unit->hash_table[0]->head->info = PRIMITIVE;
    hash_table_unit->hash_table[0]->head->content.type = INT;
    hash_table_unit->hash_table[0]->size = 1;

    hash_table_unit->hash_table[1]->head = (hash_node)malloc(sizeof(struct hash_node));
    hash_table_unit->hash_table[1]->head->next = NULL;
    strcpy(hash_table_unit->hash_table[1]->head->subject, "var_2");
    hash_table_unit->hash_table[1]->head->info = PRIMITIVE;
    hash_table_unit->hash_table[1]->head->content.type = REAL;
    hash_table_unit->hash_table[1]->size = 1;

    hash_table_unit->hash_table[2]->head = (hash_node)malloc(sizeof(struct hash_node));
    hash_table_unit->hash_table[2]->head->next = NULL;
    strcpy(hash_table_unit->hash_table[2]->head->subject, "func_1");
    hash_table_unit->hash_table[2]->head->info = FUNCTION_HASH;
    hash_table_unit->hash_table[2]->head->content.hash_table_unit = (hash_table_holder)malloc(sizeof(struct hash_table_holder));
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table = (hash_linked_list*)malloc(sizeof(hash_linked_list)*TABLE_SIZE);
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->num_children = 0;
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->ip_list = NULL;
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->op_list = NULL;
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->children = NULL;

    for(int j = 0; j < TABLE_SIZE; j++)
    {
        hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[j] = (hash_linked_list)malloc(sizeof(struct hash_linked_list));
        hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[j]->head = NULL;
        hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[j]->size = 0;
    }

    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[0]->head = (hash_node)malloc(sizeof(struct hash_node));
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[0]->head->next = NULL;
    strcpy(hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[0]->head->subject, "var_3_func_1");
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[0]->head->info = PRIMITIVE;
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[0]->head->content.type = INT;
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[0]->size = 1;

    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[1]->head = (hash_node)malloc(sizeof(struct hash_node));
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[1]->head->next = NULL;
    strcpy(hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[1]->head->subject, "var_4_func_1");
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[1]->head->info = PRIMITIVE;
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[1]->head->content.type = REAL;
    hash_table_unit->hash_table[2]->head->content.hash_table_unit->hash_table[1]->size = 1;

    displayHashTable(0,hash_table_unit);
    
}
*/

/*
    [ROOT Hash Unit with functions ONLY]
         ======================             
        || - ip_list          ||        
        || - op_list          ||        
        || - hash_table       ||        
        || ------------------ ||        
        || |func_1 | -------|-||--------|
        || |                | ||        |
        || |                | ||        |
        || |                | ||        |
        || |                | ||        |
        || |                | ||        |
        || |                | ||        |
        || |                | ||        |
        || ------------------ ||        |
        || - num_children = NA  ||        |
        ||--------------------||        |
        ||                    ||        |               [Table for func_1]
         ======================         |----------> ======================                  
                                                    || - ip_list          ||                                                   
                                                    || - op_list          ||                                                   
                                                    || - hash_table       ||                                                           
                                                    || ------------------ ||                                                           
                                                    || |var_1 |   INT   | ||                                                       
                                                    || |                | ||                                                                    
                                                    || |                | ||                                                                            
                                                    || |                | ||                                                                    
                                                    || |                | ||                                                                    
                                                    || |                | ||                                                                            
                                                    || |                | ||                                                                            
                                                    || |                | ||                                                                                    
                                                    || ------------------ ||                                                                                    
                                                    || - num_children = 1   ||      
                                                    ||--------------------||                                                                                              
                                                    ||         |          ||                                                                                    
                                                     ==========|===========                                                                                                 
                                                               |
    [Nested Scope with Parent func_1]                          |
         ====================== <------------------------------| 
        || - ip_list          ||        
        || - op_list          ||        
        || - hash_table       ||        
        || ------------------ ||        
        || |var_2  |  REAL  | ||
        || |                | ||        
        || |                | ||        
        || |                | ||        
        || |                | ||        
        || |                | ||        
        || |                | ||        
        || |                | ||        
        || ------------------ ||        
        || - num_children = 0   ||
        ||--------------------||        
        ||                    ||        
         ======================

*/