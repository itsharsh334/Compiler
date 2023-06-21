/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#include "SymbolTable.c"

enum code_gen_state
{
    ROOT_GEN,               // The overlaying structure of the program
    DRIVER_GEN,             // The driver module
    MODULE_GEN,             // Any other module (this and the above may be combined)
    STATEMENT_MASTER_GEN,   // A linked list of statements
    DECLARE_GEN,            // A variable declaration statement
    ASSIGNMENT_GEN,         // An assignment statement
    PARAM_GEN,              // A parameter list                 
    FOR_GEN,                // A for loop
    WHILE_GEN,              // A while loop
    CASE_GEN,               // A case block in a switch-case conditional
    GETVALUE_GEN,           // An input statement
    PRINT_GEN,              // An output statement
    MODULE_REUSE_GEN,       // Function call
};
typedef enum code_gen_state code_gen_state;

int temp_counter = 1;
int label_counter = 20;
int cunter = 0;
int cunterr = 0;
char prev_state[4][LEXEME_SIZE+1];

char global_type = 'r';

int createTempVarAndReturnOffset(hash_table_holder root, char type);

void code_generation(char quadruple[4][21], FILE* fp_asm);

int computeExpressionValue(ASTNode node, hash_table_holder SymbolTable, FILE* fp);

void recursiveCodeGenerator(ASTNode root, hash_table_holder SymbolTable, code_gen_state state, FILE *fp);

void masterCodeGenerator(ASTNode root, hash_table_holder SymbolTable, char *output_file_name);