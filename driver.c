/*
GROUP NUMBER: 5
MEMBERS:
    1. HRISHIKESH HARSH (2020A7PS0313P)
    2. HARSH PRIYADARSHI (2020A7PS0110P)
    3. ANTRIKSH SHARMA (2020A7PS1691P)
    4. KAUSTAB CHOUDHURY (2020A7PS0013P)
    5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)
*/

#include "Code_Gen.c"
#include <time.h>
int grammar_flag = 0;
void emptyBuffer(char *buffer)
{
    /* Empties the buffer */
    for (int i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = '\0';
    return;
}
ASTNode node_created_once;
int flag_ast_created = 0;

void callLexer(char *test_file_name)
{
    /*
    - The first 4 lines of this function are to reset everything to the initial state in case callParser() was called before this function.
    - Please DO NOT change any of these lines.
    */
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the lexer.
    -   The hash table is populated with the keywords
    */
    hash_linked_list *hash_table = construct_hash_table_lexer();
    hash_table = populate_hash_table_lexer(hash_table);
    // print_hash_table(hash_table);

    FILE *fp = fopen(test_file_name, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    fillBuffers(fp);

    /*
        “Everything can be taken from a man but one thing:
            the last of the human freedoms—to choose one’s attitude in any given set of circumstances, to choose one’s own way.”
    */

    int begin = 0;
    int forward = 0;
    while (EOF_Flag == 0)
    {
        token_info token8 = getNextToken(fp, hash_table, &begin, &forward);
        if (token8 == NULL)
            continue;

        if (token8->type == ERROR_ID)
        {
            // printf("Error: \"%s\" Size exceeds %d | Line: %d\n", big_identifier, LEXEME_SIZE, lineNum);
            // token8=tokenise();
            // token8 = (token_info)malloc(sizeof(struct token_info));
            // token8->type = ERROR;
            // token8->lineNum = lineNum;
            // printf("ERROR: %s | line: %d | Type: %d\n", big_identifier, token8->lineNum, token8->type);
            free(big_identifier);
        }
        else if (token8->type == ERROR)
        {
            // do nothing
            // printf("ERROR: %s | line: %d ", token8->content.identifier, token8->lineNum);
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
    }
    fclose(fp);
    return;
}

void callParser(char *test_file_name, char *tree_out_name)
{
    /*
    - The first 4 lines of this function are to reset everything to the initial state in case callLexer() was called before this function.
    - Please DO NOT change any of these lines.
    */
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the parser.
    -   The hash table is populated with the terminals and non-terminals
    */

    /*
    Brief description of the variables used here on:
     - grammar: The grammar of the language
     - reversedGrammar: The reversed grammar of the language
     - epsilon_set: The set of non-terminals that can derive epsilon
     - set_first: The first set of all non-terminals
     - rule_wise_first: The first set of all rules
     - set_follow: The follow set of all non-terminals
     - parse_table: The parse table of the language
     - parse_tree: The parse tree of the input source code
    */

    /*
    -   The following lines populate, construct and get ready all the above variables necessary for the parser.
    -   Kindly DO NOT tamper with any of these lines.
    -   If you wish to ever print any of the above variables, you can do so by uncommenting the respective print functions.
    -   Please note that we were not aware that comments could exceed BUFFER_SIZE. If it does, the lexer output would be erroneous.
    -   Hence take care to not exceed BUFFER_SIZE in your comments.
    */
    alpha_list *hash_table_parser = construct_hash_table_parser();
    hash_table_parser = populate_hash_table_parser(hash_table_parser);
    // print_alpha_list(hash_table_parser);

    FILE *fp_2 = fopen("grammar_modified.txt", "r");

    if (fp_2 == NULL)
        printf("Error opening grammar_modified.txt file!\n");

    if (grammar_flag == 0)
    {
        grammar = malloc(NUM_RULES * sizeof(alpha_node));
        grammar = createGrammarBase(fp_2, hash_table_parser);
        // printf("\n check 1");
        if (grammar == NULL)
            printf("Unable to create grammar base!\n");

        // printGrammar(grammar, NUM_RULES);

        epsilon_set = getEpsilonNonTerminals(grammar, NUM_RULES);

        reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
        reversedGrammar = reverseGrammar(grammar);

        // printf("\n[Reversed]");
        // printGrammar(reversedGrammar, NUM_RULES);

        set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

        for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
        {
            set_first[i] = (Set *)malloc(sizeof(Set));
            set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_first[i]->array = getFirstSet(grammar, i);
            set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_first, NUM_NON_TERMINALS, 0);

        rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

        for (int i = 0; i < NUM_RULES; i++)
        {
            rule_wise_first[i] = (Set *)malloc(sizeof(Set));
            rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
            rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
            rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
        }

        // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

        set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
        set_follow[0] = (Set *)malloc(sizeof(Set));
        set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
        set_follow[0]->array[DOLLAR] = 1;
        set_follow[0]->size = 1;

        for (int i = 1; i < NUM_NON_TERMINALS; i++)
        {
            set_follow[i] = (Set *)malloc(sizeof(Set));
            set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_follow[i]->array = getFollowSet(grammar, i);
            set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_follow, NUM_NON_TERMINALS, 1);
        grammar_flag = 1;
    }
    int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
    }
    createParseTable(rule_wise_first, set_follow, grammar, parseTable);

    // printParseTable(parseTable, grammar);

    fclose(fp_2);
    /*
    - Reserved for KASAB
    */

    parseTree parse_tree = parseInputSourceCode(test_file_name, parseTable, grammar, reversedGrammar);

    if (syntax_error == 1)
    {
        printf("\n[Please note that the error message of the same line may be repeated multiple times]\n");
        printf("[This happens as the Parser tries to recover from the error]\n\n");
        return;
    }

    if (size_declared_parse_tree == 0)
    {
        printParseTree(parse_tree, tree_out_name);
        size_declared_parse_tree = 1;
    }
    FILE *fp_5 = fopen(tree_out_name, "r");
    if (fp_5 == NULL)
    {
        printf("tree in file\n");
    }
    while (!feof(fp_5))
    {
        char c = fgetc(fp_5);
        if (c != EOF)
            printf("%c", c);
    }
    printf("\n[Parse Tree Generated \u2713]\n");
    printf("\n\n");
    fclose(fp_5);

    return;
}

void callAST(char *test_file_name, char *tree_out_name)
{
    /*
    - The first 4 lines of this function are to reset everything to the initial state in case callLexer() was called before this function.
    - Please DO NOT change any of these lines.
    */
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the parser.
    -   The hash table is populated with the terminals and non-terminals
    */

    /*
    Brief description of the variables used here on:
     - grammar: The grammar of the language
     - reversedGrammar: The reversed grammar of the language
     - epsilon_set: The set of non-terminals that can derive epsilon
     - set_first: The first set of all non-terminals
     - rule_wise_first: The first set of all rules
     - set_follow: The follow set of all non-terminals
     - parse_table: The parse table of the language
     - parse_tree: The parse tree of the input source code
    */

    alpha_list *hash_table_parser = construct_hash_table_parser();
    hash_table_parser = populate_hash_table_parser(hash_table_parser);
    // print_alpha_list(hash_table_parser);

    FILE *fp_2 = fopen("grammar_modified.txt", "r");

    if (fp_2 == NULL)
        printf("Error opening grammar_modified.txt file!\n");
    if (grammar_flag == 0)
    {
        grammar = malloc(NUM_RULES * sizeof(alpha_node));
        grammar = createGrammarBase(fp_2, hash_table_parser);
        // printf("\n check 1");
        if (grammar == NULL)
            printf("Unable to create grammar base!\n");

        // printGrammar(grammar, NUM_RULES);

        epsilon_set = getEpsilonNonTerminals(grammar, NUM_RULES);

        reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
        reversedGrammar = reverseGrammar(grammar);

        // printf("\n[Reversed]");
        // printGrammar(reversedGrammar, NUM_RULES);

        set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

        for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
        {
            set_first[i] = (Set *)malloc(sizeof(Set));
            set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_first[i]->array = getFirstSet(grammar, i);
            set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_first, NUM_NON_TERMINALS, 0);

        rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

        for (int i = 0; i < NUM_RULES; i++)
        {
            rule_wise_first[i] = (Set *)malloc(sizeof(Set));
            rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
            rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
            rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
        }

        // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

        set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
        set_follow[0] = (Set *)malloc(sizeof(Set));
        set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
        set_follow[0]->array[DOLLAR] = 1;
        set_follow[0]->size = 1;

        for (int i = 1; i < NUM_NON_TERMINALS; i++)
        {
            set_follow[i] = (Set *)malloc(sizeof(Set));
            set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_follow[i]->array = getFollowSet(grammar, i);
            set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_follow, NUM_NON_TERMINALS, 1);
        grammar_flag = 1;
    }
    int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
    }
    createParseTable(rule_wise_first, set_follow, grammar, parseTable);

    // printParseTable(parseTable, grammar);

    fclose(fp_2);

    parseTree parse_tree = parseInputSourceCode(test_file_name, parseTable, grammar, reversedGrammar);

    if (syntax_error == 1)
    {
        printf("\n[Please note that the error message of the same line may be repeated multiple times]\n");
        printf("[This happens as the Parser tries to recover from the error]\n\n");
        return;
    }
    if (size_declared_parse_tree == 0)
    {
        printParseTree(parse_tree, tree_out_name);
        size_declared_parse_tree = 1;
    }
    if (flag_ast_created == 0)
    {
        node_created_once = getASTInformation(parse_tree->root, 0);
        flag_ast_created = 1;
    }
    printf("[AST Created \u2713]\n\n");
    displayASTprint(node_created_once);
    printf("\n\n");

    return;
}

void callMemory(char *test_file_name, char *tree_out_name)
{
    /*
    - The first 4 lines of this function are to reset everything to the initial state in case callLexer() was called before this function.
    - Please DO NOT change any of these lines.
    */
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the parser.
    -   The hash table is populated with the terminals and non-terminals
    */

    /*
    Brief description of the variables used here on:
     - grammar: The grammar of the language
     - reversedGrammar: The reversed grammar of the language
     - epsilon_set: The set of non-terminals that can derive epsilon
     - set_first: The first set of all non-terminals
     - rule_wise_first: The first set of all rules
     - set_follow: The follow set of all non-terminals
     - parse_table: The parse table of the language
     - parse_tree: The parse tree of the input source code
    */

    /*
    -   The following lines populate, construct and get ready all the above variables necessary for the parser.
    -   Kindly DO NOT tamper with any of these lines.
    -   If you wish to ever print any of the above variables, you can do so by uncommenting the respective print functions.
    -   Please note that we were not aware that comments could exceed BUFFER_SIZE. If it does, the lexer output would be erroneous.
    -   Hence take care to not exceed BUFFER_SIZE in your comments.
    */
    alpha_list *hash_table_parser = construct_hash_table_parser();
    hash_table_parser = populate_hash_table_parser(hash_table_parser);
    // print_alpha_list(hash_table_parser);

    FILE *fp_2 = fopen("grammar_modified.txt", "r");

    if (fp_2 == NULL)
        printf("Error opening grammar_modified.txt file!\n");
    if (grammar_flag == 0)
    {
        grammar = malloc(NUM_RULES * sizeof(alpha_node));
        grammar = createGrammarBase(fp_2, hash_table_parser);
        // printf("\n check 1");
        if (grammar == NULL)
            printf("Unable to create grammar base!\n");

        // printGrammar(grammar, NUM_RULES);

        epsilon_set = getEpsilonNonTerminals(grammar, NUM_RULES);

        reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
        reversedGrammar = reverseGrammar(grammar);

        // printf("\n[Reversed]");
        // printGrammar(reversedGrammar, NUM_RULES);

        set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

        for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
        {
            set_first[i] = (Set *)malloc(sizeof(Set));
            set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_first[i]->array = getFirstSet(grammar, i);
            set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_first, NUM_NON_TERMINALS, 0);

        rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

        for (int i = 0; i < NUM_RULES; i++)
        {
            rule_wise_first[i] = (Set *)malloc(sizeof(Set));
            rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
            rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
            rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
        }

        // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

        set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
        set_follow[0] = (Set *)malloc(sizeof(Set));
        set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
        set_follow[0]->array[DOLLAR] = 1;
        set_follow[0]->size = 1;

        for (int i = 1; i < NUM_NON_TERMINALS; i++)
        {
            set_follow[i] = (Set *)malloc(sizeof(Set));
            set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_follow[i]->array = getFollowSet(grammar, i);
            set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_follow, NUM_NON_TERMINALS, 1);
        grammar_flag = 1;
    }
    int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
    }
    createParseTable(rule_wise_first, set_follow, grammar, parseTable);

    // printParseTable(parseTable, grammar);

    fclose(fp_2);

    parseTree parse_tree = parseInputSourceCode(test_file_name, parseTable, grammar, reversedGrammar);

    if (syntax_error == 1)
    {
        printf("\n[Please note that the error message of the same line may be repeated multiple times]\n");
        printf("[This happens as the Parser tries to recover from the error]\n\n");
        return;
    }

    if (size_declared_parse_tree == 0)
    {
        printParseTree(parse_tree, tree_out_name);
        size_declared_parse_tree = 1;
    }
    if (flag_ast_created == 0)
    {
        node_created_once = getASTInformation(parse_tree->root, 0);
        flag_ast_created = 1;
    }
    if (ast_counted == 0)
    {
        displayAST(node_created_once);
        ast_counted = 1;
    }
    float compression_percent = ((parse_tree_size - ast_size1) / (float)parse_tree_size) * 100.0;
    printf("parse tree size = %ld, parse tree node count = %d\n", parse_tree_size, parse_tree_count);
    printf("ast size = %ld, ast node count = %d\n", ast_size1, ast_count1);
    printf("Compression percentage = %f\n", compression_percent);
    printf("\n\n");

    return;
}

