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
#include <stdbool.h>
#include <string.h>
#include "stackADT.c"
#include "macros.h"

/*Descriptions of all functions used in the here*/

/*
- get_token_key_lexer (): Returns the key for the hash table
- construct_hash_table_lexer(): Constructs the hash table
- populate_hash_table_lexer(): Populates the hash table with keywords
- print_hash_table(): Prints the hash table
- check_for_keyword(): Checks if the token is a keyword
- refillBuffer(): Refills the buffer (selection based on the variable in_use)
- fillBuffers(): Fills the buffer at the start of the program
- tokenise(): Tokenises the forward - begin part of the buffer (Selection based on the variable in_use)
- bufferSwitch(): Switches the buffer (Selection based on the variable in_use and the value of forward)
- selectBuffer(): Selects the buffer to be used (Selection based on the variable in_use)
- printBuffer(): Prints the buffers
- printToken(): Prints the token
- isAlphabet(): Checks if the character is an alphabet
- isDigit(): Checks if the character is a digit
- getNextToken(): Returns the next token (using all the functions above)
*/

/*
- +1 is for the null character at the end of the string
*/

int EOF_Flag = 0;    /*End of File*/
int BUFFER_SIZE = 0; /*The size of the buffer*/

/*The list of keywords*/
char *keywords[] = {"integer", "real", "boolean", "of", "array", "start",
                    "end", "declare", "module", "driver", "program", "get_value",
                    "print", "use", "with", "parameters", "true", "false", "takes",
                    "input", "returns", "AND", "OR", "for", "in", "switch", "case",
                    "break", "default", "while"};

char *big_identifier; /*The big identifier in case identifier size exceeds LEXEME_SIZE*/

/*The type of Buffer use*/
/*
- BUFFER_1: Buffer 1 is in use (begin and forward are in Buffer 1)
- ONE_TWO: Buffer 1 has begin and Buffer 2 has forward
- TWO_ONE: Buffer 2 has begin and Buffer 1 has forward
- BUFFER_2: Buffer 2 is in use (begin and forward are in Buffer 2)
*/
enum buffer_use_type
{
    BUFFER_1,
    ONE_TWO,
    TWO_ONE,
    BUFFER_2
};
typedef enum buffer_use_type buffer_use_type;

buffer_use_type in_use = BUFFER_1; /*We start with Buffer 1*/

/*The type of Final States. More to be added*/
enum states
{
    START_STATE,
    ID_STATE,
    NUM_STATE,
    RNUM_STATE,
    RNUMF_STATE,
    RNUM0_STATE,
    RNUM1_STATE,
    RNUM2_STATE,
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
    RO1_STATE,
    RO_STATE
};
typedef enum states states;

/*The type of State*/
enum state_type
{
    NON_FINAL,
    FINAL
};
typedef enum state_type state_type;

/*
- The type of Token
- First 30 are the keywords
*/
/*
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
*/
/*Union to store the content of the token*/
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

int get_token_key_lexer(char *token, int begin, int forward)
{
    /*Self explanatory*/
    long key = 7;
    int scale_factor = 31;
    int i = begin;
    while (i < forward)
    {
        char c = *(token + i);
        key = key * scale_factor + c;
        i++;
    }
    if (key < 0)
        key = -key;
    return key % 64;
}

hash_linked_list *construct_hash_table_lexer()
{
    // Self explanatory
    hash_linked_list *hash_table = (hash_linked_list *)malloc(sizeof(hash_linked_list) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        hash_table[i] = (hash_linked_list)malloc(sizeof(struct hash_linked_list));
        hash_table[i]->head = NULL;
        hash_table[i]->size = 0;
    }
    return hash_table;
}

hash_linked_list *populate_hash_table_lexer(hash_linked_list *hash_table)
{
    for (int i = 0; i < 30; i++) // 30 is the number of keywords
    {
        int key = get_token_key_lexer(keywords[i], 0, strlen(keywords[i]));

        hash_node new_node = (hash_node)malloc(sizeof(struct hash_node));
        strcpy(new_node->keyword, keywords[i]);
        new_node->keyword_type = i;
        new_node->next = NULL;

        if (hash_table[key]->head == NULL)
        {
            hash_table[key]->head = new_node;
            hash_table[key]->size++;
        }
        else
        {
            hash_node t = hash_table[key]->head;
            while (t->next != NULL)
            {
                t = t->next;
            }
            t->next = new_node;
            hash_table[key]->size++;
        }
    }
    return hash_table;
}

void print_hash_table(hash_linked_list *hash_table)
{
    /*Self explanatory*/
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (hash_table[i]->size == 0)
            continue;

        printf("Key: %d | Size: %d\n", i, hash_table[i]->size);
        hash_node t = hash_table[i]->head;
        while (t != NULL)
        {
            printf("Keyword: %s | Type: %d\n", t->keyword, t->keyword_type);
            t = t->next;
        }
        printf("\n");
    }
}

