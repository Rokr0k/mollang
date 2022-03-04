#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "parse.h"

#define HELP_STRING "Usage: %s [options] file\n"                            \
                    "Options:\n"                                            \
                    "  -h         Display this informations.\n"             \
                    "  -v         Display compiler version informations.\n" \
                    "  -o <file>  Place the output info <file>.\n"
#define VERSION_STRING "molc 1.0.0\n" \
                       "Copyright 2022 ⓒ Rokr0k\n"

#define RED "\033[31m"
#define RESET "\033[m"

int main(int argc, char **argv)
{
    FILE *input = NULL;
    FILE *output = NULL;
    char input_file[4096] = {0};
    char output_file[4096] = {0};
    int c;

    while ((c = getopt(argc, argv, "hvo:")) != -1)
    {
        switch (c)
        {
        case 'h':
            fprintf(stdout, HELP_STRING, argv[0]);
            exit(0);
        case 'v':
            fprintf(stdout, VERSION_STRING);
            exit(0);
        case 'o':
            realpath(optarg, output_file);
            break;
        }
    }

    if (optind < argc)
    {
        realpath(argv[optind], input_file);
        if (strcasecmp(strrchr(input_file, '.'), ".mol") != 0)
        {
            fprintf(stderr, RED "확장자 몰?루\n" RESET);
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, RED "파일 몰?루\n" RESET);
        exit(1);
    }

    if (output_file[0] == 0)
    {
        strcpy(output_file, strrchr(input_file, '/') + 1);
        strcpy(strrchr(output_file, '.'), ".molx");
    }

    input = fopen(input_file, "r");
    output = fopen(output_file, "wb");
    if (input == NULL || output == NULL)
    {
        if (input)
        {
            fclose(input);
        }
        else
        {
            fprintf(stderr, RED "%s 몰?루\n" RESET, input_file);
        }
        if (output)
        {
            fclose(output);
        }
        else
        {
            fprintf(stderr, RED "%s 몰?루\n" RESET, output_file);
        }
        exit(1);
    }

    init_parser();
    parse(input, output);
    quit_parser();

    fclose(input);
    fclose(output);
    exit(0);
}