void callDisplaySymbolTable(char *test_file_name, char *tree_out_name)
{

    /*
    - The first 4 lines of this function are to reset everything to the initial state in case callLexer() was called before this function.
    - Please DO NOT change any of these lines.
    */
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the parser.
    -   The hash table is populated with the terminals and non-terminals
    */

    /*
    Brief description of the variables used here on:
     - grammar: The grammar of the language
     - reversedGrammar: The reversed grammar of the language
     - epsilon_set: The set of non-terminals that can derive epsilon
     - set_first: The first set of all non-terminals
     - rule_wise_first: The first set of all rules
     - set_follow: The follow set of all non-terminals
     - parse_table: The parse table of the language
     - parse_tree: The parse tree of the input source code
    */

    /*
    -   The following lines populate, construct and get ready all the above variables necessary for the parser.
    -   Kindly DO NOT tamper with any of these lines.
    -   If you wish to ever print any of the above variables, you can do so by uncommenting the respective print functions.
    -   Please note that we were not aware that comments could exceed BUFFER_SIZE. If it does, the lexer output would be erroneous.
    -   Hence take care to not exceed BUFFER_SIZE in your comments.
    */
    alpha_list *hash_table_parser = construct_hash_table_parser();
    hash_table_parser = populate_hash_table_parser(hash_table_parser);
    // print_alpha_list(hash_table_parser);

    FILE *fp_2 = fopen("grammar_modified.txt", "r");

    if (fp_2 == NULL)
        printf("Error opening grammar_modified.txt file!\n");

    if (grammar_flag == 0)
    {
        grammar = malloc(NUM_RULES * sizeof(alpha_node));
        grammar = createGrammarBase(fp_2, hash_table_parser);
        // printf("\n check 1");
        if (grammar == NULL)
            printf("Unable to create grammar base!\n");

        // printGrammar(grammar, NUM_RULES);

        epsilon_set = getEpsilonNonTerminals(grammar, NUM_RULES);

        reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
        reversedGrammar = reverseGrammar(grammar);

        // printf("\n[Reversed]");
        // printGrammar(reversedGrammar, NUM_RULES);

        set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

        for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
        {
            set_first[i] = (Set *)malloc(sizeof(Set));
            set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_first[i]->array = getFirstSet(grammar, i);
            set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_first, NUM_NON_TERMINALS, 0);

        rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

        for (int i = 0; i < NUM_RULES; i++)
        {
            rule_wise_first[i] = (Set *)malloc(sizeof(Set));
            rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
            rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
            rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
        }

        // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

        set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
        set_follow[0] = (Set *)malloc(sizeof(Set));
        set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
        set_follow[0]->array[DOLLAR] = 1;
        set_follow[0]->size = 1;

        for (int i = 1; i < NUM_NON_TERMINALS; i++)
        {
            set_follow[i] = (Set *)malloc(sizeof(Set));
            set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_follow[i]->array = getFollowSet(grammar, i);
            set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_follow, NUM_NON_TERMINALS, 1);
        grammar_flag = 1;
    }

    int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
    }
    createParseTable(rule_wise_first, set_follow, grammar, parseTable);

    // printParseTable(parseTable, grammar);

    fclose(fp_2);

    parseTree parse_tree = parseInputSourceCode(test_file_name, parseTable, grammar, reversedGrammar);

    if (syntax_error == 1)
    {
        printf("\n[Please note that the error message of the same line may be repeated multiple times]\n");
        printf("[This happens as the Parser tries to recover from the error]\n\n");
        return;
    }

    if (size_declared_parse_tree == 0)
    {
        printParseTree(parse_tree, tree_out_name);
        size_declared_parse_tree = 1;
    }
    if (flag_ast_created == 0)
    {
        node_created_once = getASTInformation(parse_tree->root, 0);
        flag_ast_created = 1;
    }

    curr_size = 2;
    module_calls = (char **)malloc(curr_size * sizeof(char *));
    for (int i = 0; i < curr_size; i++)
    {
        module_calls[i] = NULL;
    }
    module_names = (char **)malloc((curr_size / 2) * sizeof(char *));
    module_names[0] = NULL;
    pending_module_checks = (freeze_state *)malloc(freeze_state_size * sizeof(freeze_state));
    pending_module_checks[freeze_state_size - 1] = NULL;
    if (big_root_created == 0)
    {
        big_root_created = 1;
        hash_table_holder root = createHashTableHolder(ROOT);
        strcpy(root->self_name, "FUNCTION_ROOT");
        BIG_ROOT = root;
        populateSymbolTable(BIG_ROOT, node_created_once, ROOT_STATE);
    }
    displaySymbolTables(BIG_ROOT, 0);
    // displayAllArrays();
    // printf("semantic_error_count: %d\n",semantic_error_count);

    printf("\n\n");

    return;
}