int check_for_keyword(char *token, int begin, int forward, hash_linked_list *hash_table)
{
    int key = get_token_key_lexer(token, begin, forward);
    hash_node t = hash_table[key]->head; /*Go to the head of the linked list at the index*/
    while (t != NULL)
    {
        if (strcmp(t->keyword, token) == 0)
        {
            return t->keyword_type;
        }
        t = t->next; /*Go to the next node*/
    }
    return -1;
}

void refillBuffer(FILE *fp)
{
    /*
    - If buffer 1 is in use, refill buffer 2 (ensure that this does not happen right after start when both buffers are sufficiently filled)
    - If buffer 2 is in use, refill buffer 1
    */
    if (in_use == BUFFER_2)
    {
        char c;
        int i = 0;
        while (!feof(fp) && i < BUFFER_SIZE)
        {
            c = fgetc(fp);
            buffer_1[i] = c;
            i++;
        }
        if (c == EOF)
            buffer_1[i] = '\0';
        return;
    }
    else
    {
        char c;
        int i = 0;
        while (!feof(fp) && i < BUFFER_SIZE)
        {
            c = fgetc(fp);
            buffer_2[i] = c;
            i++;
        }
        if (c == EOF)
            buffer_2[i] = '\0';
        return;
    }
}

void fillBuffers(FILE *fp)
{
    /*
    - Only to be called at the start of the program
    - Fill both buffers
    */
    char c;
    int i = 0;
    while (!feof(fp) && i < BUFFER_SIZE)
    {
        c = fgetc(fp);
        buffer_1[i] = c;
        i++;
    }
    if (c == EOF)
        buffer_1[i] = '\0';

    i = 0;
    while (!feof(fp) && i < BUFFER_SIZE)
    {
        c = fgetc(fp);
        buffer_2[i] = c;
        i++;
    }
    if (c == EOF)
        buffer_2[i] = '\0';
    return;
}

FILE *getStream(FILE *fp)
{
    fillBuffers(fp);
    return fp;
}

token_info tokenise(int begin, int forward, terminal_token type, int lineNum, hash_linked_list *hash_table)
{
    token_info token;
    token = (token_info)malloc(sizeof(struct token_info));

    /*
    - If either Buffer 1 or Buffer 2 is in use, the token is in one buffer only
    - If both Buffer 1 and Buffer 2 are in use, the token is in both buffers
    */
    int length = 0;
    if (in_use == BUFFER_1 || in_use == BUFFER_2)
    {
        length = forward - begin;
    }
    else
    {
        length = forward + (BUFFER_SIZE - begin);
    }

    char token_string[length + 1];

    token->lineNum = lineNum;

    /*Collect the token from the buffer(s) based on in_use*/
    if (in_use == BUFFER_1)
    {
        for (int i = begin; i < forward; i++)
        {
            token_string[i - begin] = buffer_1[i];
        }
        token_string[forward - begin] = '\0';
    }
    else if (in_use == ONE_TWO)
    {
        // begin starts in buffer 1 and forward in buffer 2
        int i = begin;
        while (i < BUFFER_SIZE)
        {
            token_string[i - begin] = buffer_1[i];
            i++;
        }
        i = 0;
        while (i < forward)
        {
            token_string[i + BUFFER_SIZE - begin] = buffer_2[i];
            i++;
        }
        token_string[forward + BUFFER_SIZE - begin] = '\0';
    }
    else if (in_use == TWO_ONE)
    {
        // begin starts in buffer 2 and forward in buffer 1
        int i = begin;
        while (i < BUFFER_SIZE)
        {
            token_string[i - begin] = buffer_2[i];
            i++;
        }
        i = 0;
        while (i < forward)
        {
            token_string[i + BUFFER_SIZE - begin] = buffer_1[i];
            i++;
        }
        token_string[forward + BUFFER_SIZE - begin] = '\0';
    }
    else
    {
        for (int i = begin; i < forward; i++)
        {
            token_string[i - begin] = buffer_2[i];
        }
        token_string[forward - begin] = '\0';
    }

    if (length > LEXEME_SIZE)
    {
        big_identifier = malloc(sizeof(char) * (length + 1));
        strcpy(big_identifier, token_string);
        printf("Error: \"%s\" Size exceeds %d | Line: %d\n", big_identifier, LEXEME_SIZE, lineNum);
        token->type = ERROR_ID;
        token->lineNum = lineNum;
        strcpy(token->content.identifier, "size exceeded");
        // printf("ERROR: %s | line: %d | Type: %d\n", big_identifier, token->lineNum, token->type);
        // free(big_identifier);
        return token;
    }

    /*
    - If the token is an ID, check if it is a keyword and tokenise accordingly
    - If the token is a NUM, tokenise it as an integer
    - If the token is a RNUM, tokenise it as a float
    - If the token is neither of the above, tokenise it genrically (Example: +, <=, etc.)
    - Note that we store the "+", "<=", etc. as strings in the token_content.identifier field
    */
    int res;
    if (type == ID)
    {
        // printf("token_string: %s\n", token_string);
        res = check_for_keyword(token_string, 0, length, hash_table);
        if (res != -1)
        {
            token->type = res;
            strcpy(token->content.identifier, token_string);
            return token;
        }
        else
        {
            token->type = ID;
            strcpy(token->content.identifier, token_string);
            return token;
        }
    }
    else if (type == NUM)
    {
        token->type = NUM;
        token->content.valueOfInt = atoi(token_string);
        return token;
    }
    else if (type == RNUM)
    {
        token->type = RNUM;
        token->content.valueOfFloat = atof(token_string);
        return token;
    }
    else
    {
        token->type = type;
        strcpy(token->content.identifier, token_string);
        return token;
    }
}

