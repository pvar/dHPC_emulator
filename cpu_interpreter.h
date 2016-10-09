/**
 * @file cpu_interpreter.h
 * @brief Prototypes, macros, enumerators and global variables...
 *
 * Most of the macros in this file play a decisive role in what this system can achieve.
 * The program memory size determines the maximum size of the programs, while the stack size
 * determines the maximum number of nested function calls.
 */

#ifndef CPU_INTERPRETER_H
#define CPU_INTERPRETER_H

// ------------------------------------------------------------------------------
// INCLUDES
// ------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "thread_cpu_gpu.h"
#include "cmd_various.h"
#include "cmd_video.h"
#include "cmd_audio.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

uint16_t get_linenumber (void);
void init_basic (void);
void interpreter (void);

// ------------------------------------------------------------------------------
// MACROS
// ------------------------------------------------------------------------------

/*
 * MEMORY_SIZE = PROGRAM_SPACE + VAR_SIZE + STACK_SIZE
 * 1200 is the approximate footprint of CPU stack and variables used by the firmware
 */

#define HIGHLOW_HIGH    1
#define HIGHLOW_UNKNOWN 4

#define MEMORY_SIZE 65526       // 64 kB
#define INPUT_BUFFER_SIZE 8     // 8 Byte
#define MAX_FRAME_COUNT 10
#define STACK_SIZE (sizeof( struct stack_for_frame ) * MAX_FRAME_COUNT)
#define VAR_SIZE sizeof( int16_t )

#define STACK_GOSUB_FLAG 'G'
#define STACK_FOR_FLAG 'F'

// ------------------------------------------------------------------------------
// ENUMERATORS
// ------------------------------------------------------------------------------

enum EXECUTION_STATUS {
        POST_CMD_NOTHING = 0,
        POST_CMD_EXEC_LINE = 1,
        POST_CMD_NEXT_LINE = 2,
        POST_CMD_NEXT_STATEMENT = 3,
        POST_CMD_WARM_RESET = 4,
        POST_CMD_PROMPT = 5,
        POST_CMD_LOOP = 6
};


// ------------------------------------------------------------------------------
// DATA TYPES
// ------------------------------------------------------------------------------

typedef uint16_t LINE_NUMBER;
typedef uint8_t LINE_LENGTH;

struct stack_for_frame {
        uint8_t frame_type;
        uint8_t for_var;
        uint16_t terminal;
        uint16_t step;
        uint8_t *line_ptr;
        uint8_t *text_ptr;
};

struct stack_gosub_frame {
        uint16_t frame_type;
        uint8_t *line_ptr;
        uint8_t *text_ptr;
};

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

// general messages (definitions in printing.c)
extern const uint8_t msg_welcome[25];
extern const uint8_t msg_ram_bytes[11];
extern const uint8_t msg_rom_bytes[11];
extern const uint8_t msg_available[17];
extern const uint8_t msg_break[7];
extern const uint8_t msg_ok[3];

// error messages (definitions in printing.c)
extern const uint8_t err_msgxl[6];
extern const uint8_t err_msgxr[7];
extern const uint8_t err_msg01[20];
extern const uint8_t err_msg02[13];
extern const uint8_t err_msg03[15];
extern const uint8_t err_msg04[21];
extern const uint8_t err_msg05[20];
extern const uint8_t err_msg07[18];
extern const uint8_t err_msg08[21];
extern const uint8_t err_msg09[20];
extern const uint8_t err_msg0A[18];
extern const uint8_t err_msg0B[17];
extern const uint8_t err_msg0C[19];
extern const uint8_t err_msg0D[14];
extern const uint8_t err_msg0E[17];
extern const uint8_t err_msg0F[16];
extern const uint8_t err_msg10[20];
extern const uint8_t err_msg11[22];
extern const uint8_t err_msg12[23];
extern const uint8_t err_msg13[13];
extern const uint8_t err_msg14[24];
extern const uint8_t err_msg15[21];

// functions that return nothing / might print a value (definition in parser.c)
extern const uint8_t commands[208];

// functions that return a value / print nothing (definition in parser.c)
extern const uint8_t functions[27];

// relational operators (definition in parser.c)
extern const uint8_t relop_table[12];

// other keywords (definitions in parser.c)
extern const uint8_t to_tab[3];
extern const uint8_t step_tab[5];
extern const uint8_t highlow_tab[12];

LINE_LENGTH linelen;
LINE_NUMBER linenum;

uint8_t program_space[MEMORY_SIZE];
uint8_t input_buffer[INPUT_BUFFER_SIZE];

uint8_t *variables_ptr;
uint8_t *stack_ptr;
uint8_t *line_ptr;
uint8_t *text_ptr;
uint8_t *prog_end_ptr;
uint8_t *stack_limit;
uint8_t error_code;


#endif
