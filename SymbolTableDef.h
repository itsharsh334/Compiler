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