#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <linux/limits.h>

#define HELP_STRING "Usage: %s [options] file\n"                     \
                    "Options:\n"                                     \
                    "  -h | --help     Display this informations.\n" \
                    "  -v | --version  Display compiler version informations.\n"
#define VERSION_STRING "molx 1.0.0\n" \
                       "Copyright 2022 ⓒ Rokr0k\n"

#define RED "\033[31m"
#define RESET "\033[m"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {0, 0, 0, 0},
};

size_t length;
int *buffer;
int registers[10];
int state;

typedef struct stack_t
{
    int *data;
    size_t length;
    size_t capacity;
} stack_t;

stack_t *create_stack()
{
    stack_t *s = malloc(sizeof(stack_t));
    s->data = malloc(sizeof(int));
    s->length = 0;
    s->capacity = 1;
}

void push_stack(stack_t *s, int item)
{
    if (s->length + 1 > s->capacity)
    {
        s->capacity *= 2;
        s->data = realloc(s->data, sizeof(int) * s->capacity);
    }
    s->data[s->length++] = item;
}

int pop_stack(stack_t *s)
{
    if (s->length > 0)
    {
        return s->data[--s->length];
    }
    return rand();
}

void delete_stack(stack_t *s)
{
    free(s->data);
    free(s);
}

stack_t *stack;

typedef struct file_buffer
{
    char filename[PATH_MAX];
    char *data;
    size_t length;
    size_t capacity;
    size_t cursor;
} file_buffer;

file_buffer *open_file(char *file)
{
    file_buffer *b = malloc(sizeof(file_buffer));
    strcpy(b->filename, file);
    FILE *f = fopen(file, "r");
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        b->length = ftell(f);
        rewind(f);
        b->capacity = 1 << (sizeof(size_t) * 8 - __builtin_clz(b->length));
        b->data = malloc(sizeof(char) * b->capacity);
        fread(b->data, sizeof(char), b->length, f);
        fclose(f);
    }
    else
    {
        b->data = malloc(sizeof(char));
        b->length = 0;
        b->capacity = 1;
    }
    return b;
}

void reserve_file(file_buffer *b, size_t s)
{
    if (s > b->capacity)
    {
        b->capacity = 1 << (sizeof(size_t) * 8 - __builtin_clz(s));
        b->data = realloc(b->data, b->capacity);
    }
    b->length = MAX(b->length, s);
}

void close_file(file_buffer *b)
{
    FILE *f = fopen(b->filename, "w");
    fwrite(b->data, sizeof(char), b->length, f);
    fclose(f);
    free(b->data);
    free(b);
}

file_buffer *file;

typedef enum type_enum
{
    UNKNOWN = -1,
    IMMEDIATE,
    REGISTER,
    ADDRESS,
    REGISTERIZED_ADDRESS,
    REGISTERIZED_ADDRESSERIZED_REGISTER
} type_enum;

typedef struct type_t
{
    type_enum type;
    int value;
} type_t;

int evaluate(type_t type)
{
    switch (type.type)
    {
    case IMMEDIATE:
        return type.value;
    case REGISTER:
        return registers[type.value];
    case ADDRESS:
        if (type.value < 0)
        {
            return rand() % length;
        }
        return type.value;
    case REGISTERIZED_ADDRESS:
        return buffer[type.value];
    case REGISTERIZED_ADDRESSERIZED_REGISTER:
        return buffer[registers[type.value]];
    default:
        return rand();
    }
}

void insert(type_t type, int value)
{
    switch (type.type)
    {
    case REGISTER:
        registers[type.value] = value;
        break;
    case REGISTERIZED_ADDRESS:
        buffer[type.value] = value;
        break;
    case REGISTERIZED_ADDRESSERIZED_REGISTER:
        buffer[registers[type.value]] = value;
        break;
    }
}

