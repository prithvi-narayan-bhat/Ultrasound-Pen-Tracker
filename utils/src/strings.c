/**
*      @file strings.c
*      @author Prithvi Bhat
*      @brief Source file for string parsing and allied operational commands
**/

#include "uart.h"
#include "strings.h"
#include "inttypes.h"
#include <stdlib.h>
#include <string.h>

#define ASCII_BACKSPACE         8
#define ASCII_DELETE            127
#define ASCII_CARRIAGE_RETURN   13
#define ASCII_UPPER_ALPHABET_A  65
#define ASCII_UPPER_ALPHABET_Z  90
#define ASCII_LOWER_ALPHABET_A  97
#define ASCII_LOWER_ALPHABET_Z  122

#define RESET(c)                (c = 0)                                                 // Macro to reassign value to 0
#define ASSERT_CLEAR(c)         ((c == ASCII_BACKSPACE || c == ASCII_DELETE) ? 1 : 0)   // Macro to validate character is ASCII backspace or ASCII delete
#define ASSERT_PRINTABLE(c)     ((c >= 32 && c <= 126) ? 1 : 0)                         // Macro to validate if character is ASCII printable
#define ASSERT_EOL(c)           ((c == ASCII_CARRIAGE_RETURN) ? 1 : 0)                  // Macro to validate if character is ASCII carriage return
#define ASSERT_BUFFER_FULL(c)   ((c >= (MAX_STRING_LENGTH - 1)) ? 1 : 0)                // Macro to validate if input string buffer is full
#define ASSERT_NUMBER(c)        ((c >= 48) && (c <= 57) ? 1 : 0)                        // Macro to validate if character is number


// Macro to validate if character is an alphabet
#define ASSERT_ALPHABET(c)      ((((c >= ASCII_UPPER_ALPHABET_A) && (c <= ASCII_UPPER_ALPHABET_Z)) || ((c >= ASCII_LOWER_ALPHABET_A) && (c <= ASCII_LOWER_ALPHABET_Z))) ? 1 : 0)
/**
*      @brief Function to get user input over UART
*      @param user_data Pointer to user_data structure
**/
void string_input_get(string_data_t *user_data)
{
    uint8_t character;
    uint16_t character_count;

    for (character_count = 0; character_count < MAX_STRING_LENGTH; character_count++)
    {
        character = getcUart0();                                                        // Read characters from user terminal
        if (ASSERT_CLEAR(character) && character_count > 0)                             // Validate
        {
            character_count--;                                                          // Decrement character count
        }

        if (ASSERT_EOL(character) || ASSERT_BUFFER_FULL(character_count))               // Validate
        {
            user_data->input_string[character_count] = '\0';                            // Indicate string has been complete and return
            return;
        }

        if (ASSERT_PRINTABLE(character))                                                // Validate
        {
            user_data->input_string[character_count] = character;                       // Append character to user input buffer
        }
    }
}

/**
*      @brief Function to parse the contents of the input string
*      @param user_data Pointer to user_data structure
**/
void string_parse(string_data_t *user_data)
{
    uint16_t character_count;
    RESET(user_data->count);                                                            // Initialise count to 0
    uint8_t delimiter_flag = 1;                                                         // To indicate encounter of delimiters (non-alphanumeric chars)

    for (character_count = 0; character_count < MAX_STRING_LENGTH; character_count++)
    {
        if (ASSERT_ALPHABET(user_data->input_string[character_count]))                  // Validate
        {
            if (delimiter_flag && user_data->count < 10)                                // Validate
            {
                delimiter_flag = 0;                                                     // Reset flag to 0
                user_data->position[user_data->count] = character_count;
                user_data->type[user_data->count] = 'a';                                // Set field type to alphabet
                user_data->count++;                                                     // Increment count
            }
        }

        else if (ASSERT_NUMBER(user_data->input_string[character_count]))               // Validate
        {
            delimiter_flag = 0;                                                         // Reset flag to 0
            user_data->position[user_data->count] = character_count;
            user_data->type[user_data->count] = 'n';                                    // Set field type to numeric
            user_data->count++;                                                         // increment count
        }

        else
        {
            delimiter_flag = 1;                                                         // Revert flag to 1
            user_data->input_string[character_count] = '\0';                            // Indicate end of string
        }
    }
}

/**
*      @brief Function to retrieve string
*      @param user_data Pointer to user data structure
*      @param fieldNumber position to retrieve from
*      @return char* pointer to the string at position
**/
char *getFieldString(string_data_t *user_data, uint8_t fieldNumber)
{
    if (user_data->count > fieldNumber)     return &user_data->input_string[user_data->position[fieldNumber]];
    else                                    return '\0';
}

/**
*      @brief Function to retrieve integer
*      @param user_data Pointer to user data structure
*      @param fieldNumber position to retrieve from
*      @return int32_t value of integer at position
**/
int32_t getFieldInteger(string_data_t *user_data, uint8_t fieldNumber)
{
    return atoi(&user_data->input_string[user_data->position[fieldNumber]]);
}

/**
*      @brief Function to determine if the  input string is a command or not
*      @param user_data Pointer to user data structure
*      @param command a valid command to compare with
*      @param arg_count number of arguments expected for the command
*      @return true if valid command
*      @return false if invalid command
**/
bool isCommand(string_data_t *user_data, const char *command, uint8_t arg_count)
{
    bool cmp = strcmp(getFieldString(user_data, 0), command);

    if (user_data->count >= arg_count && cmp == 0)   return true;
    else                                             return false;
}

/**
*      @brief Function to convert a given number to string
*      @param string Character pointer to hold converted integer
*      @param number to covert to string
*      @return char* pointer for recursive calls
**/
static char *itoa_helper(char *string, int32_t number)
{
    if (number <= -10)   string = itoa_helper(string, number / 10);

    *string++ = '0' - number % 10;
    return string;
}

/**
*      @brief Function to convert integer to string
*      @param string destination to store converted string
*      @param number to convert
*      @return char*
**/
char *itoa(char *string, int32_t number)
{
    char *s = string;
    if (number < 0)  *s++ = '-';
    else        number = -number;                                                       // Append negative sign to start of number

    *itoa_helper(s, number) = '\0';                                                     // Append NULL character to indicate end of string
    return string;
}