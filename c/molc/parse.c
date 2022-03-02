#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include "parse.h"

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

static regex_t operation, addression;
static regex_t imme, regi, addr, regi_addr, regi_addr_regi;

static int immediator(char *expr)
{
    int value = 0;
    while (expr[0])
    {
        if (strncmp(expr, "몰", strlen("몰")) == 0)
        {
            value <<= 2;
            expr += strlen("몰");
        }
        else if (strncmp(expr, "아", strlen("아")) == 0)
        {
            value <<= 2;
            value++;
            expr += strlen("아");
        }
        else if (strncmp(expr, "?", 1) == 0)
        {
            value <<= 2;
            value += 2;
            expr++;
        }
        else if (strncmp(expr, "루", strlen("루")) == 0)
        {
            value <<= 2;
            value += 3;
            expr += strlen("루");
        }
    }
    return value;
}

static type_t typeParser(char *expr)
{
    type_t type;
    if (regexec(&imme, expr, 0, NULL, 0) == 0)
    {
        type.type = IMMEDIATE;
        type.value = immediator(expr);
    }
    else if (regexec(&regi, expr, 0, NULL, 0) == 0)
    {
        type.type = REGISTER;
        type.value = expr[strlen("몰")] - '0';
    }
    else if (regexec(&addr, expr, 0, NULL, 0) == 0)
    {
        type.type = ADDRESS;
        type.value = strlen(expr) - strlen("ﾌﾄｽﾄ");
    }
    else if (regexec(&regi_addr, expr, 0, NULL, 0) == 0)
    {
        type.type = REGISTERIZED_ADDRESS;
        type.value = strlen(expr) - strlen("털자");
    }
    else if (regexec(&regi_addr_regi, expr, 0, NULL, 0) == 0)
    {
        type.type = REGISTERIZED_ADDRESSERIZED_REGISTER;
        type.value = expr[strlen("털")] - '0';
    }
    else
    {
        type.type = UNKNOWN;
        type.value = 0;
    }
    return type;
}

typedef struct buffer_vector_t
{
    int *data;
    size_t length;
    size_t capacity;
} buffer_vector_t;

static buffer_vector_t *create_buffer()
{
    buffer_vector_t *b = malloc(sizeof(buffer_vector_t));
    b->data = malloc(sizeof(int));
    b->length = 0;
    b->capacity = 1;
    return b;
}

static void append_buffer(buffer_vector_t *b, int item)
{
    if (b->length + 1 > b->capacity)
    {
        b->capacity *= 2;
        b->data = realloc(b->data, sizeof(int) * b->capacity);
    }
    b->data[b->length++] = item;
}

static void delete_buffer(buffer_vector_t *b)
{
    free(b->data);
    free(b);
}

static buffer_vector_t *buffer_vector;

typedef struct address_reserve
{
    long pos;
    int id;
} address_reserve;

typedef struct addr_vector_t
{
    address_reserve *data;
    size_t length;
    size_t capacity;
} addr_vector_t;

static addr_vector_t *create_addr_vector()
{
    addr_vector_t *v = malloc(sizeof(addr_vector_t));
    v->data = malloc(sizeof(address_reserve));
    v->length = 0;
    v->capacity = 1;
    return v;
}

static void append_addr_vector(addr_vector_t *v, address_reserve item)
{
    if (v->length + 1 > v->capacity)
    {
        v->capacity *= 2;
        v->data = realloc(v->data, sizeof(address_reserve) * v->capacity);
    }
    v->data[v->length++] = item;
}

static void delete_addr_vector(addr_vector_t *v)
{
    free(v->data);
    free(v);
}

static addr_vector_t *addr_vector;

typedef struct address_value
{
    int value;
    int id;
} address_value;

typedef struct addr_val_node
{
    address_value value;
    struct addr_val_node *left, *right;
} addr_val_node;

typedef struct addr_val_tree
{
    addr_val_node *root;
} addr_val_tree;