void bufferSwitch(int *begin, int *forward, FILE *fp, int flag)
{
    /*
    Switching Mechanism:
    - If Buffer 1 was in use and forward has reached the end of the buffer, we use Buffer 1 and Buffer 2 (begin in Buffer 1, forward in Buffer 2)
    - If Buffer 2 was in use and forward has reached the end of the buffer, we use Buffer 2 and Buffer 1 (begin in Buffer 2, forward in Buffer 1)
    - If both were in use and begin was in Buffer 1 and forward was in Buffer 2, we swittch to Buffer 2 and refill Buffer 1
    - If both were in use and begin was in Buffer 2 and forward was in Buffer 1, we swittch to Buffer 1 and refill Buffer 2
    - The flag is used to indicate that the bufferSwitch function was called after tokenisation (begin=forward)
        and it's NOT a coincidence (Example: begin = 3 (in Buffer 1) AND forward = 3 (in Buffer 2)
    */
    if (in_use == BUFFER_1)
    {
        if (*forward == BUFFER_SIZE)
        {
            *forward = 0;
            in_use = ONE_TWO;

            if (*begin == BUFFER_SIZE)
            {
                *begin = 0;
                in_use = BUFFER_2;
                refillBuffer(fp);
            }
            return;
        }
    }
    else if (in_use == BUFFER_2)
    {
        if (*forward == BUFFER_SIZE)
        {
            *forward = 0;
            in_use = TWO_ONE;

            if (*begin == BUFFER_SIZE)
            {
                *begin = 0;
                in_use = BUFFER_1;
                refillBuffer(fp);
            }
            return;
        }
    }
    else if (in_use == ONE_TWO)
    {
        if (*begin == *forward && flag == 1)
        {
            in_use = BUFFER_2;
            refillBuffer(fp);
            return;
        }
    }
    else
    {
        if (*begin == *forward && flag == 1)
        {
            in_use = BUFFER_1;
            refillBuffer(fp);
            return;
        }
    }
}

void selectBuffer()
{
    /*
    - Select the buffer to be used for reading the next character
    - If in_use is BUFFER_1 or TWO_ONE, we use Buffer 1 as forward is in Buffer 1
    - If in_use is BUFFER_2 or ONE_TWO, we use Buffer 2 as forward is in Buffer 2
    */
    if (in_use == BUFFER_1 || in_use == TWO_ONE)
    {
        strcpy(selected_buffer, buffer_1);
    }
    else
    {
        strcpy(selected_buffer, buffer_2);
    }
}

void printBuffer()
{
    // Self Explanatory
    printf("\nBuffer 1: %s\n", buffer_1);
    printf("Buffer 2: %s", buffer_2);
    printf("\n\n");
    return;
}

void printToken(token_info token)
{
    // Self Explanatory
    if (token->type == ERROR || token->type == ERROR_ID)
    {
        // printf("Line_number: %d | Lexeme: %s \n", token->lineNum, token->content.identifier);
    }
    else if (token->type == NUM)
    {
        printf("Line_number: %d | Lexeme: %d | Token_name: %s\n", token->lineNum, token->content.valueOfInt, terminals_string[token->type]);
    }
    else if (token->type == RNUM)
    {
        printf("Line_number: %d | Lexeme: %lf | Token_name: %s\n", token->lineNum, token->content.valueOfFloat, terminals_string[token->type]);
    }
    else if (token->type == ID)
    {
        printf("Line_number: %d | Lexeme: %s | Token_name: %s\n", token->lineNum, token->content.identifier, terminals_string[token->type]);
    }
    else
    {
        printf("Line_number: %d | Lexeme: %s | Token_name: %s\n", token->lineNum, token->content.identifier, terminals_string[token->type]);
    }
    return;
}

int isAlphabet(char c)
{
    // Self Explanatory
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
    {
        return 1;
    }
    return 0;
}

int isDigit(char c)
{
    // Self Explanatory
    if (c >= '0' && c <= '9')
    {
        return 1;
    }
    return 0;
}