int main(int argc, char **argv)
{
    FILE *input = NULL;
    char input_file[PATH_MAX] = {0};
    int c;
    while ((c = getopt_long(argc, argv, "hv", long_options, NULL)) != -1)
    {
        switch (c)
        {
        case 'h':
            fprintf(stdout, HELP_STRING, argv[0]);
            exit(0);
            break;
        case 'v':
            fprintf(stdout, VERSION_STRING);
            exit(0);
            break;
        }
    }

    if (optind < argc)
    {
        realpath(argv[optind], input_file);
        if (strcasecmp(strrchr(input_file, '.'), ".molx") != 0)
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

    input = fopen(input_file, "rb");
    if (input == NULL)
    {
        fprintf(stderr, RED "%s 몰?루\n" RESET, input_file);
        exit(1);
    }

    fseek(input, 0, SEEK_END);
    length = ftell(input) / sizeof(int);
    rewind(input);
    buffer = malloc(sizeof(int) * length);
    fread(buffer, sizeof(int), length, input);
    stack = create_stack();

    for (int i = 0; i < 10; i++)
    {
        registers[i] = rand();
    }

    int cursor = 0;

    while (cursor < length)
    {
        if (buffer[cursor] < 25) // NAND
        {
            type_t arg0 = {buffer[cursor] / 5, buffer[cursor + 1]};
            type_t arg1 = {buffer[cursor] % 5, buffer[cursor + 2]};
            insert(arg0, ~(evaluate(arg0) & evaluate(arg1)));
            cursor += 3;
        }
        else if (buffer[cursor] < 50) // SHL
        {
            type_t arg0 = {(buffer[cursor] - 25) / 5, buffer[cursor + 1]};
            type_t arg1 = {(buffer[cursor] - 25) % 5, buffer[cursor + 2]};
            insert(arg0, evaluate(arg0) << evaluate(arg1));
            cursor += 3;
        }
        else if (buffer[cursor] < 75) // SHR
        {
            type_t arg0 = {(buffer[cursor] - 50) / 5, buffer[cursor + 1]};
            type_t arg1 = {(buffer[cursor] - 50) % 5, buffer[cursor + 2]};
            insert(arg0, evaluate(arg0) >> evaluate(arg1));
            cursor += 3;
        }
        else if (buffer[cursor] < 100) // MOV
        {
            type_t arg0 = {(buffer[cursor] - 75) / 5, buffer[cursor + 1]};
            type_t arg1 = {(buffer[cursor] - 75) % 5, buffer[cursor + 2]};
            insert(arg0, evaluate(arg1));
            cursor += 3;
        }
        else if (buffer[cursor] < 125) // CMP
        {
            type_t arg0 = {(buffer[cursor] - 100) / 5, buffer[cursor + 1]};
            type_t arg1 = {(buffer[cursor] - 100) % 5, buffer[cursor + 2]};
            int a = evaluate(arg0);
            int b = evaluate(arg1);
            state = a > b;
            cursor += 3;
        }
        else if (buffer[cursor] < 130) // JG
        {
            type_t arg0 = {buffer[cursor] - 125, buffer[cursor + 1]};
            if (state)
            {
                cursor = evaluate(arg0);
            }
            else
            {
                cursor += 2;
            }
        }
        else if (buffer[cursor] < 135) // JMP
        {
            type_t arg0 = {buffer[cursor] - 130, buffer[cursor + 1]};
            cursor = evaluate(arg0);
        }
        else if (buffer[cursor] < 140) // CALL
        {
            type_t arg0 = {buffer[cursor] - 135, buffer[cursor + 1]};
            push_stack(stack, cursor + 2);
            cursor = evaluate(arg0);
        }
        else if (buffer[cursor] < 141) // RET
        {
            cursor = pop_stack(stack);
        }
        else if (buffer[cursor] < 146) // INT
        {
            type_t arg0 = {buffer[cursor] - 141, buffer[cursor + 1]};
            switch (evaluate(arg0))
            {
            case 0:
                fprintf(stdout, "%c", (char)registers[0]);
                break;
            case 1:
                fscanf(stdin, " %c", (char *)&registers[0]);
                break;
            case 2:
            {
                char *symFilename = malloc(sizeof(char) * PATH_MAX);
                char *filename = malloc(sizeof(char) * PATH_MAX);
                int pointer = registers[1];
                while (buffer[pointer])
                {
                    filename[pointer - registers[1]] = buffer[pointer];
                    pointer++;
                }
                filename[pointer - registers[1]] = '\0';
                if (filename[0] != '/' || filename[0] != '~')
                {
                    strncpy(symFilename, input_file, strrchr(input_file, '/') - input_file + 1);
                    strcat(symFilename, filename);
                }
                else
                {
                    strcpy(symFilename, filename);
                }
                realpath(symFilename, filename);
                file = open_file(filename);
                free(symFilename);
                free(filename);
                break;
            }
            case 3:
                reserve_file(file, file->cursor + 1);
                file->data[file->cursor++] = registers[0];
                break;
            case 4:
                reserve_file(file, file->cursor + 1);
                registers[0] = file->data[file->cursor++];
                break;
            case 5:
                file->cursor = registers[2];
                reserve_file(file, file->cursor + 1);
                break;
            case 6:
                registers[2] = file->cursor;
                break;
            case 7:
                registers[2] = file->length;
                break;
            case 8:
                close_file(file);
                break;
            case 9:
            {
                struct timespec ts;
                ts.tv_sec = registers[0] / 1000;
                ts.tv_nsec = (registers[0] % 1000) * 1000000;
                nanosleep(&ts, &ts);
                break;
            }
            case 10:
                exit(0);
                break;
            }
            cursor += 2;
        }
        else if (buffer[cursor] < 151) // PUSH
        {
            type_t arg0 = {buffer[cursor] - 146, buffer[cursor + 1]};
            push_stack(stack, evaluate(arg0));
            cursor += 2;
        }
        else if (buffer[cursor] < 156) // POP
        {
            type_t arg0 = {buffer[cursor] - 151, buffer[cursor + 1]};
            insert(arg0, pop_stack(stack));
            cursor += 2;
        }
        else if (buffer[cursor] < 157) // HLT
        {
            int a = 1, b = 0;
            *(int *)NULL = a / b;
        }
    }
}
