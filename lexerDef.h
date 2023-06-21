/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#include "macros.h"

char *keywords[] = {"integer", "real", "boolean", "of", "array", "start",
                    "end", "declare", "module", "driver", "program", "get_value",
                    "print", "use", "with", "parameters", "true", "false", "takes",
                    "input", "returns", "AND", "OR", "for", "in", "switch", "case",
                    "break", "default", "while"};

enum terminal_token
{
    INTEGER,
    REAL,
    BOOLEAN,
    OF,
    ARRAY,
    START,
    END,
    DECLARE,
    MODULE,
    DRIVER,
    PROGRAM,
    GET_VALUE,
    PRINT,
    USE,
    WITH,
    PARAMETERS,
    TRUE,
    FALSE,
    TAKES,
    INPUT,
    RETURNS,
    AND,
    OR,
    FOR,
    IN,
    SWITCH,
    CASE,
    BREAK,
    DEFAULT,
    WHILE,
    ID,
    NUM,
    RNUM,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LT,
    LE,
    GE,
    GT,
    EQ,
    NE,
    DEF,
    ENDDEF,
    DRIVERDEF,
    DRIVERENDDEF,
    COLON,
    RANGEOP,
    SEMICOL,
    COMMA,
    ASSIGNOP,
    SQBO,
    SQBC,
    BO,
    BC,
    EPSILON,
    DOLLAR,
    ERROR
};
typedef enum terminal_token terminal_token;

enum states
{
    START_STATE,
    ID_STATE,
    NUM_STATE,
    RNUM_STATE,
    MUL_STATE,
    DIV_STATE,
    COMMENT_STATE,
    COMMENT_STATE1,
    SQBC_STATE,
    SQBO_STATE,
    BO_STATE,
    BC_STATE,
    COMMA_STATE,
    SEMICOL_STATE,
    NE1_STATE,
    NE_STATE,
    COLON_STATE,
    PLUS_STATE,
    MINUS_STATE,
    EQ1_STATE,
    EQ_STATE,
    ASSIGNOP_STATE,
    LT_STATE,
    LE_STATE,
    DEF_STATE,
    DRIVERDEF_STATE,
    GT_STATE,
    GE_STATE,
    ENDDEF_STATE,
    DRIVERENDDEF_STATE,
    ERROR_STATE,
    RO1_STATE,
    RO_STATE
};
typedef enum states states;

enum state_type
{
    NON_FINAL,
    FINAL
};
typedef enum state_type state_type;

enum buffer_use_type
{
    BUFFER_1,
    ONE_TWO,
    TWO_ONE,
    BUFFER_2
};
typedef enum buffer_use_type buffer_use_type;

buffer_use_type in_use = BUFFER_1;

union token_content
{
    int valueOfInt;
    double valueOfFloat;
    char identifier[LEXEME_SIZE + 1];
};
typedef union token_content token_content;

/*The token structure*/
struct token_info
{
    enum terminal_token type;
    int lineNum;
    token_content content;
};
typedef struct token_info *token_info;

/*The token linked list at each hash_table index*/
struct hash_node
{
    char keyword[LEXEME_SIZE + 1];
    enum terminal_token keyword_type;
    struct hash_node *next;
};
typedef struct hash_node *hash_node;

/*The hash table*/
struct hash_linked_list
{
    hash_node head;
    int size;
};
typedef struct hash_linked_list *hash_linked_list;
