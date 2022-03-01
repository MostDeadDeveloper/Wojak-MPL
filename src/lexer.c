#include "globals.h"
#include "tokens.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#define ARRAY_MAX_SIZE 1024

#define ACTION_normal_transition(input, to_state) \
    ACTION_transition_N_single(next_char == input, to_state)

#define ACTION_transition_N_single(boolean_condition, to_state) \
    if(boolean_condition) \
        do { \
            lexeme[forward_lexeme_ptr - begin_lexeme_ptr] = next_char; \
            current_state = to_state; \
        } while(0)

#define ACTION_FINAL_STATE(token_str, rep) \
    do { \
        strcpy(LEX_CHAR_VALUE, lexeme); \
        strcpy(lex_token_desc, token_str); \
        lex_token = rep; \
        return; \
    } while(0)

#define ACTION_retract_accept(token_str, rep) \
    do { \
        retract_character_by_1(1); \
        ACTION_FINAL_STATE(token_str, rep); \
    } while(0)

#define ACTION_final_state_with_transition(transitions, accept) \
    get_next_char(); \
    transitions; \
    else accept; \
    break

#define ACTION_transition_for_words(transitions) \
    ACTION_final_state_with_transition( \
        transitions, \
        do { retract_character_by_1(1); current_state = 1; } while(0))

#define ACTION_word_or_transition_for_identifiers(token_str, rep) \
    ACTION_final_state_with_transition( \
        ACTION_transition_N_single(isalnum(next_char) || next_char == '_', 1), \
        ACTION_retract_accept(token_str, rep))

void initialize_program();
void check_next_line();
void get_next_char();
void get_next_char_nonblank();
void retract_character_by_1(uint16_t);
void empty_out_list(uint8_t);

extern FILE *input_file;
extern FILE *sym_file;

char LEX_CHAR_VALUE[80];
char lex_token_desc[80];
int8_t lex_token;
char next_char;

uint8_t current_state;

char buffer[ARRAY_MAX_SIZE * 2];

bool should_empty_out_list[2];
uint16_t begin_lexeme_ptr;

uint16_t forward_lexeme_ptr;

