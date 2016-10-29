/*
 * tokenizer.c
 * Written by Rohit Tikyani
 * for CS211, Assignment 1
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/*
 * An array of token type names which corresponds to the indices of the token_type enumeration.
 */
char *TOKENS[] = {
        "whitespace", "decimal", "hexadecimal", "octal", "float"
};

/*
 * A type of token.
 */
enum token_type {
    WHITESPACE, DECIMAL, HEXADECIMAL, OCTAL, FLOAT
};

/*
 * Tokenizer type. You need to fill in the type as part of your implementation.
 */
typedef struct TokenizerT_ {
    char *input;
    char *input_index;
} TokenizerT;

/*
* Function declaration statements
*/
TokenizerT *TKCreate(char *);

void TKDestroy(TokenizerT *);

char *TKGetNextToken(TokenizerT *);

enum token_type get_token_type(char *, char *);

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 *
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */
TokenizerT *TKCreate(char *input) {
    TokenizerT *tokenizer = malloc(sizeof(TokenizerT));
    tokenizer->input = strdup(input);
    tokenizer->input_index = tokenizer->input;
    return tokenizer;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */
void TKDestroy(TokenizerT *tokenizer) {
    free(tokenizer->input);
    free(tokenizer);
}

/*
 * This function determines the length of the given token.
 */
size_t get_token_length(char *current_index, char *end_index, enum token_type type) {

    char *start_index = current_index;

    while (current_index < end_index) {

        char current = *current_index;

        if (type == HEXADECIMAL) {
            if (!isxdigit(current) && tolower(current) != 'x') {
                break;
            }
        } else if (type == OCTAL) {
            if (!isdigit(current)) {
                break;
            }

            int value = current - '0';

            if (value > 7) {
                break;
            }
        } else if (type == DECIMAL) {

            if (current == '.') {
                type = FLOAT;
            } else if (!isdigit(current)) {
                break;
            }
        } else if (type == FLOAT) {
            if (!isdigit(current) && current != '-' && current != 'e') {
                break;
            }
        }

        current_index += sizeof(char);
    }
    if (type == FLOAT) {
        return current_index - start_index + 100000;
    }
    return current_index - start_index;
}

/*
 * This function determines the type of token and returns it as a token_type.
 */
enum token_type get_token_type(char *current_index, char *end_index) {
    char current = *current_index;

    if (isspace(current)) {
        return WHITESPACE;
    }

    if (isdigit(current)) {

        if (current == '0') {

            char *next_index = current_index + sizeof(char);

            if (next_index < end_index) {
                char next = *next_index;

                if (tolower(next) == 'x') {
                    next = *(next_index + 1);
                    if (isxdigit(next)) {
                        return HEXADECIMAL;
                    }
                }

                if (isdigit(next)) {
                    int value = next - '0';

                    if (value <= 7) {
                        return OCTAL;
                    }
                }

            }
        }

        return DECIMAL;
    }

    return (enum token_type) NULL;
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */
char *TKGetNextToken(TokenizerT *tokenizer) {
    char *start_index = tokenizer->input_index;
    char *end_index = start_index + strlen(tokenizer->input_index);

    if (start_index == end_index) {
        return NULL;
    }

    enum token_type type = get_token_type(start_index, end_index);

    if (type == WHITESPACE) {
        tokenizer->input_index = start_index + sizeof(char);
        return TKGetNextToken(tokenizer);
    }

    size_t token_size = get_token_length(start_index, end_index, type);
    if (token_size > 99999) {
        type = FLOAT;
        token_size = token_size - 100000;
    }

    char *token = malloc(token_size);
    memcpy(token, start_index, token_size);
    token[token_size] = '\0';

    printf("%s", TOKENS[type]);

    tokenizer->input_index = start_index + token_size;

    return token;
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */
int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Invalid arguments.\n");
        return EXIT_FAILURE;
    }

    char *string = argv[1];

    TokenizerT *tokenizer = TKCreate(string);

    char *token = TKGetNextToken(tokenizer);

    while (token != NULL) {
        printf(" %s\n", token);
        fflush(stdout);
        free(token);

        token = TKGetNextToken(tokenizer);
    }

    TKDestroy(tokenizer);

    return EXIT_SUCCESS;
}