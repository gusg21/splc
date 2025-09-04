#include "main_args.h"

#include <stddef.h>
#include <getopt.h>

struct main_args main_args_parse(int argc, char* argv[]) {
    struct main_args args = (struct main_args) {
        .source_file_path = NULL
    };

    char c;
    while ((c = getopt (argc, argv, "s:")) != -1) {
        switch (c) {
            case 's':
                args.source_file_path = optarg;
                break;
        }
    }

    return args;
}