#include "log.h"

#include <stdio.h>

void log_splc_error(int line, const char* message)
{
    fprintf(stderr, "ERROR (line %d): %s", line, message);
}