void callActivationRecords(char *test_file_name, char *tree_out_name)
{

    /*
    - The first 4 lines of this function are to reset everything to the initial state in case callLexer() was called before this function.
    - Please DO NOT change any of these lines.
    */
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the parser.
    -   The hash table is populated with the terminals and non-terminals
    */

    /*
    Brief description of the variables used here on:
     - grammar: The grammar of the language
     - reversedGrammar: The reversed grammar of the language
     - epsilon_set: The set of non-terminals that can derive epsilon
     - set_first: The first set of all non-terminals
     - rule_wise_first: The first set of all rules
     - set_follow: The follow set of all non-terminals
     - parse_table: The parse table of the language
     - parse_tree: The parse tree of the input source code
    */

    /*
    -   The following lines populate, construct and get ready all the above variables necessary for the parser.
    -   Kindly DO NOT tamper with any of these lines.
    -   If you wish to ever print any of the above variables, you can do so by uncommenting the respective print functions.
    -   Please note that we were not aware that comments could exceed BUFFER_SIZE. If it does, the lexer output would be erroneous.
    -   Hence take care to not exceed BUFFER_SIZE in your comments.
    */
    alpha_list *hash_table_parser = construct_hash_table_parser();
    hash_table_parser = populate_hash_table_parser(hash_table_parser);
    // print_alpha_list(hash_table_parser);

    FILE *fp_2 = fopen("grammar_modified.txt", "r");

    if (fp_2 == NULL)
        printf("Error opening grammar_modified.txt file!\n");

    if (grammar_flag == 0)
    {
        grammar = malloc(NUM_RULES * sizeof(alpha_node));
        grammar = createGrammarBase(fp_2, hash_table_parser);
        // printf("\n check 1");
        if (grammar == NULL)
            printf("Unable to create grammar base!\n");

        // printGrammar(grammar, NUM_RULES);

        epsilon_set = getEpsilonNonTerminals(grammar, NUM_RULES);

        reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
        reversedGrammar = reverseGrammar(grammar);

        // printf("\n[Reversed]");
        // printGrammar(reversedGrammar, NUM_RULES);

        set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

        for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
        {
            set_first[i] = (Set *)malloc(sizeof(Set));
            set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_first[i]->array = getFirstSet(grammar, i);
            set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_first, NUM_NON_TERMINALS, 0);

        rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

        for (int i = 0; i < NUM_RULES; i++)
        {
            rule_wise_first[i] = (Set *)malloc(sizeof(Set));
            rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
            rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
            rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
        }

        // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

        set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
        set_follow[0] = (Set *)malloc(sizeof(Set));
        set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
        set_follow[0]->array[DOLLAR] = 1;
        set_follow[0]->size = 1;

        for (int i = 1; i < NUM_NON_TERMINALS; i++)
        {
            set_follow[i] = (Set *)malloc(sizeof(Set));
            set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_follow[i]->array = getFollowSet(grammar, i);
            set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_follow, NUM_NON_TERMINALS, 1);
        grammar_flag = 1;
    }

    int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
    }
    createParseTable(rule_wise_first, set_follow, grammar, parseTable);

    // printParseTable(parseTable, grammar);

    fclose(fp_2);

    parseTree parse_tree = parseInputSourceCode(test_file_name, parseTable, grammar, reversedGrammar);

    if (syntax_error == 1)
    {
        printf("\n[Please note that the error message of the same line may be repeated multiple times]\n");
        printf("[This happens as the Parser tries to recover from the error]\n\n");
        return;
    }

    if (size_declared_parse_tree == 0)
    {
        printParseTree(parse_tree, tree_out_name);
        size_declared_parse_tree = 1;
    }
    if (flag_ast_created == 0)
    {
        node_created_once = getASTInformation(parse_tree->root, 0);
        flag_ast_created = 1;
    }

    curr_size = 2;
    module_calls = (char **)malloc(curr_size * sizeof(char *));
    for (int i = 0; i < curr_size; i++)
    {
        module_calls[i] = NULL;
    }
    module_names = (char **)malloc((curr_size / 2) * sizeof(char *));
    module_names[0] = NULL;
    pending_module_checks = (freeze_state *)malloc(freeze_state_size * sizeof(freeze_state));
    pending_module_checks[freeze_state_size - 1] = NULL;

    if (big_root_created == 0)
    {
        big_root_created = 1;
        hash_table_holder root = createHashTableHolder(ROOT);
        strcpy(root->self_name, "FUNCTION_ROOT");
        BIG_ROOT = root;
        populateSymbolTable(BIG_ROOT, node_created_once, ROOT_STATE);
    }
    displaySymbolTables(BIG_ROOT, 0);
    // displayAllArrays();
    // printf("semantic_error_count: %d\n",semantic_error_count);

    printf("\n\n");
    for (int i = 0; i < function_count; i++)
    {
        if (all_functions[i] == NULL)
            continue;
        else
        {
            printf("| Module: %s | Space_Occupied: %d |\n", all_functions[i]->module_name, all_functions[i]->occupied_space);
        }
    }
    printf("\n\n");

    return;
}