int main(int argc, char *argv[]) {
    // Check if arguments are properly given
    if(argc < 2) {
        printf("Error: no given files.\n");
        printf("Usage is: wojak filename.wojak\n");
        exit(EXIT_FAILURE);
    }

    // Open input file in read mode
    if((input_file = fopen(argv[1], "r")) == NULL) {
        printf("Error: cannot open file with name %s. Please check if this file exists"
            " or if the program cannot access it.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // Extract the file name only, use it for output file naming convention
    char filename[256] = { '\0' };
    for(int i = 0; i < strlen(argv[1]); i++) {
        if(argv[1][i] == '.')
            break;
        filename[i] = argv[1][i];
    }

    // Open symbol table output file
    if((sym_file = fopen(strcat(filename, ".symwojak"), "w")) == NULL) {
        printf("Error: cannot create symbol table output file %s. Please check"
            " if the program cannot access it.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // Initialize the lexer
    initialize_program();

    // Print all tokens in the file
    check_next_line();
    while(lex_token != EOF) {
        printf("Lexeme is: %s, Token is: %s\n", LEX_CHAR_VALUE, lex_token_desc);
        fprintf(sym_file, "%s,%d,%s\n", LEX_CHAR_VALUE, lex_token, lex_token_desc);
        check_next_line();
    }

    // Close the files
    fclose(sym_file);
    fclose(input_file);

}
void initialize_program() {
    // Initialize pointer at end of second half, which will reset later for convenience
    forward_lexeme_ptr = ARRAY_MAX_SIZE * 2 - 2;

    // BUFFER STUFF
    // Initialize EOF sentinels
    buffer[ARRAY_MAX_SIZE - 1] = EOF;
    buffer[ARRAY_MAX_SIZE * 2 - 1] = EOF;
    // Initialize reset flags
    should_empty_out_list[0] = 1;
    should_empty_out_list[1] = 1;
}
void check_next_line() {
    current_state = 0;

    char lexeme[80] = { '\0' };

    while(1) {
        switch(current_state) {
            case 0:
                get_next_char_nonblank();
                begin_lexeme_ptr = forward_lexeme_ptr;

                ACTION_normal_transition('+', 2);
                else ACTION_normal_transition('-', 4);
                else ACTION_normal_transition('*', 6);
                else ACTION_normal_transition('/', 8);
                else ACTION_normal_transition('%', 10);
                else ACTION_normal_transition('&', 11);
                else ACTION_normal_transition('|', 13);
                else ACTION_normal_transition('~', 15);
                else ACTION_normal_transition('!', 16);
                else ACTION_normal_transition('^', 17);
                else ACTION_normal_transition('=', 19);
                else ACTION_normal_transition('>', 21);
                else ACTION_normal_transition('<', 24);
                else ACTION_normal_transition('(', 167);
                else ACTION_normal_transition(')', 168);
                else ACTION_normal_transition('\'', 169);
                else ACTION_normal_transition('\"', 170);
                else ACTION_normal_transition(',', 171);
                else ACTION_normal_transition('[', 172);
                else ACTION_normal_transition(']', 173);
                else ACTION_normal_transition('.', 174);
                else ACTION_normal_transition('{', 175);
                else ACTION_normal_transition('}', 176);
                else ACTION_normal_transition(';',208);
                // Start of keywords
                else ACTION_normal_transition('p', 27);
                else ACTION_normal_transition('D', 32);
                else ACTION_normal_transition('l', 39);
                else ACTION_normal_transition('E', 51);
                else ACTION_normal_transition('a', 55);
                else ACTION_normal_transition('I', 61);
                else ACTION_normal_transition('B', 67);
                else ACTION_normal_transition('C', 72);
                else ACTION_normal_transition('S', 80);
                else ACTION_normal_transition('W', 90);
                else ACTION_normal_transition('T', 95);
                else ACTION_normal_transition('F', 190);
                else ACTION_normal_transition('B', 195);
                else ACTION_normal_transition('G', 214);
                // Start of identifier
                else ACTION_transition_N_single(isalpha(next_char) || next_char == '_', 1);
                // Start of integer or float literal
                else ACTION_transition_N_single(isdigit(next_char), 177);
                // End of File
                else ACTION_normal_transition(EOF, 206);
                // Invalid token
                else printf("Unrecognized token %c\n.", next_char);
                break;
            case 1: ACTION_final_state_with_transition(
                        ACTION_transition_N_single(
                            isalnum(next_char) || next_char == '_', 1),
                        ACTION_retract_accept("Identifier", IDENTIFIER));
            case 2: ACTION_final_state_with_transition(
                        ACTION_normal_transition('+', 3),
                        ACTION_retract_accept("OPERATOR_PLUS", ADD_OP));
            case 3: ACTION_FINAL_STATE("OPERATOR_INCREMENT", INC_OP);
            case 4: ACTION_final_state_with_transition(
                        ACTION_normal_transition('-', 5),
                        ACTION_retract_accept("OPERATOR_MINUS", HYPHEN));
            case 5: ACTION_final_state_with_transition(
                        ACTION_normal_transition('>', 166),
                        ACTION_retract_accept("OPERATOR_DECREMENT", DEC_OP));
            case 6: ACTION_final_state_with_transition(
                        ACTION_normal_transition('*', 7),
                        ACTION_retract_accept("OPERATOR_MULTIPIPLY", MUL_OP));
            case 7: ACTION_FINAL_STATE("OPERATOR_EXPONENT",EXP_OP);
            case 8: ACTION_FINAL_STATE("OPERATOR_DIVIDE", FW_SLASH);
            case 10: ACTION_FINAL_STATE("Operator_MODULO", MOD_OP);
            case 11: ACTION_final_state_with_transition(
                        ACTION_normal_transition('&', 12),
                        ACTION_retract_accept("OPERATOR_BITWISE_AND", BITWISE_AND_OP));
            case 12: ACTION_FINAL_STATE("OPERATOR_LOGICAL_AND", LOGIC_AND_OP);
            case 13: ACTION_final_state_with_transition(
                        ACTION_normal_transition('|', 14),
                        ACTION_retract_accept("OPERATOR_BITWISE_OR", BITWISE_OR_OP));
            case 14: ACTION_FINAL_STATE("OPERATOR_LOGICAL_OR", LOGIC_OR_OP);
            case 15: ACTION_final_state_with_transition(
                        ACTION_normal_transition('/',209),
                        ACTION_retract_accept("ELEMENT_OPERATOR",BITWISE_NOT_OP);
                    );
            /*case 15: ACTION_FINAL_STATE("Bitwise NOT Operator", BITWISE_NOT_OP);*/
            case 16: ACTION_final_state_with_transition(
                        ACTION_normal_transition('=', 18),
                        ACTION_retract_accept("OPERATOR_LOGICAL_NOT", LOGIC_NOT_OP));
            case 17: ACTION_FINAL_STATE("OPERATOR_BITWISE_XOR", BITWISE_XOR_OP);
            case 18: ACTION_FINAL_STATE("OPERATOR_RELATIONAL_NOT_EQUAL", NOT_EQ_OP);
            case 19: ACTION_final_state_with_transition(
                        ACTION_normal_transition('=', 20),
                        ACTION_retract_accept("OPERATOR_ASSIGNMENT", EQ_SIGN));
            case 20: ACTION_FINAL_STATE("OPERATOR_RELATIONAL_EQUAL", EQ_OP);
            case 21: ACTION_final_state_with_transition(
                        ACTION_normal_transition('=', 22);
                        else ACTION_normal_transition('>', 23),
                        ACTION_retract_accept("OPERATOR_GREATER_THAN", RBRACKET));
            case 22: ACTION_FINAL_STATE("OPERATOR_GREATER_THAN_EQUAL", GT_EQ_OP);
            case 23: ACTION_FINAL_STATE("Bitwise Shift Right Operator", BITWISE_RIGHT_OP);
            case 24: ACTION_final_state_with_transition(
                        ACTION_normal_transition('=', 25);
                        else ACTION_normal_transition('<', 26);
                        else ACTION_normal_transition('!', 163),
                        ACTION_retract_accept("OPERATOR_LESS_THAN", LBRACKET));
            case 25: ACTION_FINAL_STATE("OPERATOR_LESS_THAN_EQUAL", LT_EQ_OP);
            case 26: ACTION_FINAL_STATE("Bitwise Shift Left Operator", BITWISE_LEFT_OP);
            case 27: ACTION_transition_for_words(ACTION_normal_transition('r', 28));
            case 28: ACTION_transition_for_words(ACTION_normal_transition('i', 29);
                        else ACTION_normal_transition('o', 158));
            case 29: ACTION_transition_for_words(ACTION_normal_transition('m', 30));
            case 30: ACTION_transition_for_words(ACTION_normal_transition('_', 31));
            case 31: ACTION_transition_for_words(ACTION_normal_transition('d', 32));
            case 32: ACTION_transition_for_words(
                        ACTION_normal_transition('E', 33);
                        else ACTION_normal_transition('O',210));
            case 33: ACTION_transition_for_words(ACTION_normal_transition('F', 34));
            case 34: ACTION_transition_for_words(ACTION_normal_transition('A', 35));
            case 35: ACTION_transition_for_words(ACTION_normal_transition('U', 36));
            case 36: ACTION_transition_for_words(ACTION_normal_transition('L', 37));
            case 37: ACTION_transition_for_words(ACTION_normal_transition('T', 38));
            case 38: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_DEFAULT", PRIM_DECLARE_KW);
            case 39: ACTION_transition_for_words(ACTION_normal_transition('i', 40));
            case 40: ACTION_transition_for_words(ACTION_normal_transition('s', 41));
            case 41: ACTION_final_state_with_transition(
                        ACTION_normal_transition('t', 42);
                        else ACTION_transition_N_single(isalnum(next_char) || '_', 1),
                        ACTION_retract_accept("list", LIST_RW)); 
            case 42: ACTION_transition_for_words(ACTION_normal_transition('_', 43));
            case 43: ACTION_transition_for_words(ACTION_normal_transition('d', 44));
            case 44: ACTION_transition_for_words(ACTION_normal_transition('e', 45));
            case 45: ACTION_transition_for_words(ACTION_normal_transition('c', 46));
            case 46: ACTION_transition_for_words(ACTION_normal_transition('l', 47));
            case 47: ACTION_transition_for_words(ACTION_normal_transition('a', 48));
            case 48: ACTION_transition_for_words(ACTION_normal_transition('r', 49));
            case 49: ACTION_transition_for_words(ACTION_normal_transition('e', 50));
            case 50: ACTION_word_or_transition_for_identifiers(
                        "list_declare Keyword", LIST_DECLARE_KW);
            case 51: ACTION_transition_for_words(
                        ACTION_normal_transition('L', 52);
                        else ACTION_normal_transition('x', 109);
                        else ACTION_normal_transition('v', 115);
                        else ACTION_normal_transition('n', 133));
            case 52: ACTION_transition_for_words(
                        ACTION_normal_transition('e', 53);
                        else ACTION_normal_transition('i', 63);
                        else ACTION_normal_transition('S', 65));
            case 53: ACTION_transition_for_words(ACTION_normal_transition('m', 54));
            case 54: ACTION_word_or_transition_for_identifiers(
                        "elem Keyword", ELEM_KW);
            case 55: ACTION_transition_for_words(ACTION_normal_transition('s', 56);
                        else ACTION_normal_transition('n', 120));
            case 56: ACTION_transition_for_words(ACTION_normal_transition('s', 57));
            case 57: ACTION_transition_for_words(ACTION_normal_transition('i', 58));
            case 58: ACTION_transition_for_words(ACTION_normal_transition('g', 59));
            case 59: ACTION_transition_for_words(ACTION_normal_transition('n', 60));
            case 60: ACTION_word_or_transition_for_identifiers(
                        "assign Keyword", ASSIGN_KW);
            case 61: ACTION_transition_for_words(
                        ACTION_normal_transition('F', 62);
                        else ACTION_normal_transition('d', 100);
                        else ACTION_normal_transition('N', 146));
            case 62: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_IF", IF_KW);
            case 63: ACTION_transition_for_words(ACTION_normal_transition('f', 64));
            case 64: ACTION_word_or_transition_for_identifiers(
                        "elif Keyword", ELIF_KW);
            case 65: ACTION_transition_for_words(ACTION_normal_transition('E', 66));
            case 66: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_ELSE", ELSE_KW);
            case 67: ACTION_transition_for_words(ACTION_normal_transition('R', 68);
                        else ACTION_normal_transition('e', 129));
            case 68: ACTION_transition_for_words(ACTION_normal_transition('E', 69));
            case 69: ACTION_transition_for_words(ACTION_normal_transition('A', 70));
            case 70: ACTION_transition_for_words(
                             ACTION_normal_transition('K', 71);
                             else ACTION_normal_transition('S',118));
            case 71: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_BREAK", BREAK_KW);
            case 72: ACTION_transition_for_words(
                        ACTION_normal_transition('O', 73);
                        else ACTION_normal_transition('A', 70);
                        else ACTION_normal_transition('N', 86));
            case 73: ACTION_transition_for_words(ACTION_normal_transition('N', 74));
            case 74: ACTION_transition_for_words(
                        ACTION_normal_transition('T', 75);
                        else ACTION_normal_transition('S', 118));
            case 75: ACTION_transition_for_words(ACTION_normal_transition('I', 76));
            case 76: ACTION_transition_for_words(ACTION_normal_transition('N', 77));
            case 77: ACTION_transition_for_words(ACTION_normal_transition('U', 78));
            case 78: ACTION_transition_for_words(ACTION_normal_transition('E', 79));
            case 79: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_CONTINUE", CONTINUE_KW);
            case 80: ACTION_transition_for_words(
                        ACTION_normal_transition('W', 81);
                        else ACTION_normal_transition('i', 112);
                        else ACTION_normal_transition('T', 185));
            case 81: ACTION_transition_for_words(ACTION_normal_transition('I', 82));
            case 82: ACTION_transition_for_words(ACTION_normal_transition('T', 83));
            case 83: ACTION_transition_for_words(ACTION_normal_transition('C', 84));
            case 84: ACTION_transition_for_words(ACTION_normal_transition('H', 85));
            case 85: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_SWITCH", SWITCH_KW
                    );
            case 86: ACTION_transition_for_words(ACTION_normal_transition('s', 87));
            case 87: ACTION_transition_for_words(ACTION_normal_transition('e', 88));
            case 89: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_CASE", CASE_KW);
            case 90: ACTION_transition_for_words(
                        ACTION_normal_transition('H', 91);
                        else ACTION_normal_transition('i', 155));
            case 91: ACTION_transition_for_words(ACTION_normal_transition('I', 92));
            case 92: ACTION_transition_for_words(ACTION_normal_transition('L', 93));
            case 93: ACTION_transition_for_words(ACTION_normal_transition('E', 94));
            case 94: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_WHILE", WHILE_KW);
            case 95: ACTION_transition_for_words( 
                        ACTION_normal_transition('y', 96);
                        else ACTION_normal_transition('h', 142);
                        else ACTION_normal_transition('o', 145);
                        else ACTION_normal_transition('R', 199));
            case 96: ACTION_transition_for_words(ACTION_normal_transition('p', 97));
            case 97: ACTION_transition_for_words(ACTION_normal_transition('e', 98));
            case 98: ACTION_word_or_transition_for_identifiers(
                        "type Keyword", TYPE_KW);
            // state 99 skipped
            case 100: ACTION_transition_for_words(ACTION_normal_transition('e', 101));
            case 101: ACTION_transition_for_words(ACTION_normal_transition('n', 102));
            case 102: ACTION_transition_for_words(ACTION_normal_transition('t', 103));
            case 103: ACTION_transition_for_words(ACTION_normal_transition('i', 104));
            case 104: ACTION_transition_for_words(ACTION_normal_transition('f', 105));
            case 105: ACTION_transition_for_words(ACTION_normal_transition('i', 106));
            case 106: ACTION_transition_for_words(ACTION_normal_transition('e', 107));
            case 107: ACTION_transition_for_words(ACTION_normal_transition('r', 108));
            case 108: ACTION_word_or_transition_for_identifiers(
                        "identifier Keyword", IDENTIFIER_KW);
            case 109: ACTION_transition_for_words(ACTION_normal_transition('p', 110));
            case 110: ACTION_transition_for_words(ACTION_normal_transition('r', 111));
            case 111: ACTION_word_or_transition_for_identifiers(
                        "expr Keyword", EXPR_KW);
            case 112: ACTION_transition_for_words(ACTION_normal_transition('z', 113));
            case 113: ACTION_transition_for_words(ACTION_normal_transition('e', 114));
            case 114: ACTION_final_state_with_transition(
                        ACTION_normal_transition('o', 127);
                        else ACTION_transition_N_single(isalnum(next_char) || next_char == '_', 1),
                        ACTION_retract_accept("size Keyword", SIZE_KW));
            case 115: ACTION_transition_for_words(ACTION_normal_transition('a', 116));
            case 116: ACTION_transition_for_words(ACTION_normal_transition('l', 117));
            case 117: ACTION_word_or_transition_for_identifiers(
                        "eval Keyword", EVAL_KW);
            case 118: ACTION_transition_for_words(
                        ACTION_normal_transition('T', 119);
                        else ACTION_normal_transition('E',89)
                    );
            case 119: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_CONST", CONST_KW);
            case 120: ACTION_transition_for_words(ACTION_normal_transition('d', 121));
            case 121: ACTION_word_or_transition_for_identifiers(
                        "and Reserved word", AND_RW);
            case 122: ACTION_transition_for_words(ACTION_normal_transition('r', 123));
            case 123: ACTION_word_or_transition_for_identifiers(
                        "or Reserved Word", OR_RW);
            case 124: ACTION_transition_for_words(ACTION_normal_transition('o', 125));
            case 125: ACTION_transition_for_words(ACTION_normal_transition('t', 126));
            case 126: ACTION_word_or_transition_for_identifiers(
                        "not Reserved Word", NOT_RW);
            case 127: ACTION_transition_for_words(ACTION_normal_transition('f', 128));
            case 128: ACTION_word_or_transition_for_identifiers(
                        "sizeof Reserved Word", SIZEOF_RW);
            case 129: ACTION_transition_for_words(ACTION_normal_transition('g', 130));
            case 130: ACTION_transition_for_words(ACTION_normal_transition('i', 131));
            case 131: ACTION_transition_for_words(ACTION_normal_transition('n', 132));
            case 132: ACTION_word_or_transition_for_identifiers(
                        "begin Reserved Word", BEGIN_RW);
            case 133: ACTION_transition_for_words(ACTION_normal_transition('d', 134));
            case 134: ACTION_word_or_transition_for_identifiers(
                        "end Reserved Word", END_RW);
            case 142: ACTION_transition_for_words(
                        ACTION_normal_transition('e', 143));
            case 143: ACTION_transition_for_words(ACTION_normal_transition('n', 144));
            case 144: ACTION_word_or_transition_for_identifiers(
                        "then Noise Word", THEN_NW);
            case 145: ACTION_word_or_transition_for_identifiers(
                        "to Noise Word", TO_NW);
            case 146: ACTION_transition_for_words(
                        ACTION_normal_transition('i', 147);
                        else ACTION_normal_transition('T', 184));
            case 147: ACTION_transition_for_words(ACTION_normal_transition('t', 148));
            case 148: ACTION_transition_for_words(ACTION_normal_transition('i', 149));
            case 149: ACTION_transition_for_words(ACTION_normal_transition('a', 150));
            case 150: ACTION_transition_for_words(ACTION_normal_transition('l', 151));
            case 151: ACTION_transition_for_words(ACTION_normal_transition('i', 152));
            case 152: ACTION_transition_for_words(ACTION_normal_transition('z', 153));
            case 153: ACTION_transition_for_words(ACTION_normal_transition('e', 154));
            case 154: ACTION_word_or_transition_for_identifiers(
                        "initialize Noise Word", INITIALIZE_NW);
            case 155: ACTION_transition_for_words(ACTION_normal_transition('t', 156));
            case 156: ACTION_transition_for_words(ACTION_normal_transition('h', 157));
            case 157: ACTION_word_or_transition_for_identifiers(
                        "with Noise Word", WITH_NW);
            case 158: ACTION_transition_for_words(ACTION_normal_transition('c', 159));
            case 159: ACTION_transition_for_words(ACTION_normal_transition('e', 160));
            case 160: ACTION_transition_for_words(ACTION_normal_transition('e', 161));
            case 161: ACTION_transition_for_words(ACTION_normal_transition('d', 162));
            case 162: ACTION_word_or_transition_for_identifiers(
                        "proceed Noise Word", PROCEED_NW);
            case 163: ACTION_normal_transition('-', 164);
            case 164: ACTION_normal_transition('-', 165);
            case 165: {
                get_next_char();
                ACTION_normal_transition('-', 166);
                else ACTION_normal_transition(EOF, 183);
                else ACTION_transition_N_single(next_char != '-', 165);
                break;
            }
            case 166: {
                get_next_char();
                ACTION_normal_transition('-', 181);
                else ACTION_normal_transition(EOF, 183);
                else ACTION_transition_N_single(next_char != '-', 165);
                break;
            }
            case 167: ACTION_FINAL_STATE("Left Parenthesis", LPAREN);
            case 168: ACTION_FINAL_STATE("Right Parenthesis", RPAREN);
            case 169: get_next_char();
                      ACTION_transition_N_single(next_char != '\'', 169);
                      else ACTION_normal_transition('\'', 180);
                      break;
            case 170: get_next_char();
                      ACTION_transition_N_single(next_char != '\"', 170);
                      else ACTION_normal_transition('\"', 180);
                      break;
            case 171: ACTION_FINAL_STATE("Comma", COMMA);
            case 172: ACTION_FINAL_STATE("Left Square Bracket", LBRACKET);
            case 173: ACTION_FINAL_STATE("Right Square Bracket", RBRACKET);
            case 174: ACTION_FINAL_STATE("Dot", DOT);
            case 175: ACTION_FINAL_STATE("Left Curly Brace", LCBRACE);
            case 176: ACTION_FINAL_STATE("Right Curly Brace", RCBRACE);
            case 177: ACTION_final_state_with_transition(
                        ACTION_transition_N_single(isdigit(next_char), 177);
                        else ACTION_normal_transition('.', 178),
                        ACTION_retract_accept("Integer Literal", INT_LITERAL));
            case 178: ACTION_final_state_with_transition(
                        ACTION_transition_N_single(isdigit(next_char), 179),
                        { retract_character_by_1(1); lexeme[forward_lexeme_ptr - begin_lexeme_ptr] = '0'; current_state = 177; });
            case 179: ACTION_final_state_with_transition(
                        ACTION_transition_N_single(isdigit(next_char), 179);
                        else ACTION_normal_transition('.', 207),
                        ACTION_retract_accept("Float Literal", FLOAT_LITERAL));
            case 180: ACTION_FINAL_STATE("String Literal", STR_LITERAL);
            case 181: {
                get_next_char();
                ACTION_normal_transition('>', 182);
                else ACTION_normal_transition(EOF, 183);
                else ACTION_transition_N_single(next_char != '>', 165);
                break;
            }
            case 182: ACTION_FINAL_STATE("Comment", COMMENT);
            case 183: {
                // handle incompletes better
                printf("Incomplete comment encountered.\n");
                ACTION_FINAL_STATE("End of File", EOF);
            }
            case 184: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_INT", INT_KW);
            case 185: ACTION_transition_for_words(ACTION_normal_transition('R', 186));
            case 186: ACTION_transition_for_words(ACTION_normal_transition('I', 187));
            case 187: ACTION_transition_for_words(ACTION_normal_transition('N', 188));
            case 188: ACTION_transition_for_words(ACTION_normal_transition('G', 189));
            case 189: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_STRING", STRING_KW);
            case 190: ACTION_transition_for_words(
                        ACTION_normal_transition('O',211);
                        else ACTION_normal_transition('L', 191);
                        else ACTION_normal_transition('A', 202));
            case 191: ACTION_transition_for_words(ACTION_normal_transition('O', 192));
            case 192: ACTION_transition_for_words(ACTION_normal_transition('A', 193));
            case 193: ACTION_transition_for_words(ACTION_normal_transition('T', 194));
            case 194: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_FLOAT", FLOAT_KW);
            case 195: ACTION_transition_for_words(ACTION_normal_transition('O', 196));
            case 196: ACTION_transition_for_words(ACTION_normal_transition('O', 197));
            case 197: ACTION_transition_for_words(ACTION_normal_transition('L', 198));
            case 198: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_BOOL", BOOL_KW);
            case 199: ACTION_transition_for_words(ACTION_normal_transition('U', 200));
            case 200: ACTION_transition_for_words(ACTION_normal_transition('E', 201));
            case 201: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_TRUE", TRUE_KW);
            case 202: ACTION_transition_for_words(ACTION_normal_transition('L', 203));
            case 203: ACTION_transition_for_words(ACTION_normal_transition('S', 204));
            case 204: ACTION_transition_for_words(ACTION_normal_transition('E', 205));
            case 205: ACTION_word_or_transition_for_identifiers(
                        "KEYWORD_FALSE", FALSE_KW);
            case 206: ACTION_FINAL_STATE("End of File", EOF);

            // DEAD STATE for Invalid Float
            case 207: ACTION_final_state_with_transition(
                        ACTION_transition_N_single(isdigit(next_char), 207),
                        ACTION_retract_accept("Invalid. Too Many Decimal Points", INVALID_LITERAL));
            case 208:
                      ACTION_FINAL_STATE("SEMICOLON", SEMICOLON);
            case 209:
                      ACTION_FINAL_STATE("OPERATOR_INTEGER_DIVISION",INTEGER_DIVISION_OP);
            case 210:
                      ACTION_word_or_transition_for_identifiers("KEYWORD_DO",DO_KW);
            case 211:
                      ACTION_transition_for_words(ACTION_normal_transition('O',212));
            case 212:
                      ACTION_transition_for_words(ACTION_normal_transition('R',213));
            case 213:
                      ACTION_word_or_transition_for_identifiers("KEYWORD_FOR",FOR_KW);
            case 214:
                      ACTION_transition_for_words(ACTION_normal_transition('O',215));
            case 215:
                      ACTION_transition_for_words(ACTION_normal_transition('T',216));
            case 216:
                      ACTION_transition_for_words(ACTION_normal_transition('O',217));
            case 217:
                    ACTION_word_or_transition_for_identifiers("KEYWORD_GOTO",FOR_KW);

        }
    }
}
void get_next_char() {
    next_char = buffer[++forward_lexeme_ptr];

    if(next_char == EOF) {

        if(forward_lexeme_ptr == ARRAY_MAX_SIZE - 1) {
            if(should_empty_out_list[1]) empty_out_list(1);
            should_empty_out_list[1] = true;

            next_char = buffer[(forward_lexeme_ptr = ARRAY_MAX_SIZE)];
        }

        else if(forward_lexeme_ptr == ARRAY_MAX_SIZE * 2 - 1) {
            if(should_empty_out_list[0]) empty_out_list(0);
            should_empty_out_list[0] = true;
            next_char = buffer[(forward_lexeme_ptr = 0)];
        }
    }
}
void get_next_char_nonblank() {
    get_next_char();
    while(next_char == ' ' || next_char == '\n')
        get_next_char();
}

void retract_character_by_1(uint16_t steps) {
    if(steps < 0)
        return;

    if(forward_lexeme_ptr - steps < 0) {
        should_empty_out_list[0] = false;

        forward_lexeme_ptr = (ARRAY_MAX_SIZE * 2 - 1) - steps - forward_lexeme_ptr;
        next_char = buffer[forward_lexeme_ptr];
    }

    else if(forward_lexeme_ptr >= ARRAY_MAX_SIZE && forward_lexeme_ptr - steps < ARRAY_MAX_SIZE) {
        should_empty_out_list[1] = false;

        forward_lexeme_ptr = forward_lexeme_ptr - steps - 1;
        next_char = buffer[forward_lexeme_ptr];
    }

    else next_char = buffer[forward_lexeme_ptr -= steps];
}

void empty_out_list(uint8_t half) {
    size_t chars_read = fread(buffer + (ARRAY_MAX_SIZE * (half)),
                              sizeof(char),
                              sizeof(char) * (ARRAY_MAX_SIZE - 1),
                              input_file);
    buffer[ARRAY_MAX_SIZE * (half) + chars_read] = EOF;
}
