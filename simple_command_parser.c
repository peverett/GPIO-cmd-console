/**
 * \file
 *
 * \brief Simple command line parser.
 *
 * This program reads input from the keyboard. When the return key is read,
 * the string input is parsed for simple commands.
 *
 * \mainpage Simple Command Line Parser.
 * 
 * \par Parses simple commands, as defined in a template structure.
 *
 * -# Commands can be full e.g. 'READ_PIN' or abbreviated 'RP'.
 * -# Parsing is case insensative.
 * -# Commands can have parameters which are either Char strings or decimal
 *    numbers.
 */

#include <stdio.h>
#include <string.h>


/** 
 * \typedef (*cmd_func_t)(int argc, char *argv[])
 *
 * \brief Function pointer for command functions. 
 *
 * The first param is 'argc' which is always a count of the number of
 * arguments. All parameters MUST be integer
 *
 */
typedef int (*cmd_func_t)(int argc, char *argv[]);

/**
 * \struct _command_t
 *
 * \brief Structure for defining commands read from the command line.
 *
 */
struct _command_t{
    const char command_str[11];     /**< Command name */
    const char abbrev_str[4];       /**< Appreviated command name */
    const char help_str[41];        /**< Command help description */
    int min_arg;                    /**< Minimum number of arguments */
    int max_arg;                    /**< Maximum number of arguments */
    cmd_func_t func;                /**< Function called for the command */
};

/**
 * \typedef command_t
 *
 * \brief Typedef of the #_command_t struct.
 *
 */
typedef struct _command_t command_t;

/**
 * \brief Addition Function
 *
 * Treats all arguments as integers. Sums all arguments together.
 *
 * \param   argc    Count of argv parameters.
 * \param   argv    Char* array of argument strings.   
 *
 * \return  The result of the addition sum operation.
 */
static int add_cmd_func(int argc, char *argv[]) 
{
    int idx;
    int result = 0;

    for (idx=0; idx < argc; idx++)
    {
        result += atoi( argv[idx] );
    }

    return result;
}

/**
 * \var add_cmd
 * 
 * \brief static command description for the Addition function.
 */
static command_t add_cmd = {
    "add", 
    "a", 
    "Add up to five integers.",
    2, 
    5, 
    &add_cmd_func
};

/**
 * \brief Subtract Function
 *
 * Treats all arguments as integers. Subtracts all subsquent integar arguments
 * from the first integer argument.
 *
 * \param   argc    Count of argv parameters.
 * \param   argv    Char* array of argument strings.   
 *
 * \return  The result of the subtraction operation.
 */
static int sub_cmd_func(int argc, char *argv[]) 
{
    int idx;
    int result = 0;

    if (argc >= 1) 
    {
        result = atoi(argv[0]);
        for (idx=1; idx < argc; idx++)
        {
            result -= atoi(argv[idx]);
        }
    }
    return result;
}

/**
 * \var sub_cmd
 * 
 * \brief static command description for the subtraction function.
 */
static command_t sub_cmd = { 
    "sub",
    "s",
    "Subtract p2 from p1.",
    2, 
    2, 
    &sub_cmd_func };


/**
 * \var commands
 *
 * \brief A list of all the commands that the parser should handle.
 */
static command_t *commands[] = {
    &add_cmd,
    &sub_cmd
};

/**
 * \var command_num
 * 
 * \brief the number of parsed commands.
 *
 * Calculated from the size of the commands list.
 */
const int command_num = (int)(sizeof(commands)/sizeof(command_t *));

/**
 * \brief Reads keyboard input until [return] is pressed. 
 *
 * Reads the keyboard input. Outputs the key pressed and also
 * handles [backspace] for simple editing BUT NOT ANY OTHERS.
 *
 * \param   in_buffer   Pointer to a char array to store the input in.
 * \param   len         Maximum length of the buffer, if reached, function
 *                      returns immediately.
 *
 * \return  The number of char actually read.
 */ 
int input(char *in_buffer, int len) 
{
    char *ptr = in_buffer;
    char *max = in_buffer + len;

    /* Read the keyboard until return or len char are read. */
    while ( (*ptr = getch()) != '\r' && ptr < max)
    {
        /* A backspace deletes the previous character */
        if (*ptr == '\b' || (int)*ptr == 127)
        { 
            if (ptr > in_buffer)
            {
                --ptr;
                printf("\b \b");
            }
        }
        else
        {
            putch(*ptr++);
        }
    }
    /* Replace the last read character with a 0 to terminate the string. */
    *ptr = '\0';

    return (int)(ptr - in_buffer);
}


/**
 * /brief Search for matching command function.
 *
 * Search the list of 'command_t' structures for the one that matches
 * either the command_str or abbrev_str.
 *
 * \param   name    Command name to search for.
 * 
 * \return  The cmd_func_t function pointer for that command or NULL if the
 *          command wasn't found.
 */
static command_t *find_command(const char *name)
{
    int idx;
    
    for (idx=0; idx < command_num; idx++)
    {
        if (
                strcmp(name, commands[idx]->command_str) == 0 ||
                strcmp(name, commands[idx]->abbrev_str) == 0
           )
        {
            return  commands[idx];
        }
    }
    return NULL;
}

/**
 * Main function
 *
 *  Prompt for command line input and then process the command to see if it
 *  is one of the defined 'commands'. Currently supports commands
 *      -# add - adds together 2 to 5 integers
 *      -# sub - subtract second integer from first.
 *      -# end - exit this program.
 *
 * \return 0
 */
int main(void)
{
    char strbuff[256];
    char *argv[5];
    int argc = 0;
    int length;
    int idx;
    int count = 1;
    char *token;
    command_t *command;

    printf ("Simple Command Parser\n\n");

    printf("  %-11s %-5s args(max)  Description\n", "Command", "Abbr");
    for (idx=0; idx < command_num; idx++) 
    {
        printf("  %-11s %-4s  %d(%d)        %s \n",
            commands[idx]->command_str,
            commands[idx]->abbrev_str,
            commands[idx]->min_arg,
            commands[idx]->max_arg,
            commands[idx]->help_str
            );
    }
    printf ("  end\n\n");

    while (1)
    {
        printf("Command[%3d]> ", count);
        length = input(strbuff, 256);
        printf("\n");

        token = strtok(strbuff, " .,");

        /* Is this a command we recognise? */
        if (strcmp( token, "end") == 0)
            break;

        else if ( (command = find_command(token)) != NULL)
        {
            argc = 0;
            while ( (token = strtok( NULL, " .,")) != NULL)
            {
                argv[argc++] = token;
            }

            if (argc < command->min_arg)
            {
                printf("Output [%3d]> ERROR: too few args (less than %d)!",
                        count,
                        command->min_arg
                      );
            } 
            else if (argc > command->max_arg)
            {
                printf("Output [%3d]> ERROR: too many args (more than %d)!",
                        count,
                        command->min_arg
                      );
            }
            else {
                printf("Output [%3d]> %d", count, (*command->func)(argc, argv));
            }
        }
        else 
        {
            printf("Output [%3d]> Unknown Command: %s", count, token);
        }
        printf ("\n");
        ++count;

    }

    return 0;
}