void callDisplayArrays(char *test_file_name, char *tree_out_name)
{

    /*
    - The first 4 lines of this function are to reset everything to the initial state in case callLexer() was called before this function.
    - Please DO NOT change any of these lines.
    */
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the parser.
    -   The hash table is populated with the terminals and non-terminals
    */

    /*
    Brief description of the variables used here on:
     - grammar: The grammar of the language
     - reversedGrammar: The reversed grammar of the language
     - epsilon_set: The set of non-terminals that can derive epsilon
     - set_first: The first set of all non-terminals
     - rule_wise_first: The first set of all rules
     - set_follow: The follow set of all non-terminals
     - parse_table: The parse table of the language
     - parse_tree: The parse tree of the input source code
    */

    /*
    -   The following lines populate, construct and get ready all the above variables necessary for the parser.
    -   Kindly DO NOT tamper with any of these lines.
    -   If you wish to ever print any of the above variables, you can do so by uncommenting the respective print functions.
    -   Please note that we were not aware that comments could exceed BUFFER_SIZE. If it does, the lexer output would be erroneous.
    -   Hence take care to not exceed BUFFER_SIZE in your comments.
    */
    alpha_list *hash_table_parser = construct_hash_table_parser();
    hash_table_parser = populate_hash_table_parser(hash_table_parser);
    // print_alpha_list(hash_table_parser);

    FILE *fp_2 = fopen("grammar_modified.txt", "r");

    if (fp_2 == NULL)
        printf("Error opening grammar_modified.txt file!\n");

    if (grammar_flag == 0)
    {
        grammar = malloc(NUM_RULES * sizeof(alpha_node));
        grammar = createGrammarBase(fp_2, hash_table_parser);
        // printf("\n check 1");
        if (grammar == NULL)
            printf("Unable to create grammar base!\n");

        // printGrammar(grammar, NUM_RULES);

        epsilon_set = getEpsilonNonTerminals(grammar, NUM_RULES);

        reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
        reversedGrammar = reverseGrammar(grammar);

        // printf("\n[Reversed]");
        // printGrammar(reversedGrammar, NUM_RULES);

        set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

        for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
        {
            set_first[i] = (Set *)malloc(sizeof(Set));
            set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_first[i]->array = getFirstSet(grammar, i);
            set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_first, NUM_NON_TERMINALS, 0);

        rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

        for (int i = 0; i < NUM_RULES; i++)
        {
            rule_wise_first[i] = (Set *)malloc(sizeof(Set));
            rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
            rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
            rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
        }

        // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

        set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
        set_follow[0] = (Set *)malloc(sizeof(Set));
        set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
        set_follow[0]->array[DOLLAR] = 1;
        set_follow[0]->size = 1;

        for (int i = 1; i < NUM_NON_TERMINALS; i++)
        {
            set_follow[i] = (Set *)malloc(sizeof(Set));
            set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_follow[i]->array = getFollowSet(grammar, i);
            set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_follow, NUM_NON_TERMINALS, 1);
        grammar_flag = 1;
    }

    int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
    }
    createParseTable(rule_wise_first, set_follow, grammar, parseTable);

    // printParseTable(parseTable, grammar);

    fclose(fp_2);

    parseTree parse_tree = parseInputSourceCode(test_file_name, parseTable, grammar, reversedGrammar);

    if (syntax_error == 1)
    {
        printf("\n[Please note that the error message of the same line may be repeated multiple times]\n");
        printf("[This happens as the Parser tries to recover from the error]\n\n");
        return;
    }

    if (size_declared_parse_tree == 0)
    {
        printParseTree(parse_tree, tree_out_name);
        size_declared_parse_tree = 1;
    }
    if (flag_ast_created == 0)
    {
        node_created_once = getASTInformation(parse_tree->root, 0);
        flag_ast_created = 1;
    }

    curr_size = 2;
    module_calls = (char **)malloc(curr_size * sizeof(char *));
    for (int i = 0; i < curr_size; i++)
    {
        module_calls[i] = NULL;
    }
    module_names = (char **)malloc((curr_size / 2) * sizeof(char *));
    module_names[0] = NULL;
    pending_module_checks = (freeze_state *)malloc(freeze_state_size * sizeof(freeze_state));
    pending_module_checks[freeze_state_size - 1] = NULL;
    if (big_root_created == 0)
    {
        big_root_created = 1;
        hash_table_holder root = createHashTableHolder(ROOT);
        strcpy(root->self_name, "FUNCTION_ROOT");
        BIG_ROOT = root;
        populateSymbolTable(BIG_ROOT, node_created_once, ROOT_STATE);
    }
    displaySymbolTables(BIG_ROOT, 1);
    displayAllArrays();
    // printf("semantic_error_count: %d\n",semantic_error_count);
    printf("\n\n");

    return;
}

