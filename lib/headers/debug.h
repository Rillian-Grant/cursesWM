#ifndef FILE_DEBUG_INCLUDED
#define FILE_DEBUG_INCLUDED

#include <stdio.h>

#ifdef DEBUG
FILE *debug_output_file;

#define debug_setup() debug_output_file = fopen("debug_pipe", "a"); setlinebuf(debug_output_file)

// This macro acts just like printf
// Pinched from: https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
// Note: ## before __VA_ARGS__ omits the preceeding comma if there are no additional armuments. This ONLY WORKS IN GCC or clang with gcc compatability.
//#define debug_print(fmt, ...) do { if (DEBUG) fprintf(debug_output_file, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)
#define debug_print(fmt, ...) do { fprintf(debug_output_file, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

#define debug_shutdown() fclose(debug_output_file)
#else
#define debug_setup()
#define debug_print(fmt, ...)
#define debug_shutdown()
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

#endif
