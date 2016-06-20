/**
 * \file
 *
 * \brief Simple command line parser.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "simple_command_parser.h"

/*
 * Multiplatform support for putch/putchar and getch/getchar
 */
#ifdef __MINGW32__
    #define PUTCH putch
    #define GETCH getch
#endif
#ifdef __linux__
    #define PUTCH putchar
    #define GETCH getchar
#endif

#if !defined(GETCH) || !defined(PUTCH)
#error "No PUTCH/GETCH definition for this platform!"
#endif

/**
 * Maximum size of the input command string
 */
#define MAX_INPUT_BUFFER    128

/**
 * Maximum number of arguments a command can have
 */
#define MAX_ARGC            6

/**
 * \typedef command_t
 *
 * \brief Typedef of the _command_t struct.
 *
 */
typedef struct _command_t command_t;

/**
 * \struct _command_t
 *
 * \brief Structure for defining commands to read from the command line.
 *
 */
struct _command_t{
    /** Command name string. */
    const char          *cmd_str;
    /** Abbreviated command name string. */
    const char          *abbr_str;
    /** Command help description */
    const char          *help_str;
    /** Minimum number of arguments */
    int                 min_arg;
    /** Maximum number of arguments */
    int                 max_arg;
    /** Function called for the command */
    cmd_func_t          func;
    /** Next command_t node */
    command_t           *next;
};


/**
 * \struct _list_t
 *
 * List structure for storing all command nodes.
 */
struct _list_t {
    command_t           *head;
    int                 size;
};

/**
 * \typedef list_t
 *
 * Command node list type.
 */
typedef struct _list_t list_t;

/**
 * \var cmd_list
 *
 * List of all commands defined for the parser.
 *
 * As a static, this will initialise to NULL and 0.
 */
static list_t cmd_list;

/**
 * \var end_parsing
 *
 * Flag for the main parse loop. Set to 1 to exit the parse loop.
 * See the built-in 'end' command function - end_cmd_func().
 */
static int end_parsing;


/**
 * \brief Help command, which is added to the command parser by default.
 *
 * Lists all the commands that have been added to the parser.
 *
 * \param argc      ignored.
 * \param argv      ignored.
 *
 * \returns         0
 */
static int help_cmd_func(int argc, char *argv[])
{
    command_t *cmd_ptr;

    printf("\n%-11s  %-5s  %-61s\n", "COMMAND", "ABBR", "DESCRIPTION");

    for (cmd_ptr=cmd_list.head; cmd_ptr; cmd_ptr=cmd_ptr->next)
    {
        printf(" %-11s  %-5s  %-61s\n",
            cmd_ptr->cmd_str,
            cmd_ptr->abbr_str,
            cmd_ptr->help_str
            );
    }
    printf ("\n");

    return 1;
}


/**
 * \brief End command.
 *
 * Causes the parser to exit its read loop.
 *
 * \param argc      ignored.
 * \param argv      ignored.
 *
 * \returns         1
 */
static int end_cmd_func(int argc, char *argv[])
{
    end_parsing = 1;
    return 1;
}


/**
 * \brief Allocates and populates a new command node.
 *
 * Parameters are as for scp_add_command().
 *
 * \returns a new command_t node.
 */
static command_t *new_command(
         const char*    cmd_str,
         const char*    abbr_str,
         const char*    help_str,
         int            min_arg,
         int            max_arg,
         cmd_func_t     func
         )
{
    command_t *new_cmd = (command_t *)malloc(sizeof(command_t));
    assert(new_cmd);

    new_cmd->cmd_str    = cmd_str;
    new_cmd->abbr_str   = abbr_str;
    new_cmd->help_str   = help_str;
    new_cmd->min_arg    = min_arg;
    new_cmd->max_arg    = max_arg;
    new_cmd->func       = func;
    new_cmd->next       = NULL;

    return new_cmd;
}


/*
 * Initialise the command list by adding the default help command, and
 * the end command, unless the 'do_not_exit' flag is set.
 */
void scp_init(int do_not_exit)
{

    /* Make sure we are not re-initialising */
    assert(cmd_list.head == NULL);

    /* Add the help command */
    {
        command_t *help_cmd = new_command(
            "help",
            "h",
            "Lists all commands available.",
            0,
            0,
            help_cmd_func
            );

        cmd_list.head = help_cmd;
        cmd_list.size++;

        /* By default, also add the 'end' command. */
        if (!do_not_exit)
        {
            command_t *end_cmd = new_command(
                "end",
                "end",
                "Exit the parser.",
                0,
                0,
                end_cmd_func
                );

            help_cmd->next = end_cmd;
            cmd_list.size++;
        }
    }
}


