#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

/* Constant Declarations */
#define TAB '\t'
#define MAX_TOKEN_LENGTH 10

/* Variable Declarations */
char Look;  /* Lookahead Character */

/* Read New Character From Input Stream */
void GetChar() {
    Look = getchar();
}

/* Function Prototypes */
void Expression(void);  // Add this line
void Term(void);
void Factor(void);
void Ident(void);
void Assignment(void);

/* Report an Error */
void Error(const char* s) {
    printf("\nError: %s.\n", s);
}

/* Report Error and Halt */
void Abort(const char* s) {
    Error(s);
    exit(1);
}

/* Report What Was Expected */
void Expected(const char* s) {
    char message[100];
    snprintf(message, sizeof(message), "%s Expected", s);
    Abort(message);
}

bool IsAddOp(char c) {
    return c == '+' || c == '-';
}
bool IsMulOp(char c) {
    return c == '*' || c == '/';
}

/* Recognize an Alpha Character */
bool IsAlpha(char c) {
    return isalpha(c);
}

/* Recognize a Decimal Digit */
bool IsDigit(char c) {
    return isdigit(c);
}

bool IsAlNum(char c) {
    return IsAlpha(c) || IsDigit(c);
}

bool IsWhite(char c) {
    return c == ' ' || c == '\t';
}

void SkipWhite() {
    while (IsWhite(Look)) {
        GetChar();
    }
}

/* Match a Specific Input Character */
void Match(char x) {
    if (Look == x) {
        GetChar();
        SkipWhite();
    } else {
        char message[3];
        snprintf(message, sizeof(message), "'%c'", x);
        Expected(message);
    }
}


void GetName(char* Token) {
    int i = 0;
    if (!IsAlpha(Look)) {
        Expected("Name");
    }
    while (IsAlNum(Look) && i < MAX_TOKEN_LENGTH - 1) {
        Token[i++] = toupper(Look);
        GetChar();
        SkipWhite();
    }
    Token[i] = '\0';  // Null-terminate the string
}

void GetNum(char* Value) {
    int i = 0;
    if (!IsDigit(Look)) {
        Expected("Integer");
    }
    while (IsDigit(Look) && i < MAX_TOKEN_LENGTH - 1) {
        Value[i++] = Look;
        GetChar();
        SkipWhite();
    }
    Value[i] = '\0';  // Null-terminate the string
}

/* Output a String with Tab */
void Emit(const char* s) {
    printf("%c%s", TAB, s);
}

/* Output a String with Tab and CRLF */
void EmitLn(const char* s) {
    Emit(s);
    printf("\n");
}

void Ident() {
    char Name[MAX_TOKEN_LENGTH];
    GetName(Name);
    if (Look == '(') {
        Match('(');
        Match(')');
        char instruction[20];
        snprintf(instruction, sizeof(instruction), "BSR %s", Name);
        EmitLn(instruction);
    } else {
        char instruction[20];
        snprintf(instruction, sizeof(instruction), "MOVE %s(PC)D0", Name);
        EmitLn(instruction);
    }
}

void Factor () {
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (IsAlpha(Look)) {
        Ident();
    } else {
        char num[MAX_TOKEN_LENGTH];
        GetNum(num);
        char instruction[20];
        snprintf(instruction, sizeof(instruction), "MOVE #%s,D0",num);
        EmitLn(instruction);
    }
}

void Multiply() {
    Match('*');
    Factor();
    EmitLn("MULS (SP)+, D0");
}

void Divide() {
    Match('/');
    Factor();
    EmitLn("MOVE (SP)+, D1");
    EmitLn("DIVS D0, D1");
    EmitLn("MOVE D1, D0");
}

void Term() {
   Factor();
   while (Look == '*' || Look == '/') {
    EmitLn("MOVE D0,-(SP)");
    switch (Look) {
        case '*': Multiply(); break;
        case '/': Divide(); break;
    default: Expected("Mulop");
    }
   }
}

void Add() {
    Match('+');
    Term();
    EmitLn("ADD (SP)+,D0");
}

void Subtract() {
    Match('-');
    Term();
    EmitLn("SUB (SP)+,D0");
    EmitLn("NEG D0");
}

void Expression() {
    if (IsAddOp(Look)) {
        EmitLn("CLR D0");
    } else {
        Term();
    }
   while (Look == '+' || Look == '-') {
    EmitLn("MOVE D0,-(SP)");
    switch (Look) {
        case '+': Add(); break;
        case '-': Subtract(); break;
    default: Expected("Addop");
    }
   }
}

void Assignment() {
    char Name[MAX_TOKEN_LENGTH];
    GetName(Name);
    Match('=');
    Expression();
    char instruction[20];
    snprintf(instruction, sizeof(instruction), "LEA %s(PC),A0", Name);
    EmitLn(instruction);
    EmitLn("MOVE D0,(A0)");
}

/* Initialize */
void Init() {
    GetChar();
    SkipWhite();
    Assignment();
    if (Look != '\n') {
        Expected("Newline");
    }
}

/* Main Program */
int main() {
    Init();
    return 0;
}