static addr_val_tree *create_addr_tree()
{
    addr_val_tree *t = malloc(sizeof(addr_val_tree));
    t->root = NULL;
    return t;
}

static void set_addr_tree(addr_val_tree *t, address_value value)
{
    if (!t->root)
    {
        t->root = malloc(sizeof(addr_val_node));
        memset(t->root, 0, sizeof(addr_val_node));
        t->root->value = value;
    }
    else
    {
        addr_val_node *node = t->root;
        while (node->value.id >= 0)
        {
            if (value.id < node->value.id)
            {
                if (!node->left)
                {
                    node->left = malloc(sizeof(addr_val_node));
                    memset(node->left, 0, sizeof(addr_val_node));
                    node->left->value.id = -1;
                }
                node = node->left;
            }
            else if (value.id > node->value.id)
            {
                if (!node->right)
                {
                    node->right = malloc(sizeof(addr_val_node));
                    memset(node->right, 0, sizeof(addr_val_node));
                    node->right->value.id = -1;
                }
                node = node->right;
            }
            else
            {
                node->value.id = -1;
            }
        }
        node->value = value;
    }
}

static int get_addr_tree(addr_val_tree *t, int id)
{
    if (!t->root)
    {
        return -1;
    }
    addr_val_node *node = t->root;
    while (node)
    {
        if (id < node->value.id)
        {
            node = node->left;
        }
        else if (id > node->value.id)
        {
            node = node->right;
        }
        else
        {
            return node->value.value;
        }
    }
    return -1;
}

static void delete_addr_node(addr_val_node *node)
{
    if (node->left)
    {
        delete_addr_node(node->left);
    }
    if (node->right)
    {
        delete_addr_node(node->right);
    }
    free(node);
}

static void delete_addr_tree(addr_val_tree *t)
{
    if (t->root)
    {
        delete_addr_node(t->root);
    }
    free(t);
}

addr_val_tree *addr_tree;

void init_parser()
{
    regcomp(&operation, "^(몰|아|\\?|루){3}$", REG_EXTENDED);
    regcomp(&addression, "^ﾌﾄｽﾄ\\!*$", REG_EXTENDED);

    regcomp(&imme, "^(몰|아|\\?|루)+$", REG_EXTENDED);
    regcomp(&regi, "^몰[0-9]루$", REG_EXTENDED);
    regcomp(&addr, "^ﾌﾄｽﾄ\\!*$", REG_EXTENDED);
    regcomp(&regi_addr, "^털\\!*자$", REG_EXTENDED);
    regcomp(&regi_addr_regi, "^털[0-9]자$", REG_EXTENDED);

    buffer_vector = create_buffer();
    addr_vector = create_addr_vector();
    addr_tree = create_addr_tree();
}