void callErrors(char *test_file_name, char *tree_out_name)
{
    clock_t start_time, end_time;
    double CPU_time = 0.0, CPU_time_sec = 0.0;
    start_time = clock();
    /*
    - The first 4 lines of this function are to reset everything to the initial state in case callLexer() was called before this function.
    - Please DO NOT change any of these lines.
    */
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the parser.
    -   The hash table is populated with the terminals and non-terminals
    */

    /*
    Brief description of the variables used here on:
     - grammar: The grammar of the language
     - reversedGrammar: The reversed grammar of the language
     - epsilon_set: The set of non-terminals that can derive epsilon
     - set_first: The first set of all non-terminals
     - rule_wise_first: The first set of all rules
     - set_follow: The follow set of all non-terminals
     - parse_table: The parse table of the language
     - parse_tree: The parse tree of the input source code
    */

    /*
    -   The following lines populate, construct and get ready all the above variables necessary for the parser.
    -   Kindly DO NOT tamper with any of these lines.
    -   If you wish to ever print any of the above variables, you can do so by uncommenting the respective print functions.
    -   Please note that we were not aware that comments could exceed BUFFER_SIZE. If it does, the lexer output would be erroneous.
    -   Hence take care to not exceed BUFFER_SIZE in your comments.
    */
    alpha_list *hash_table_parser = construct_hash_table_parser();
    hash_table_parser = populate_hash_table_parser(hash_table_parser);
    // print_alpha_list(hash_table_parser);

    FILE *fp_2 = fopen("grammar_modified.txt", "r");

    if (fp_2 == NULL)
        printf("Error opening grammar_modified.txt file!\n");
    if (grammar_flag == 0)
    {
        grammar = malloc(NUM_RULES * sizeof(alpha_node));
        grammar = createGrammarBase(fp_2, hash_table_parser);
        // printf("\n check 1");
        if (grammar == NULL)
            printf("Unable to create grammar base!\n");

        // printGrammar(grammar, NUM_RULES);

        epsilon_set = getEpsilonNonTerminals(grammar, NUM_RULES);

        reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
        reversedGrammar = reverseGrammar(grammar);

        // printf("\n[Reversed]");
        // printGrammar(reversedGrammar, NUM_RULES);
        set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

        for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
        {
            set_first[i] = (Set *)malloc(sizeof(Set));
            set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_first[i]->array = getFirstSet(grammar, i);
            set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_first, NUM_NON_TERMINALS, 0);

        rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

        for (int i = 0; i < NUM_RULES; i++)
        {
            rule_wise_first[i] = (Set *)malloc(sizeof(Set));
            rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
            rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
            rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
        }

        // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

        set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
        set_follow[0] = (Set *)malloc(sizeof(Set));
        set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
        set_follow[0]->array[DOLLAR] = 1;
        set_follow[0]->size = 1;

        for (int i = 1; i < NUM_NON_TERMINALS; i++)
        {
            set_follow[i] = (Set *)malloc(sizeof(Set));
            set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_follow[i]->array = getFollowSet(grammar, i);
            set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
        }
        grammar_flag = 1;
        // displaySet(set_follow, NUM_NON_TERMINALS, 1);
    }

    int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
    }
    createParseTable(rule_wise_first, set_follow, grammar, parseTable);

    // printParseTable(parseTable, grammar);

    fclose(fp_2);
    /*
    Daily Thoughts of a Programmer:
    - I wonder if I will ever be able to write a code without any bugs
    - I wish the debugger could speak to me better
    - We really did spend 4 HOURS debugging the Follow Set function
    - I really wonder how I'll draw the equivalence between the token_info and the alpha_node
    - Parsing is really hard
    - Parse_Tree..... Ahhhh... Stress...!
    - Man, the transition was not smooth at all
    - Struggling a bit right now
    - Teammates are really helpful
    - Should I get a coffee? Or Sleep?
    - I really need to get some sleep
    - Man, I made such a stupid blunder in the First/Follow concept
    - It was terrible, especially after all the pains I took to get them running
    */

    parseTree parse_tree = parseInputSourceCode(test_file_name, parseTable, grammar, reversedGrammar);

    if (syntax_error == 1)
    {
        printf("\n[Please note that the error message of the same line may be repeated multiple times]\n");
        printf("[This happens as the Parser tries to recover from the error]\n\n");
        return;
    }
    if (size_declared_parse_tree == 0)
    {
        printParseTree(parse_tree, tree_out_name);
        size_declared_parse_tree = 1;
    }
    if (flag_ast_created == 0)
    {
        node_created_once = getASTInformation(parse_tree->root, 0);
        flag_ast_created = 1;
    }

    curr_size = 2;
    module_calls = (char **)malloc(curr_size * sizeof(char *));
    for (int i = 0; i < curr_size; i++)
    {
        module_calls[i] = NULL;
    }
    module_names = (char **)malloc((curr_size / 2) * sizeof(char *));
    module_names[0] = NULL;
    pending_module_checks = (freeze_state *)malloc(freeze_state_size * sizeof(freeze_state));
    pending_module_checks[freeze_state_size - 1] = NULL;

    if (big_root_created == 0)
    {
        big_root_created = 1;
        hash_table_holder root = createHashTableHolder(ROOT);
        strcpy(root->self_name, "FUNCTION_ROOT");
        BIG_ROOT = root;
        populateSymbolTable(BIG_ROOT, node_created_once, ROOT_STATE);
    }

    // displaySymbolTables(BIG_ROOT, 1);
    // displayAllArrays();
    // printf("semantic_error_count: %d\n",semantic_error_count);

    for (int i = 0; i < freeze_state_size; i++)
    {
        if (pending_module_checks[i] == NULL)
            break;
        if (pending_module_checks[i] != NULL)
        {
            hash_table_holder croot = pending_module_checks[i]->curr_root;
            ASTNode node = pending_module_checks[i]->node;

            char *module_name = node->children[1]->dataForPrint->lexeme;
            hash_component t = searchHashTable(BIG_ROOT->hash_table, module_name);

            if (t != NULL && t->content.hash_table_unit->isInitialised == 0)
            {
                char error_message[100];
                sprintf(error_message, "ERROR in Line %d | Module %s body never initialised\n", node->children[1]->dataForPrint->lineNo, module_name);
                type_pair er = createErrorType(error_message);
                // displayErrorType(er);
                continue;
            }

            callSemanticCheck(node, MODULE_REUSE_CHECK, croot);
        }
    }

    build_heap(semantic_error_count);
    heapSort(semantic_error_count);

    for (int i = 0; i < semantic_error_count; i++)
    {
        if (all_errors[i] == NULL)
            printf("NULL\n");
        else
            displayErrorType(all_errors[i]->err);
    }

    /*
        - Now, since we traverse the AST only once, we used the module_calls array to store the calls
        - We then use this array to create the directed graph
        - We then use the directed graph to create the module dependency graph
        - We then detect the cycles in the module dependency graph to check for recursive calls
    */

    int directed_graph_modules[UID_COUNTER][UID_COUNTER];
    visited = (int *)malloc(UID_COUNTER * sizeof(int));
    visited_on_current_path = (int *)malloc(UID_COUNTER * sizeof(int));
    path = (int *)malloc(UID_COUNTER * sizeof(int));

    for (int i = 0; i < UID_COUNTER; i++)
    {
        visited[i] = 0;
        visited_on_current_path[i] = 0;
        path[i] = -1;
        for (int j = 0; j < UID_COUNTER; j++)
        {
            directed_graph_modules[i][j] = -1;
        }
    }
    curr_ptr = 0;
    // displayModuleCallPairs(module_calls,curr_size);
    for (int i = 0; i < curr_size; i = i + 2)
    { /* A Single edge from caller to callee */
        if (module_calls[i] == NULL || module_calls[i + 1] == NULL)
            break;
        int u = searchHashTable(BIG_ROOT->hash_table, module_calls[i])->content.hash_table_unit->UID;
        int v = searchHashTable(BIG_ROOT->hash_table, module_calls[i + 1])->content.hash_table_unit->UID;

        directed_graph_modules[u][v] = 1;
    }

    int non_recursive;
    for (int i = 0; i < UID_COUNTER; i++)
    {
        non_recursive = detectRecursion(i, directed_graph_modules);
        if (non_recursive == 0)
            break;
    }
    if (non_recursive == 0)
    {
    }

    if (non_recursive == 0) // Recursion detected
    {
        num_semantic_errors++;

        int l_id = -1;
        for (int i = UID_COUNTER - 1; i >= 0; i--)
        {
            if (path[i] == -1)
                continue;
            l_id = i;
            break;
        }
        printf("\n[Recursion Detected..!]\n");
        printf("[Call Sequence: ");
        for (int i = l_id; i >= 0; i--)
        {
            // printf("i = %d ",i);
            if (i > 0)
                printf("%s() -> ", module_names[path[i]]);
            else
                printf("%s()]\n", module_names[path[i]]);
        }
    }
    else
        printf("\n[No Recursion Detected]\n");

    if (num_semantic_errors > 0)
    {
        printf("\n%d Semantic errors detected | Consider fixing them and trying again\n", num_semantic_errors);
        // return num_semantic_errors;
    }
    else
    {
        printf("\nSemantically Verified | Code Compiled Successfully......\u2713");
    }

    end_time = clock();
    CPU_time = (double)(end_time - start_time);
    CPU_time_sec = CPU_time / CLOCKS_PER_SEC;

    printf("[Time taken (CPU): %lf \u03BCs]\n", CPU_time);
    printf("[Time taken (in sec): %lf s]\n\n", CPU_time_sec);

    printf("\n\n");

    return;
}

