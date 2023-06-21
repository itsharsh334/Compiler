# GROUP NUMBER: 5
# MEMBERS:
#     1. HRISHIKESH HARSH (2020A7PS0313P)
#     2. HARSH PRIYADARSHI (2020A7PS0110P)
#     3. ANTRIKSH SHARMA (2020A7PS1691P)
#     4. KAUSTAB CHOUDHURY (2020A7PS0013P)
#     5. SHASHANK SHREEDHAR BHATT (2020A7PS0078P)

# This rule simply makes the executable for our compiler, which then has to be executed on the terminal 
# in the following format: ./compiler testcase_file.txt assembly_code.asm. 
# Here, testcase_file.txt refers to the file on which our compiler is to be run
# assembly_code.asm refers to the assembly code file that is generated.
# Obviously, these names can be changed as per the user's convenience.
compiler : SymbolTable.c SymbolTable.h SymbolTableDef.h Hashing.c TheAST.c TheAST.h TheASTDef.h IntegratedParser.c IntegratedParser.h IntegratedParserDef.h lexer.c lexerDef.h lexer.h macros.h stackADT.c First_Follow_Set.c setADT.c setADT.h commons.h driver.c grammar_modified.txt
	gcc -o compiler driver.c