void parse(FILE *input, FILE *output)
{
    char operator[4096];
    char arg0[4096], arg1[4096];
    type_t type0, type1;

    while (!feof(input))
    {
        fscanf(input, "%s", operator);
        if (regexec(&operation, operator, 0, NULL, 0) == 0)
        {
            switch (immediator(operator))
            {
            case 0: // NAND
                fscanf(input, "%s%s", arg0, arg1);
                type0 = typeParser(arg0);
                type1 = typeParser(arg1);
                if (type0.type != UNKNOWN && type0.type != IMMEDIATE && type0.type != ADDRESS && type1.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, type0.type * 5 + type1.type);
                    if (type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                    if (type1.type == ADDRESS || type1.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type1.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type1.value);
                }
                break;
            case 1: // SHL
                fscanf(input, "%s%s", arg0, arg1);
                type0 = typeParser(arg0);
                type1 = typeParser(arg1);
                if (type0.type != UNKNOWN && type0.type != IMMEDIATE && type0.type != ADDRESS && type1.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 25 + type0.type * 5 + type1.type);
                    if (type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                    if (type1.type == ADDRESS || type1.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type1.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type1.value);
                }
                break;
            case 2: // SHR
                fscanf(input, "%s%s", arg0, arg1);
                type0 = typeParser(arg0);
                type1 = typeParser(arg1);
                if (type0.type != UNKNOWN && type0.type != IMMEDIATE && type0.type != ADDRESS && type1.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 50 + type0.type * 5 + type1.type);
                    if (type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                    if (type1.type == ADDRESS || type1.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type1.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type1.value);
                }
                break;
            case 3: // MOV
                fscanf(input, "%s%s", arg0, arg1);
                type0 = typeParser(arg0);
                type1 = typeParser(arg1);
                if (type0.type != UNKNOWN && type0.type != IMMEDIATE && type0.type != ADDRESS && type1.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 75 + type0.type * 5 + type1.type);
                    if (type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                    if (type1.type == ADDRESS || type1.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type1.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type1.value);
                }
                break;
            case 4: // CMP
                fscanf(input, "%s%s", arg0, arg1);
                type0 = typeParser(arg0);
                type1 = typeParser(arg1);
                if (type0.type != UNKNOWN && type1.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 100 + type0.type * 5 + type1.type);
                    if (type0.type == ADDRESS || type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                    if (type1.type == ADDRESS || type1.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type1.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type1.value);
                }
                break;
            case 5: // JG
                fscanf(input, "%s", arg0);
                type0 = typeParser(arg0);
                if (type0.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 125 + type0.type);
                    if (type0.type == ADDRESS || type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                }
                break;
            case 6: // JMP
                fscanf(input, "%s", arg0);
                type0 = typeParser(arg0);
                if (type0.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 130 + type0.type);
                    if (type0.type == ADDRESS || type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                }
                break;
            case 7: // CALL
                fscanf(input, "%s", arg0);
                type0 = typeParser(arg0);
                if (type0.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 135 + type0.type);
                    if (type0.type == ADDRESS || type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                }
                break;
            case 8: // RET
                append_buffer(buffer_vector, 140);
                break;
            case 9: // INT
                fscanf(input, "%s", arg0);
                type0 = typeParser(arg0);
                if (type0.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 141 + type0.type);
                    if (type0.type == ADDRESS || type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                }
                break;
            case 10: // PUSH
                fscanf(input, "%s", arg0);
                type0 = typeParser(arg0);
                if (type0.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 146 + type0.type);
                    if (type0.type == ADDRESS || type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                }
                break;
            case 11: // NOP
                append_buffer(buffer_vector, 0);
                break;
            case 12: // POP
                fscanf(input, "%s", arg0);
                type0 = typeParser(arg0);
                if (type0.type != UNKNOWN)
                {
                    append_buffer(buffer_vector, 151 + type0.type);
                    if (type0.type == ADDRESS || type0.type == REGISTERIZED_ADDRESS)
                    {
                        address_reserve r = {buffer_vector->length, type0.value};
                        append_addr_vector(addr_vector, r);
                    }
                    append_buffer(buffer_vector, type0.value);
                }
                break;
            case 27: // HLT
                append_buffer(buffer_vector, 156);
                break;
            }
        }
        else if (regexec(&addression, operator, 0, NULL, 0) == 0)
        {
            type0 = typeParser(operator);
            address_value value = {buffer_vector->length, type0.value};
            set_addr_tree(addr_tree, value);
        }
    }

    for (size_t i = 0; i < addr_vector->length; i++)
    {
        buffer_vector->data[addr_vector->data[i].pos] = get_addr_tree(addr_tree, addr_vector->data[i].id);
    }

    fwrite(buffer_vector->data, sizeof(int), buffer_vector->length, output);
}

void quit_parser()
{
    regfree(&operation);
    regfree(&addression);
    regfree(&imme);
    regfree(&regi);
    regfree(&addr);
    regfree(&regi_addr);
    regfree(&regi_addr_regi);
    delete_buffer(buffer_vector);
    delete_addr_vector(addr_vector);
    delete_addr_tree(addr_tree);
}