void removeComments(char *testcaseFile, char *cleanFile)
{
    FILE *fp1 = fopen(testcaseFile, "r");
    FILE *fp2 = fopen(cleanFile, "w");
    // char input[4096];
    int in_size = 1024;
    char *input = (char *)(malloc(in_size * sizeof(char)));

    int i = 0;

    while (!feof(fp1))
    {
        input[i] = fgetc(fp1);
        i++;

        if (i >= in_size)
        {
            in_size *= 2;
            input = (char *)(realloc(input, in_size * sizeof(char)));
        }
    }

    int within_comment = 0;
    for (int i = 0; i < in_size; i++)
    {
        if (input[i] == EOF)
        {
            break;
        }
        else if (within_comment == 0)
        {
            if (input[i] == '*' && input[i + 1] == '*')
            {
                within_comment = 1;
                i++;
            }

            else
            {
                fputc(input[i], fp2);
            }
        }
        else
        {
            if (input[i] == '\n')
            {
                fputc(input[i], fp2);
            }
            if (input[i] == '*' && input[i + 1] == '*')
            {
                within_comment = 0;
                i++;
            }
        }
    }

    fclose(fp1);
    fclose(fp2);

    // Print the contents of the cleanFile
    fp2 = fopen(cleanFile, "r");

    // printf("In Size: %d\n", in_size);
    printf("[File with Comments Removed]\n\n");
    char c;
    while (!feof(fp2))
    {
        c = fgetc(fp2);
        if (c != EOF)
            printf("%c", c);
    }
    printf("\n\n");
    return;
}

