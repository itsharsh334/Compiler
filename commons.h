/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>

#define VAR_LENGTH 32        /*Maximum length of a variable (terminal/non terminal) name*/
#define GRAMMAR_TABLE_SIZE 128       /*Size of the hash table for Grammar Rules*/
#define NUM_RULES 139       /*Number of rules in the grammar*/
#define NUM_NON_TERMINALS 73 /*Number of non terminals in the grammar*/
#define NUM_TERMINALS 58     /*Number of terminals in the grammar*/

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
    ERROR,
    ERROR_ID
};
typedef enum terminal_token terminal_token;
