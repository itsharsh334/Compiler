/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#define TABLE_SIZE 64
// #define BUFFER_SIZE 128
#define LEXEME_SIZE 20
#define SYMBOL_TABLE_SIZE 128

int lexerError = 0;
int parse_tree_count = 0;
long parse_tree_size = 0;
int ast_count1 = 0;
long ast_size1 = 0;
int switch_error_flag = 0;
int size_declared_parse_tree=0;
int ast_counted=0;
int big_root_created=0;
char *buffer_1;        /* Buffer 1 */
char *buffer_2;        /* Buffer 2 */
char *selected_buffer; /* Buffer currently in use */

int lineNum = 1; /* Line number we are at currently */