void callCodeGeneration(char *test_file_name, char *tree_out_name, char *asm_file)
{
    in_use = BUFFER_1;
    lineNum = 1;
    EOF_Flag = 0;
    emptyBuffer(buffer_1);
    emptyBuffer(buffer_2);
    emptyBuffer(selected_buffer);

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    /*
    -   The following lines are to initialise the hash table for the parser.
    -   The hash table is populated with the terminals and non-terminals
    */

    /*
    Brief description of the variables used here on:
     - grammar: The grammar of the language
     - reversedGrammar: The reversed grammar of the language
     - epsilon_set: The set of non-terminals that can derive epsilon
     - set_first: The first set of all non-terminals
     - rule_wise_first: The first set of all rules
     - set_follow: The follow set of all non-terminals
     - parse_table: The parse table of the language
     - parse_tree: The parse tree of the input source code
    */

    /*
    -   The following lines populate, construct and get ready all the above variables necessary for the parser.
    -   Kindly DO NOT tamper with any of these lines.
    -   If you wish to ever print any of the above variables, you can do so by uncommenting the respective print functions.
    -   Please note that we were not aware that comments could exceed BUFFER_SIZE. If it does, the lexer output would be erroneous.
    -   Hence take care to not exceed BUFFER_SIZE in your comments.
    */
    alpha_list *hash_table_parser = construct_hash_table_parser();
    hash_table_parser = populate_hash_table_parser(hash_table_parser);
    // print_alpha_list(hash_table_parser);

    FILE *fp_2 = fopen("grammar_modified.txt", "r");

    if (fp_2 == NULL)
        printf("Error opening grammar_modified.txt file!\n");
    if (grammar_flag == 0)
    {
        grammar = malloc(NUM_RULES * sizeof(alpha_node));
        grammar = createGrammarBase(fp_2, hash_table_parser);
        // printf("\n check 1");
        if (grammar == NULL)
            printf("Unable to create grammar base!\n");

        // printGrammar(grammar, NUM_RULES);

        epsilon_set = getEpsilonNonTerminals(grammar, NUM_RULES);

        reversedGrammar = malloc(NUM_RULES * sizeof(alpha_node));
        reversedGrammar = reverseGrammar(grammar);

        // printf("\n[Reversed]");
        // printGrammar(reversedGrammar, NUM_RULES);
        set_first = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));

        for (int i = NUM_NON_TERMINALS - 1; i >= 0; i--)
        {
            set_first[i] = (Set *)malloc(sizeof(Set));
            set_first[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_first[i]->array = getFirstSet(grammar, i);
            set_first[i]->size = getSize(set_first[i]->array, NUM_TERMINALS);
        }

        // displaySet(set_first, NUM_NON_TERMINALS, 0);

        rule_wise_first = (Set **)malloc(NUM_RULES * sizeof(Set *));

        for (int i = 0; i < NUM_RULES; i++)
        {
            rule_wise_first[i] = (Set *)malloc(sizeof(Set));
            rule_wise_first[i]->array = (int *)malloc(NUM_TERMINALS * sizeof(int));
            rule_wise_first[i]->array = getFirstSetForRule(grammar[i]);
            rule_wise_first[i]->size = getSize(rule_wise_first[i]->array, NUM_TERMINALS);
        }

        // displayRuleWiseFirstSet(rule_wise_first, NUM_RULES);

        set_follow = (Set **)malloc(NUM_NON_TERMINALS * sizeof(Set *));
        set_follow[0] = (Set *)malloc(sizeof(Set));
        set_follow[0]->array = malloc(NUM_TERMINALS * sizeof(int));
        set_follow[0]->array[DOLLAR] = 1;
        set_follow[0]->size = 1;

        for (int i = 1; i < NUM_NON_TERMINALS; i++)
        {
            set_follow[i] = (Set *)malloc(sizeof(Set));
            set_follow[i]->array = malloc(NUM_TERMINALS * sizeof(int));
            set_follow[i]->array = getFollowSet(grammar, i);
            set_follow[i]->size = getSize(set_follow[i]->array, NUM_TERMINALS);
        }
        grammar_flag = 1;
        // displaySet(set_follow, NUM_NON_TERMINALS, 1);
    }

    int **parseTable = (int **)malloc(NUM_NON_TERMINALS * sizeof(int *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        parseTable[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));
    }
    createParseTable(rule_wise_first, set_follow, grammar, parseTable);

    // printParseTable(parseTable, grammar);

    fclose(fp_2);
    /*
    Daily Thoughts of a Programmer:
    - I wonder if I will ever be able to write a code without any bugs
    - I wish the debugger could speak to me better
    - We really did spend 4 HOURS debugging the Follow Set function
    - I really wonder how I'll draw the equivalence between the token_info and the alpha_node
    - Parsing is really hard
    - Parse_Tree..... Ahhhh... Stress...!
    - Man, the transition was not smooth at all
    - Struggling a bit right now
    - Teammates are really helpful
    - Should I get a coffee? Or Sleep?
    - I really need to get some sleep
    - Man, I made such a stupid blunder in the First/Follow concept
    - It was terrible, especially after all the pains I took to get them running
    */

    parseTree parse_tree = parseInputSourceCode(test_file_name, parseTable, grammar, reversedGrammar);

    if (syntax_error == 1)
    {
        printf("\n[Please note that the error message of the same line may be repeated multiple times]\n");
        printf("[This happens as the Parser tries to recover from the error]\n\n");
        return;
    }
    if (size_declared_parse_tree == 0)
    {
        printParseTree(parse_tree, tree_out_name);
        size_declared_parse_tree = 1;
    }
    if (flag_ast_created == 0)
    {
        node_created_once = getASTInformation(parse_tree->root, 0);
        flag_ast_created = 1;
    }

    curr_size = 2;
    module_calls = (char **)malloc(curr_size * sizeof(char *));
    for (int i = 0; i < curr_size; i++)
    {
        module_calls[i] = NULL;
    }
    module_names = (char **)malloc((curr_size / 2) * sizeof(char *));
    module_names[0] = NULL;
    pending_module_checks = (freeze_state *)malloc(freeze_state_size * sizeof(freeze_state));
    pending_module_checks[freeze_state_size - 1] = NULL;

    if (big_root_created == 0)
    {
        big_root_created = 1;
        hash_table_holder root = createHashTableHolder(ROOT);
        strcpy(root->self_name, "FUNCTION_ROOT");
        BIG_ROOT = root;
        populateSymbolTable(BIG_ROOT, node_created_once, ROOT_STATE);
    }

    // displaySymbolTables(BIG_ROOT, 1);
    // displayAllArrays();
    // printf("semantic_error_count: %d\n",semantic_error_count);

    for (int i = 0; i < freeze_state_size; i++)
    {
        if (pending_module_checks[i] == NULL)
            break;
        if (pending_module_checks[i] != NULL)
        {
            hash_table_holder croot = pending_module_checks[i]->curr_root;
            ASTNode node = pending_module_checks[i]->node;

            char *module_name = node->children[1]->dataForPrint->lexeme;
            hash_component t = searchHashTable(BIG_ROOT->hash_table, module_name);

            if (t != NULL && t->content.hash_table_unit->isInitialised == 0)
            {
                char error_message[100];
                sprintf(error_message, "ERROR in Line %d | Module %s body never initialised\n", node->children[1]->dataForPrint->lineNo, module_name);
                type_pair er = createErrorType(error_message);
                // displayErrorType(er);
                continue;
            }

            callSemanticCheck(node, MODULE_REUSE_CHECK, croot);
        }
    }

    /*
        - Now, since we traverse the AST only once, we used the module_calls array to store the calls
        - We then use this array to create the directed graph
        - We then use the directed graph to create the module dependency graph
        - We then detect the cycles in the module dependency graph to check for recursive calls
    */

    int directed_graph_modules[UID_COUNTER][UID_COUNTER];
    visited = (int *)malloc(UID_COUNTER * sizeof(int));
    visited_on_current_path = (int *)malloc(UID_COUNTER * sizeof(int));
    path = (int *)malloc(UID_COUNTER * sizeof(int));

    for (int i = 0; i < UID_COUNTER; i++)
    {
        visited[i] = 0;
        visited_on_current_path[i] = 0;
        path[i] = -1;
        for (int j = 0; j < UID_COUNTER; j++)
        {
            directed_graph_modules[i][j] = -1;
        }
    }
    curr_ptr = 0;
    // displayModuleCallPairs(module_calls,curr_size);
    for (int i = 0; i < curr_size; i = i + 2)
    { /* A Single edge from caller to callee */
        if (module_calls[i] == NULL || module_calls[i + 1] == NULL)
            break;
        int u = searchHashTable(BIG_ROOT->hash_table, module_calls[i])->content.hash_table_unit->UID;
        int v = searchHashTable(BIG_ROOT->hash_table, module_calls[i + 1])->content.hash_table_unit->UID;

        directed_graph_modules[u][v] = 1;
    }

    int non_recursive;
    for (int i = 0; i < UID_COUNTER; i++)
    {
        non_recursive = detectRecursion(i, directed_graph_modules);
        if (non_recursive == 0)
            break;
    }
    if (non_recursive == 0)
    {
    }

    if (non_recursive == 0) // Recursion detected
    {
        num_semantic_errors++;

        int l_id = -1;
        for (int i = UID_COUNTER - 1; i >= 0; i--)
        {
            if (path[i] == -1)
                continue;
            l_id = i;
            break;
        }
        // printf("\n[Recursion Detected..!]\n");
        // printf("[Call Sequence: ");
        for (int i = l_id; i >= 0; i--)
        {
            // printf("i = %d ",i);
            if (i > 0)
                printf("%s() -> ", module_names[path[i]]);
            else
                printf("%s()]\n", module_names[path[i]]);
        }
    }

    if (num_semantic_errors > 0)
    {
        printf("\n%d Semantic errors detected | Consider fixing them and trying again\n", num_semantic_errors);
        // return num_semantic_errors;
    }
    else
    {
        printf("\nSemantically Verified \u2713 |\nProceeding with code generation...\n");

        masterCodeGenerator(node_created_once, BIG_ROOT, asm_file);
        printf("\nCode generation complete. \u2713\n");
    }

    printf("\n\n");

    return;
}

