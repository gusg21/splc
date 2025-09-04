#if !defined(MAIN_ARGS_H)
#define MAIN_ARGS_H

struct main_args {
    char* source_file_path;
};

struct main_args main_args_parse(int argc, char* argv[]);

#endif // MAIN_ARGS_H
