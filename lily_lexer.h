#ifndef LILY_LEXER_H
# define LILY_LEXER_H

# include <stdio.h>

# include "lily_raiser.h"
# include "lily_symtab.h"

typedef enum {
    tk_left_parenth,
    tk_right_parenth,
    tk_comma,
    tk_left_curly,
    tk_right_curly,
    tk_colon,
    tk_left_bracket,
    tk_right_bracket,
    tk_equal,
    tk_eq_eq,
    tk_not,
    tk_not_eq,
    tk_multiply,
    tk_multiply_eq,
    tk_divide,
    tk_divide_eq,
    tk_plus,
    tk_plus_eq,
    tk_minus,
    tk_minus_eq,
    /* Note: Lexer assumes that less and greater have x_eq, x_shift, and
       x_shift_eq are 1, 2, and 3 places after the less/greater token.
       You can move lt/gt so long as all four tokens are moved together. */
    tk_lt,
    tk_lt_eq,
    tk_left_shift,
    tk_left_shift_eq,
    tk_gt,
    tk_gt_eq,
    tk_right_shift,
    tk_right_shift_eq,
    tk_word,
    tk_double_quote,
    tk_integer,
    tk_number,
    tk_dot,
    tk_bitwise_and,
    tk_logical_and,
    tk_bitwise_or,
    tk_logical_or,
    tk_typecast_parenth,
    tk_two_dots,
    tk_three_dots,
    tk_invalid,
    tk_end_tag,
    tk_eof
} lily_token;

typedef enum {
    lm_from_file,
    lm_from_str
} lily_lexer_mode;

typedef struct {
    int hit_eof;
    FILE *lex_file;
    char *filename;
    int line_num;
    char *ch_class;
    char *lex_buffer;
    char *save_buffer;
    int lex_bufpos;
    int lex_bufend;
    int lex_bufsize;
    int label_size;
    char *label;
    lily_token token;
    lily_lexer_mode mode;
    lily_value value;
    lily_raiser *raiser;
} lily_lex_state;

void lily_free_lex_state(lily_lex_state *);
void lily_lexer(lily_lex_state *);
void lily_lexer_handle_page_data(lily_lex_state *);
void lily_load_file(lily_lex_state *, char *);
int lily_load_str(lily_lex_state *, char *);
lily_lex_state *lily_new_lex_state(lily_raiser *);
char *tokname(lily_token);

#endif