void printMenu()
{
    printf("------------------------------------------------------------\n");
    printf("[0 : Exit]\n");
    printf("[1 : Run Lexer To Generate Tokens]\n");
    printf("[2 : Call Parser Module To Parse The Code]\n");
    printf("[3 : Print The Abstract Syntax Tree [AST] (Preorder Traversal)]\n");
    printf("[4 : Display Memory Usage & Compression Percentage]\n");
    printf("[5 : Print The Symbol Table]\n");
    printf("[6 : Print The Activation Record Sizes)]\n");
    printf("[7 : Print The Type Expressions & Widths Of Array Variables]\n");
    printf("[8 : Print Errors (Syntactic & Semantic)]\n");
    printf("[9 : Generate Assembly Code]\n");
    printf("------------------------------------------------------------\n\nChoice : ");
}

void printStatus()
{
    printf("\n\"However, for practical purposes, in a hopelessly practical world...\"\n");

    printf("\nLEVEL 4 : Symbol table / AST / Semantic Rules / Code Generation modules work\n");

    printf("\n==============================================================\n");
    printf("\u2022 FIRST and FOLLOW set automated \u2713\n");
    printf("\u2022 Both lexical and syntax analysis modules implemented \u2713\n");
    printf("\u2022 Parse tree constructed \u2713\n");
    printf("\u2022 AST generated \u2713\n");
    printf("\u2022 Symbol Table successfully made \u2713\n");
    printf("\u2022 Semantical analysis modules implemented \u2713\n");
    printf("\u2022 Errors completely reported \u2713\n");
    printf("\u2022 Intermediate Code fully generated \u2713\n");
    printf("\u2022 Final assembly code generated \u2713\n");
    printf("==============================================================\n");
}