token_info getNextToken(FILE *fp, hash_linked_list *hash_table, int *begin, int *forward)
{
    char c;

    int current_state = START_STATE; /* Current state of the DFA */
    state_type st_type = NON_FINAL;  /* Type of the current state */
    int has_complete_token = 0;      /* Flag to indicate if a complete token has been formed */
    int scanned_comment = 0;         /* Flag to indicate if a comment has been scanned */

    selectBuffer(); /* Select the buffer to be used for reading the next character */

    token_info token;
    token = (token_info)malloc(sizeof(struct token_info));

    int counter_for_id_length = 0;
    while (!has_complete_token)
    {
        switch (current_state)
        {
        case START_STATE:
            st_type = NON_FINAL;           /* Set the state type */
            c = selected_buffer[*forward]; /* Read the next character */
            // printf("c=%c, begin=%d, forward=%d, in_use =%d\n", c, *begin, *forward, in_use);

            *begin = *forward;
            if (scanned_comment == 1)
            {
                scanned_comment = 0;
                bufferSwitch(begin, forward, fp, 1);
                selectBuffer();
            }

            // printf("c=%c, begin=%d, forward=%d \n", c,*begin,*forward);

            if (isAlphabet(c) || c == '_')
            {
                // counter_for_id_length++;
                current_state = ID_STATE;
            }
            else if (isDigit(c))
            {
                current_state = NUM_STATE;
            }
            else if (c == ' ' || c == '\t' || c == '\r')
            {
                ++(*begin); /* Ignore the space */
                //(*forward)++;
            }
            else if (c == '*')
            {
                current_state = MUL_STATE;
            }
            else if (c == '/')
            {
                current_state = DIV_STATE;
            }
            else if (c == '[')
            {
                current_state = SQBO_STATE;
            }
            else if (c == ']')
            {
                current_state = SQBC_STATE;
            }
            else if (c == ')')
            {
                current_state = BC_STATE;
            }
            else if (c == '(')
            {
                current_state = BO_STATE;
            }
            else if (c == ',')
            {
                current_state = COMMA_STATE;
            }
            else if (c == ';')
            {
                current_state = SEMICOL_STATE;
            }
            else if (c == '!')
            {
                current_state = NE1_STATE;
            }
            else if (c == ':')
            {
                current_state = COLON_STATE;
            }
            else if (c == '+')
            {
                current_state = PLUS_STATE;
            }
            else if (c == '-')
            {
                current_state = MINUS_STATE;
            }
            else if (c == '=')
            {
                current_state = EQ1_STATE;
            }
            else if (c == '<')
            {
                current_state = LT_STATE;
            }
            else if (c == '>')
            {
                current_state = GT_STATE;
            }
            else if (c == '.')
            {
                current_state = RO1_STATE;
            }
            else if (c == '\n')
            {
                lineNum++;
                ++(*begin);
            }
            else
            {
                if (c == EOF)
                {
                    // printf("\nEND OF FILE | LINE NUMBER %d.\n", lineNum);
                    EOF_Flag = 1;
                    return NULL;
                }
                else
                {
                    printf("Lexical Error: Character used is outside the language's defined alphabet. Character used is %c.\nKindly use characters belonging to:\n\t => {a-z|A-Z|_|0-9}\n\t => {+|-|*|/|<|>|=|!}\n\t => {:|.|,|;|[|]|(|)}\n", c);
                    token = tokenise(*begin, *forward, ERROR, lineNum, hash_table);
                    (*forward)++;
                    *begin = *forward;
                    lexerError = 1;
                    has_complete_token = 1;
                    return token;
                }
            }

            break;

        case ID_STATE:
            st_type = FINAL;               /* Set the state type */
            c = selected_buffer[*forward]; /* Read the next character */

            // printf("c=%c, begin=%d, forward=%d \n", c,*begin,*forward);

            if (isAlphabet(c) || isDigit(c) || c == '_')
            {
                counter_for_id_length++;
                current_state = ID_STATE;
            }
            else
            {
                // if (c == '\n')
                //     lineNum++;

                if (st_type == FINAL)
                {
                    // if (counter_for_id_length <= 20)
                    // {
                    token = tokenise(*begin, *forward, ID, lineNum, hash_table); /* Tokenise the string */
                    has_complete_token = 1;                                      /* Set the flag */
                    *begin = *forward;                                           /* Set begin to forward */
                    return token;                                                /* Return the token */
                    // }
                    // else
                    // {
                    //     int length = 0;
                    //     if (in_use == BUFFER_1 || in_use == BUFFER_2)
                    //     {
                    //         length = *forward - *begin;
                    //     }
                    //     else
                    //     {
                    //         length = *forward + (BUFFER_SIZE - *begin);
                    //     }

                    //     char token_string[length + 1];

                    //     // token->lineNum = lineNum;

                    //     /*Collect the token from the buffer(s) based on in_use*/
                    //     if (in_use == BUFFER_1)
                    //     {
                    //         for (int i = *begin; i < *forward; i++)
                    //         {
                    //             token_string[i - *begin] = buffer_1[i];
                    //         }
                    //         token_string[*forward - *begin] = '\0';
                    //     }
                    //     else if (in_use == ONE_TWO)
                    //     {
                    //         // begin starts in buffer 1 and forward in buffer 2
                    //         int i = *begin;
                    //         while (i < BUFFER_SIZE)
                    //         {
                    //             token_string[i - *begin] = buffer_1[i];
                    //             i++;
                    //         }
                    //         i = 0;
                    //         while (i < *forward)
                    //         {
                    //             token_string[i + BUFFER_SIZE - *begin] = buffer_2[i];
                    //             i++;
                    //         }
                    //         token_string[*forward + BUFFER_SIZE - *begin] = '\0';
                    //     }
                    //     else if (in_use == TWO_ONE)
                    //     {
                    //         // begin starts in buffer 2 and forward in buffer 1
                    //         int i = *begin;
                    //         while (i < BUFFER_SIZE)
                    //         {
                    //             token_string[i - *begin] = buffer_2[i];
                    //             i++;
                    //         }
                    //         i = 0;
                    //         while (i < *forward)
                    //         {
                    //             token_string[i + BUFFER_SIZE - *begin] = buffer_1[i];
                    //             i++;
                    //         }
                    //         token_string[*forward + BUFFER_SIZE - *begin] = '\0';
                    //     }
                    //     else
                    //     {
                    //         for (int i = *begin; i < *forward; i++)
                    //         {
                    //             token_string[i - *begin] = buffer_2[i];
                    //         }
                    //         token_string[*forward - *begin] = '\0';
                    //     }
                    //     printf("Error: ID length exceeded 20 in line number %d. The id lexeme is %s of length %d.\n", lineNum, token_string, counter_for_id_length);
                    //     current_state = START_STATE;
                    //     (*forward)--;
                    //     lexerError = 1;
                    //     has_complete_token = 1;
                    // }
                }
                else
                {
                    // THIS ERROR WILL NEVER OCCUR!! XD
                }
            }
            break;

        case NUM_STATE:
            st_type = FINAL;               /* Set the state type */
            c = selected_buffer[*forward]; /* Read the next character */

            if (isDigit(c))
            {
                current_state = NUM_STATE;
            }
            else
            {
                if (c == '.')
                {
                    current_state = RNUM0_STATE;
                }

                if (current_state == NUM_STATE)
                {
                    token = tokenise(*begin, *forward, NUM, lineNum, hash_table); /* Tokenise the string */
                    has_complete_token = 1;                                       /* Set the flag */
                    *begin = *forward;                                            /* Set begin to forward */
                    return token;                                                 /* Return the token */
                }
            }
            break;

        case RNUM0_STATE:
            st_type = NON_FINAL;           /* Set the state type */
            c = selected_buffer[*forward]; /* Read the next character */

            if (isDigit(c))
            {
                current_state = RNUM_STATE;
            }
            else
            {
                if (c == '.')
                {
                    // RETRACTION
                    // (*forward)--;

                    if ((*forward) == 0)
                    {
                        if (in_use == ONE_TWO)
                        {
                            (*forward) = BUFFER_SIZE - 1;
                            in_use = BUFFER_1;
                        }
                        if (in_use == TWO_ONE)
                        {
                            (*forward) = BUFFER_SIZE - 1;
                            in_use = BUFFER_2;
                        }
                    }
                    else
                    {
                        (*forward)--;
                    }

                    current_state = START_STATE;
                    token = tokenise(*begin, *forward, NUM, lineNum, hash_table); /* Tokenise the string */
                    has_complete_token = 1;                                       /* Set the flag */
                    *begin = *forward;                                            /* Set begin to forward */
                    return token;
                }

                if (current_state == RNUM0_STATE)
                {
                    if (c == EOF)
                        printf("\nLexical Error: Expected [0..9] after '[.]' in line number %d. Received '\\n'.\n", lineNum); // Error message
                    printf("\nLexical Error: Expected [0..9] after '[.]' for RNUM in line number %d. Received '%c'.\n", lineNum, c);
                    token = tokenise(*begin, *forward, ERROR, lineNum, hash_table);
                    // (*forward)--;
                    *begin = *forward;
                    lexerError = 1;
                    has_complete_token = 1;
                    return token;
                }
            }
            break;

        case RNUM_STATE:
            st_type = FINAL;               /* Set the state type */
            c = selected_buffer[*forward]; /* Read the next character */

            if (isDigit(c))
            {
                current_state = RNUM_STATE;
            }
            else
            {
                if (c == 'E' || c == 'e')
                {
                    current_state = RNUM1_STATE;
                }

                if (current_state == RNUM_STATE)
                {
                    token = tokenise(*begin, *forward, RNUM, lineNum, hash_table); /* Tokenise the string */
                    has_complete_token = 1;                                        /* Set the flag */
                    *begin = *forward;                                             /* Set begin to forward */
                    return token;                                                  /* Return the token */
                }
            }
            break;

        case RNUM1_STATE:
            st_type = NON_FINAL;           /* Set the state type */
            c = selected_buffer[*forward]; /* Read the next character */

            if (isDigit(c))
            {
                current_state = RNUMF_STATE;
            }
            else
            {
                if (c == '+' || c == '-')
                {
                    current_state = RNUM2_STATE;
                }

                if (current_state == RNUM1_STATE)
                {
                    if (c == EOF)
                        printf("\nLexical Error: Expected [0..9] or [+|-] after '[e|E]' in line number %d. Received '\\n'.\n", lineNum); // Error message
                    printf("\nLexical Error: Expected [0..9] or [+|-] after '[e|E]' for RNUM in line number %d. Received '%c'.\n", lineNum, c);
                    token = tokenise(*begin, *forward, ERROR, lineNum, hash_table);
                    // (*forward)--;
                    *begin = *forward;
                    lexerError = 1;
                    has_complete_token = 1;
                    return token;
                }
            }
            break;

        case RNUM2_STATE:
            st_type = NON_FINAL;           /* Set the state type */
            c = selected_buffer[*forward]; /* Read the next character */

            if (isDigit(c))
            {
                current_state = RNUMF_STATE;
            }
            else
            {
                if (current_state == RNUM2_STATE)
                {
                    if (c == EOF)
                        printf("\nLexical Error: Expected [0..9] after '[+|_]' in line number %d. Received '\\n'.\n", lineNum); // Error message
                    printf("\nLexical Error: Expected [0..9] after '[+|_]' for RNUM in line number %d. Received '%c'.\n", lineNum, c);
                    token = tokenise(*begin, *forward, ERROR, lineNum, hash_table);
                    // (*forward)--;
                    *begin = *forward;
                    lexerError = 1;
                    has_complete_token = 1;
                    return token;
                }
            }
            break;

        case RNUMF_STATE:
            st_type = FINAL;               /* Set the state type */
            c = selected_buffer[*forward]; /* Read the next character */

            if (isDigit(c))
            {
                current_state = RNUMF_STATE;
            }
            else
            {
                if (current_state == RNUMF_STATE)
                {
                    token = tokenise(*begin, *forward, RNUM, lineNum, hash_table); /* Tokenise the string */
                    has_complete_token = 1;                                        /* Set the flag */
                    *begin = *forward;                                             /* Set begin to forward */
                    return token;                                                  /* Return the token */
                }
            }
            break;

        case MUL_STATE:
            st_type = FINAL;
            c = selected_buffer[*forward];

            // printf("c=%c, begin=%d, forward=%d, in_use=%d\n", c, *begin, *forward, in_use);

            if (c == '*')
            {
                current_state = COMMENT_STATE;
                (*forward)++;
                *begin = *forward;
                bufferSwitch(begin, forward, fp, 1);
                selectBuffer();
                continue;
            }
            else
            {
                token = tokenise(*begin, *forward, MUL, lineNum, hash_table);
                has_complete_token = 1;
                *begin = *forward;
                return token;
            }

            break;

        case DIV_STATE:
            st_type = FINAL;

            token = tokenise(*begin, *forward, DIV, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case SQBC_STATE:
            st_type = FINAL;

            token = tokenise(*begin, *forward, SQBC, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case SQBO_STATE:
            st_type = FINAL;

            token = tokenise(*begin, *forward, SQBO, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;
            break;

        case BC_STATE:
            st_type = FINAL;

            token = tokenise(*begin, *forward, BC, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;
            break;

        case BO_STATE:
            st_type = FINAL;

            token = tokenise(*begin, *forward, BO, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;
            break;

        case COMMA_STATE:
            st_type = FINAL;

            token = tokenise(*begin, *forward, COMMA, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;
            break;

        case SEMICOL_STATE:
            st_type = FINAL;

            token = tokenise(*begin, *forward, SEMICOL, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;
            break;

        case NE1_STATE:
            // printf("in ne_1 state");
            st_type = NON_FINAL;
            has_complete_token = 0;
            c = selected_buffer[*forward];
            if (c == '=')
            {
                // printf("state changed from ne1 to ne");
                current_state = NE_STATE;
            }
            else
            {
                c = selected_buffer[*forward];
                if (c == EOF)
                    printf("Lexical Error: Expected '=' after '!' in line number %d. Received '\\n'.\n", lineNum); // Error message
                printf("Lexcial Error: Expected '=' after '!' in line number %d. Received '%c'.\n", lineNum, c);
                token = tokenise(*begin, *forward, ERROR, lineNum, hash_table);
                // (*forward)--;
                *begin = *forward;
                lexerError = 1;
                has_complete_token = 1;
                return token;
            }
            break;

        case NE_STATE:
            // printf("in ne state");
            st_type = FINAL;
            token = tokenise(*begin, *forward, NE, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;
            break;
        case RO1_STATE:
            // printf("in ne_1 state");
            st_type = NON_FINAL;
            has_complete_token = 0;
            c = selected_buffer[*forward];
            if (c == '.')
            {
                // printf("state changed from ne1 to ne");
                current_state = RO_STATE;
            }
            else
            {
                c = selected_buffer[*forward];
                if (c == EOF)
                    printf("Lexical Error: Expected '.' after '.' in line number %d. Received '\\n'.\n", lineNum); // Error message
                printf("Lexical Error: Expected '.' after '.' for range operator in line number %d. Received '%c'.\n", lineNum, c);
                token = tokenise(*begin, *forward, ERROR, lineNum, hash_table);
                // (*forward)--;
                *begin = *forward;
                lexerError = 1;
                has_complete_token = 1;
                return token;
            }
            break;

        case RO_STATE:
            // printf("in ne state");
            st_type = FINAL;
            token = tokenise(*begin, *forward, RANGEOP, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;
            break;
        case COMMENT_STATE:
            has_complete_token = 0;
            st_type = NON_FINAL;
            c = selected_buffer[*forward];

            // printf("c=%c, begin=%d, forward=%d, in_use=%d\n", c,*begin,*forward,in_use);
            if (c == '*')
            {
                current_state = COMMENT_STATE1;
                (*forward)++;
                *begin = *forward;
                bufferSwitch(begin, forward, fp, 1);
                selectBuffer();
                continue;
            }
            else if (c == '\n')
            {
                lineNum++;
                (*forward)++;
                *begin = *forward;
                bufferSwitch(begin, forward, fp, 1);
                selectBuffer();
                continue;
            }
            else
            {
                current_state = COMMENT_STATE;
                (*forward)++;
                *begin = *forward;
                bufferSwitch(begin, forward, fp, 1);
                selectBuffer();
                continue;
                // ignore the character, because it is within a comment
            }
            break;

        case COMMENT_STATE1:
            has_complete_token = 0;
            st_type = NON_FINAL;
            c = selected_buffer[*forward];

            // printf("c=%c, begin=%d, forward=%d, in_use=%d\n", c,*begin,*forward,in_use);
            if (c == '*')
            {
                // *begin = *forward;
                current_state = START_STATE;
                scanned_comment = 1;
            }
            else
            {
                current_state = COMMENT_STATE;
                (*forward)++;
                *begin = *forward;
                bufferSwitch(begin, forward, fp, 1);
                selectBuffer();
                continue;
            }
            break;

        case COLON_STATE:
            if (selected_buffer[*forward] == '=')
            {
                current_state = ASSIGNOP_STATE; // Check if forward pointer has '=' symbol, else tokenise.
            }
            else
            {
                st_type = FINAL;
                token = tokenise(*begin, *forward, COLON, lineNum, hash_table);
                has_complete_token = 1;
                *begin = *forward;
                return token;
            }

            break;

        case ASSIGNOP_STATE:
            st_type = FINAL;
            token = tokenise(*begin, *forward, ASSIGNOP, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case MINUS_STATE:
            st_type = FINAL;
            token = tokenise(*begin, *forward, MINUS, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case PLUS_STATE:
            st_type = FINAL;
            token = tokenise(*begin, *forward, PLUS, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case LT_STATE:
            if (selected_buffer[*forward] == '=')
            { // Check if forward pointer has '=' symbol; if not, then check for '<'; else tokenise.
                current_state = LE_STATE;
            }
            else if (selected_buffer[*forward] == '<')
            {
                current_state = DEF_STATE;
            }
            else
            {
                st_type = FINAL;
                token = tokenise(*begin, *forward, LT, lineNum, hash_table);
                has_complete_token = 1;
                *begin = *forward;
                return token;
            }

            break;

        case LE_STATE:
            st_type = FINAL;
            token = tokenise(*begin, *forward, LE, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case DEF_STATE:
            if (selected_buffer[*forward] == '<')
            { // Check if forward pointer has '<' symbol; else tokenise.
                current_state = DRIVERDEF_STATE;
            }
            else
            {
                st_type = FINAL;
                token = tokenise(*begin, *forward, DEF, lineNum, hash_table);
                has_complete_token = 1;
                *begin = *forward;
                return token;
            }

            break;

        case DRIVERDEF_STATE:
            st_type = FINAL;
            token = tokenise(*begin, *forward, DRIVERDEF, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case GT_STATE:
            if (selected_buffer[*forward] == '=')
            { // Check if forward pointer has '=' symbol; if not, then check for '>'; else tokenise.
                current_state = GE_STATE;
            }
            else if (selected_buffer[*forward] == '>')
            {
                current_state = ENDDEF_STATE;
            }
            else
            {
                st_type = FINAL;
                token = tokenise(*begin, *forward, GT, lineNum, hash_table);
                has_complete_token = 1;
                *begin = *forward;
                return token;
            }

            break;

        case GE_STATE:
            st_type = FINAL;
            token = tokenise(*begin, *forward, GE, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case ENDDEF_STATE:
            if (selected_buffer[*forward] == '>')
            { // Check if forward pointer has '>' symbol; else tokenise.
                current_state = DRIVERENDDEF_STATE;
            }
            else
            {
                st_type = FINAL;
                token = tokenise(*begin, *forward, ENDDEF, lineNum, hash_table);
                has_complete_token = 1;
                *begin = *forward;
                return token;
            }

            break;

        case DRIVERENDDEF_STATE:
            st_type = FINAL;
            token = tokenise(*begin, *forward, DRIVERENDDEF, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        case EQ1_STATE:
            st_type = NON_FINAL; // Check if forward pointer has '=' symbol; else tokenise erroneous lexemes.
            if (selected_buffer[*forward] == '=')
            {
                current_state = EQ_STATE;
            }

            else
            {
                c = selected_buffer[*forward]; // Error message
                if (c == EOF)
                    printf("Lexical Error: Expected '=' after '=' in line number %d. Received '\\n'.\n", lineNum);
                printf("Lexcial Error: Expected '=' after '=' in line number %d. Received '%c'.\n", lineNum, c);
                token = tokenise(*begin, *forward, ERROR, lineNum, hash_table);
                // (*forward)--;
                *begin = *forward;
                lexerError = 1;
                has_complete_token = 1;
                return token;
            }

            break;

        case EQ_STATE:
            token = tokenise(*begin, *forward, EQ, lineNum, hash_table);
            has_complete_token = 1;
            *begin = *forward;
            return token;

            break;

        default:
            break;
        }
        if (EOF_Flag == 1)
        {
            break;
        }
        (*forward)++;                        /* Increment forward */
        bufferSwitch(begin, forward, fp, 0); /* Switch the buffers if required */
        selectBuffer();                      /* Select the buffer to be used for reading the next character */
    }
    return token;
}
/*
int main(void)
{
    BUFFER_SIZE = 128;

    buffer_1 = (char *)malloc((BUFFER_SIZE+1) * sizeof(char));
    buffer_2 = (char *)malloc((BUFFER_SIZE+1) * sizeof(char));
    selected_buffer = (char *)malloc((BUFFER_SIZE+1) * sizeof(char));

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    hash_linked_list *hash_table = construct_hash_table_lexer();
    hash_table = populate_hash_table_lexer(hash_table);
    // print_hash_table(hash_table);

    // removeComments("sample.txt", "clean.txt");

    FILE *fp = fopen("t2.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    fillBuffers(fp);
    printBuffer();

    int begin = 0;
    int forward = 0;

    while (EOF_Flag == 0)
    {
        token_info token8 = getNextToken(fp, hash_table, &begin, &forward);
        // printBuffer();

        if(EOF_Flag==1)
            break;
        // printf("Ok\n");
        if (token8->type == ERROR_ID)
        {
            // printf("Error: \"%s\" Size exceeds %d | Line: %d\n", big_identifier, LEXEME_SIZE, lineNum);
            // token8=tokenise();
            // token8 = (token_info)malloc(sizeof(struct token_info));
            // token8->type = ERROR;
            // token8->lineNum = lineNum;
            printf("ERROR: %s | line: %d | Type: %d\n", big_identifier, token8->lineNum, token8->type);
            free(big_identifier);
        }
        else if (EOF_Flag == 0)
            printToken(token8);
        // //         {
        //           if (lexerError != 1)
        //             printToken(token8);
        //           else
        //             lexerError = 0;
        // //         }
        bufferSwitch(&begin, &forward, fp, 1);
        // break;
    }

    // a loop to obtain all tokens and print them like above would be convenient
    fclose(fp);
    return 0;
}
*/
