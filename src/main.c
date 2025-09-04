#include <stdio.h>

#include "main_args.h"
#include "repl.h"

int main(int argc, char* argv[]) {
    printf("splc\n");

    struct main_args args = main_args_parse(argc, argv);

    if (args.source_file_path == NULL) {
        /* Run the REPL. */
        repl_run();
    } else {
        /* Interpret a file from a given source file. */
        printf("Compiling file: %s\n", args.source_file_path);
    }

    return 0;
}