int main(int argc, char *argv[])
{
    // int buffer_size = 0;

    /*
       Fun Trivia/Stats:
    -  We spent 4 hours debugging the Follow Set function
    -  We spent 4 hours designing and debugging the Error recovery strategies
    -  We spent close to 22 hours debugging the Parser
    -  Lines of Code: 4,600+ (excluding useless comments)
    -  Time spent combinedly (times when all 5 of us were together): 52hrs 45mins (approx.)
    -  Time spent per day: 52.75/28 = 1hr 53mins (approx.) (2 Feb - 1 Mar)
    -  It doesn't include the time spent individually by each of us
    -  We spent at least 10 hours figuring out NASM syntax
    */

    /*
    -  What you see below is the result of countless sleepless hours and a lot of hard work.
    -  Dealing other courses, assignments, quizzes and all, we still managed to get this done.
    -  When one of our teammates was down with a fever; the other was unwell; and the third was overburdened with other courses,
        and yet we all sat down and went through this
    -  We did all we could, in our own ways, to get this done.
    */
    char *test_file_name = argv[1];
    char *asm_file = argv[2];
    char tree_out_name[20] = "tree_out.txt";
    BUFFER_SIZE = 2048;

    buffer_1 = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));
    buffer_2 = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));
    selected_buffer = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));

    buffer_1[BUFFER_SIZE] = '\0';
    buffer_2[BUFFER_SIZE] = '\0';
    selected_buffer[BUFFER_SIZE] = '\0';

    printStatus();

    int choice;
    printMenu();

    scanf("%d", &choice);
    printf("\n");

    clock_t start_time, end_time;

    double CPU_time = 0.0, CPU_time_sec = 0.0;

    while (choice != 0)
    {
        switch (choice)
        {
        case 1:
            callLexer(test_file_name);
            break;
        case 2:
            callParser(test_file_name, tree_out_name);
            break;
        case 3:
            callAST(test_file_name, tree_out_name);
            break;
        case 4:
            callMemory(test_file_name, tree_out_name);
            break;
        case 5:
            callDisplaySymbolTable(test_file_name, tree_out_name);
            break;
        case 6:
            callActivationRecords(test_file_name, tree_out_name);
            break;
        case 7:
            callDisplayArrays(test_file_name, tree_out_name);
            break;
        case 8:
            callErrors(test_file_name, tree_out_name);
            break;
        case 9:
            callCodeGeneration(test_file_name, tree_out_name, asm_file);
            break;
        default:
            printf("[Invalid Choice]\n");
            break;
        }
        printMenu();
        // printf("%s\n", test_file_name);
        scanf("%d", &choice);
        printf("\n");
    }
}
