#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__


typedef enum
{
	ASSEMBLER_SUCCESS,
	ASSEMBLER_INVALID_EXT,
	ASSEMBLER_FILE_OPEN_ERR,
	ASSEMBLER_FILE_READ_ERR,
	ASSEMBLER_PARSE_ERR,
	ASSEMBLER_MEM_ERR,
	ASSEMBLER_MISSING_SYM

} AssemblerStatus;


typedef struct assembler Assembler;

/**
 * Initializes a new assembler object.
 * In case of a memory allocation error, NULL is returned
 **/
Assembler* assemblerInit();

/**
 * Process the given file, and output the binray, extern, and entry files.
 * In case of an error, no files are written, and and appropriate status is
 * returned.
 **/
AssemblerStatus assemblerProcess(Assembler* assembler, const char* filename);

/**
 * Resets the assembler state.
 **/
void assemblerReset(Assembler* assembler);

/**
 * Frees the given assembler object.
 * Passing NULL has no effect
 **/
void assemblerDestroy(Assembler* assembler);


#endif // __ASSEMBLER_H__
