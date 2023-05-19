/**
*      @file strings.h
*      @author Prithvi Bhat
*      @brief Header file for string parsing and allied operational commands
**/
#ifndef STRINGS_H
#define STRINGS_H

#include "uart.h"
#include "inttypes.h"

#define MAX_STRING_LENGTH   80          // Maximum number of characters in input string
#define MAX_FIELDS          10          // Maximum number of acceptable arguments

typedef struct
{
    char input_string[MAX_STRING_LENGTH];
    char type[MAX_FIELDS];
    uint8_t position[MAX_FIELDS];
    uint8_t count;
} string_data_t;

// Function prototypes
char *itoa(char *string, int32_t number);
void string_input_get(string_data_t *user_data);
void string_parse(string_data_t *user_data);
char *getFieldString(string_data_t *user_data, uint8_t fieldNumber);
int32_t getFieldInteger(string_data_t *user_data, uint8_t fieldNumber);
bool isCommand(string_data_t *user_data, const char *command, uint8_t arg_count);

#endif
