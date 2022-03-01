enum tokens {
    // Identifier
    IDENTIFIER,

    // Arithmetic Operators
    ADD_OP,
    INC_OP,
    HYPHEN,
    DEC_OP,
    MUL_OP,
    EXP_OP,
    FW_SLASH,
    FLR_OP,
    MOD_OP,

    // Bitwise Operators
    BITWISE_AND_OP,
    BITWISE_OR_OP,
    BITWISE_NOT_OP,
    BITWISE_XOR_OP,
    BITWISE_LEFT_OP,
    BITWISE_RIGHT_OP,

    // Logical Operators
    LOGIC_AND_OP,
    LOGIC_OR_OP,
    LOGIC_NOT_OP,

    // Assignment
    EQ_SIGN,

    // Relational Operators
    EQ_OP,
    NOT_EQ_OP,
    RANGLE,
    GT_EQ_OP,
    LANGLE,
    LT_EQ_OP,

    // Delimiters
    LPAREN,
    RPAREN,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    COMMA,
    LBRACKET,
    RBRACKET,
    DOT,
    COMMENT_BEGIN,
    COMMENT,
    COMMENT_END,
    LCBRACE,
    RCBRACE,

    // Literals
    INT_LITERAL,
    FLOAT_LITERAL,
    STR_LITERAL,
    INVALID_LITERAL,

    // Keywords
    PRIM_DECLARE_KW,
    LIST_DECLARE_KW,
    ELEM_KW,
    ASSIGN_KW,
    IF_KW,
    ELIF_KW,
    ELSE_KW,
    BREAK_KW,
    CONTINUE_KW,
    SWITCH_KW,
    CASE_KW,
    WHILE_KW,
    TYPE_KW,
    IDENTIFIER_KW,
    EXPR_KW,
    SIZE_KW,
    EVAL_KW,
    CONST_KW,

    INT_KW,
    FLOAT_KW,
    STRING_KW,
    BOOL_KW,

    TRUE_KW,
    FALSE_KW,

    // Reserved Words
    AND_RW,
    OR_RW,
    NOT_RW,
    SIZEOF_RW,
    BEGIN_RW,
    END_RW,
    LIST_RW,

    // Noise Words
    THEN_NW,
    TO_NW,
    INITIALIZE_NW,
    WITH_NW,
    PROCEED_NW,

    // newly added ones
    SEMICOLON,
    INTEGER_DIVISION_OP,
    DO_KW,
    DOUBLE_KW,
    FOR_KW,
    GOTO_KW,
};
