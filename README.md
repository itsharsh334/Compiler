## Compiler_Construction_Upgraded
- Compiler constructed for a custom-defined langauge ERPLAG which is a strongly typed language with primitive data types as integer and floating point. It also supports two other data types: boolean and arrays. The language supports arithmetic and boolean expressions. The language supports assignment statements, input/output statements, declarative, conditional, iterative, and function call statements. The language supports the modular implementation of the functionalities. Functions can return multiple values. The function may or may not return a value as well. The scope of the variables is static and the variable is visible only in the block where it is declared. The language is designed for the course CS F363 (Compiler Construction)
- You can see the Compiler in action through the following steps:
    - On any Linux-based Terminal, execute:
      ```
      > make
      ```
    - Then execute the following command:
      ```
      > ./compiler testcase.txt code.asm
      ```
    - In the above command testcase.txt could be chosen from any of the *t[1-6].txt, ts[1-10].txt, c[1-10].txt*. Note that many of them contain lexical, syntactic and semantic errors for testing the robustness of the Compiler; hence, proceed accordingly.
    - *code.asm* is the x86 generated executable code for the corresponding ERPLAG code.
    -  To see the output (in case the test case was lexically, syntactically and semantically correct), execute the following command:
        ```
        > nasm -felf64 code.asm && ld code.o && ./a.out
        ```
- And finally, cheers and shoutout to my teammates **_Hrishikesh, Kaustab, Antriksh and Shashank_** for the wonderful journey and experience...
