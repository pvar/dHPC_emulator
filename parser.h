/**
 * @file parser.h
 * @brief Prototypes, macros, enumerators and global variables...
 *
 * The @c COMMANDS enumerator contains a member for each command supported by the language.
 * The memebers of the enumerator appear in the same order as the corresponding commands, in the
 * @c commands table (see parser.c). The last member of the enumerator is used for assignments
 * (some_variable = some_value) which do not use some special command. I other words, CMD_ASSIGNMENT
 * corresponds to no entry in the \c commands table!
 */

#ifndef PARSER_H
#define PARSER_H

// ------------------------------------------------------------------------------
// INCLUDES
// ------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "cpu_gpu.h"
#include "interpreter.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

int8_t scantable (const uint8_t *table);
void parse_channel (void);
void parse_notes (void);
int16_t parse_expr_s1 (void);

// ------------------------------------------------------------------------------
// ENUMERATORS
// ------------------------------------------------------------------------------

enum COMMANDS {
        CMD_LIST = 0,
        CMD_NEW,
        CMD_RUN,
        CMD_NEXT,
        CMD_LET,
        CMD_IF,
        CMD_GOTO,
        CMD_MPLAY,
        CMD_MSTOP,
        CMD_TEMPO,
        CMD_MUSIC,
        CMD_GOSUB,
        CMD_RETURN,
        CMD_RANDOMIZE,
        CMD_RNDSEED,
        CMD_RST,
        CMD_CLS,
        CMD_REM,
        CMD_FOR,
        CMD_INPUT,
        CMD_BEEP,
        CMD_PRINT,
        CMD_LOCATE,
        CMD_POKE,
        CMD_PSET,
        CMD_STOP,
        CMD_END,
        CMD_MEM,
        CMD_PEN,
        CMD_PAPER,
        CMD_QMARK,
        CMD_HASH,
        CMD_QUOTE,
        CMD_DELAY,
        CMD_FILES,
        CMD_CFORMAT,
        CMD_CCHAIN,
        CMD_CSAVE,
        CMD_CLOAD,
        CMD_PINDIR,
        CMD_PINDWRITE,
        CMD_ASSIGNMENT
};
enum {
        FN_PEEK = 0,
        FN_ABS,
        FN_RND,
        FN_PINDREAD,
        FN_PINAREAD,
        FN_UNKNOWN
};
enum OPERATORS {
        RELOP_GE = 0,
        RELOP_NE,
        RELOP_GT,
        RELOP_EQ,
        RELOP_LE,
        RELOP_LT,
        RELOP_NE_BANG,
        RELOP_UNKNOWN
};

#endif