/*
 * scp_add_command - adds a new command to the command list.
 */
 void scp_add_command(
         const char*    cmd_str,
         const char*    abbr_str,
         const char*    help_str,
         int            min_arg,
         int            max_arg,
         cmd_func_t     func
         )
{
    command_t *new_cmd;
    command_t *cmd_ptr = cmd_list.head;

    /* Validate scp has been initialised */
    assert(cmd_list.head);

    /* Validate inputs. */
    assert(cmd_str);
    /* abbr_str can be NULL */
    assert(help_str);
    assert(min_arg <= max_arg);
    assert(func);

    /* Validate strings are not too long. */
    assert(strlen(cmd_str) < MAX_CMD_STR);
    assert(strlen(abbr_str) < MAX_ABBR_STR);
    assert(strlen(help_str) < MAX_HELP_STR);

    /* Create a new command node. */
    new_cmd = new_command(cmd_str, abbr_str, help_str, min_arg, max_arg, func);

    /* Add it to the end of the command list */
    for (cmd_ptr=cmd_list.head; cmd_ptr->next; cmd_ptr = cmd_ptr->next);
    cmd_ptr->next = new_cmd;
    cmd_list.size++;
}


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
static int input(char *in_buffer, int len)
{
    char *ptr = in_buffer;
    char *max = in_buffer + len;

    /* Read the keyboard until return or max char are read. */
    while ( (*ptr = GETCH()) != '\r' &&  *ptr != '\n' && ptr < max)
    {
        /* A backspace deletes the previous character */
        if (*ptr == '\b' || (int)*ptr == 127)
        {
            if (ptr > in_buffer)
            {
                --ptr;
                /* Backspace, print space over the char, backspace again. */
                printf("\b \b");
            }
        }
        else
        {
            PUTCH(*ptr++);
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
 * either the cmd_str or abbr_str (if defined). Simple linear search.
 *
 * \param   name    Command name to search for.
 *
 * \return  The cmd_func_t function pointer for that command or NULL if the
 *          command wasn't found.
 */
static command_t *find_command(const char *name)
{
    command_t *cmd_ptr;

    for (cmd_ptr=cmd_list.head; cmd_ptr; cmd_ptr = cmd_ptr->next)
    {
        if (
                strcmp(name, cmd_ptr->cmd_str) == 0 ||
                (
                    cmd_ptr->abbr_str &&
                    strcmp(name, cmd_ptr->abbr_str) == 0
                )
           )
        {
            return  cmd_ptr;
        }
    }
    return NULL;
}


/**
 * scp_parse function.
 */
void scp_parse(void)
{
    char strbuff[MAX_INPUT_BUFFER];
    char *argv[MAX_ARGC];
    int argc = 0;
    int length;
    int count = 1;
    char *token;
    command_t *command;

    /* Call the built-in 'help' command to display the commands already
     * added to the parser.
     */
    help_cmd_func(0, NULL);

    /* While the 'end_parsing' flag is not set, keep parsing commands.
     * This flag can be set be the 'end' command - if it is enabled.
     */
    while (end_parsing == 0)
    {
        printf("In [%d]> ", count);
        length = input(strbuff, MAX_INPUT_BUFFER);
        printf("\n");

        /* If the input is empty (string size 0) then continue. */
        if (length == 0)
            continue;

        token = strtok(strbuff, " .,");

        if ( (command = find_command(token)) != NULL)
        {
            argc = 0;
            while (
                    (token = strtok( NULL, " .,")) != NULL &&
                    argc < MAX_ARGC
                  )
            {
                argv[argc++] = token;
            }

            if (argc < command->min_arg)
            {
                printf("Out[%d]> ERROR: [%s] too few args (less than %d)!",
                        count,
                        command->cmd_str,
                        command->min_arg
                      );
            }
            else if (argc > command->max_arg)
            {
                printf("Out[%d]> ERROR: [%s] too many args (more than %d)!",
                        count,
                        command->cmd_str,
                        command->max_arg
                      );
            }
            else {
                printf("Out[%d]> %d", count, (*command->func)(argc, argv));
            }
        }
        else
        {
            printf("Out[%d]> Unknown Command: %s", count, token);
        }
        printf ("\n");
        ++count;
    }
}

