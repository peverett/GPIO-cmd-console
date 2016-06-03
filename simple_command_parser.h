/**
 * \file
 *
 * \brief Simple command line parser.
 *
 * Provides a very simple command line parser. Provides two build in 'commands'
 * -# help - which lists all the commands supported.
 * -# end - which exits the parser loop.
 */

/**
 * The maximum command string length including terminating 0.
 */
#define MAX_CMD_STR 11

/**
 * The maximum command abbreviation string length including terminating 0.
 */
#define MAX_ABBR_STR 5

/**
 * The maximum command help string length including terminating 0.
 */
#define MAX_HELP_STR 41

/**
 * \typedef (*cmd_func_t)(int argc, char *argv[])
 *
 * \brief Function pointer type for command functions.
 *
 * To define a new command function, it must conform to this function prototype.
 *
 * The function parameters are as for a C program main() and must be processed
 * appropriately e.g. if arguments are integer, they must be converted from
 * string to int inside the command function.
 *
 * The function must return an int value. Generally, 1 indicates success and
 * 0 indicates failure.
 */
typedef int (*cmd_func_t)(int argc, char *argv[]);

/**
 * \brief Initialise the Simple Command Parser.
 *
 * This must be called before any other of the simple command parser functions.
 *
 * \param do_not_exit   Set to any value other than 0 to disable the 'end'
 *                      command and parse forever.
 */
void scp_init(int do_not_exit);


/**
 * \brief Add a new command for the parser to process.
 *
 *  Adds new commands to the parser, with help and parameter validation.
 *  Inputs are validated and will result in assert if invalid.
 *
 * \param   cmd_str     The full command string e.g. 'add' or 'sub', etc.
 *                      This cannot exceed #MAX_CMD_STR in length.
 * \param   abbr_str    An abbreviated command e.g. 'a' or 's', etc. Can be
 *                      NULL if there is no abbreviated form.
 *                      This cannot exceed #MAX_ABBR_STR in length.
 * \param   help_str    String describing function usage - displayed for 'help'
 *                      built in parse command.
 *                      This cannot exceed #MAX_HELP_STR in length.
 * \param   min_arg     Minimum number of args expected - used for validation.
 * \param   max_arg     Maximum number of args expected.
 * \param   func        Function pointer using the #cmd_func_t type for the
 *                      function to be called when the command is parsed.
 */
 void scp_add_command(
         const char*    cmd_str,
         const char*    abbr_str,
         const char*    help_str,
         int            min_arg,
         int            max_arg,
         cmd_func_t     func
         );


 /**
 * \brief Run the comamnd line parser.
 *
 * This function will loop until the 'end' command is entered, unless the
 * 'end' command has been disabled - see scp_init().
 *
 */
void scp_parse(void);

/**
 * \mainpage Simple Command Parser
 *
 * \section Purpose
 *
 * This module provides a very simple and compact command line parser, suitable
 * for embedded applications that provide a simple serial interface e.g. 
 * Arduino.
 *
 * \section Implementation
 *
 * To specify your own commands for the parser and then run the parser to
 * execute them, follow these steps:
 *
 * -# Implement your own command functions. They must use the #cmd_func_t
 *    function prototype.
 * -# Call scp_init() to initialise the parser. You can configure the parser
 *    to exit when the 'end' command is entered or not. The parser has one
 *    other command function already defined, 'help' which lists all the 
 *    commands it has been configured with.
 * -# Call scp_add_command() to add your own defined command functions.
 * -# Call scp_parse() to start the input and command parsing loop.
 *
 * \section Example
 *
 * The following code will produce a simple parser with two commands:
 * -# add - adds parameters together.
 * -# sub - subtracts the second parameter from the first parameter.
 *
 * The parser adds two commands by default:
 * -# help - displays all defined commands.
 * -# end  - exits the parser (if enabled by flag in scp_init()).
 *
 * \code {c}

#include <stdio.h>
#include "simple_command_parser.h"

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

int main(void)
{
    scp_init(0);

    scp_add_command(
            "add",
            "a",
            "Add <P1> to <P2> [... to <P5>]",
            2,
            5,
            add_cmd_func
            );

    scp_add_command(
            "sub",
            "s",
            "Subtract <P2> from <P1>",
            2,
            2,
            sub_cmd_func
            );

    printf ("Simple Command Parser\n");
    scp_parse();

    return 0;
}

 * \endcode
 *
 * \section Output
 *
 * \code{txt}
 *
>parser_example
Simple Command Parser

COMMAND      ABBR   DESCRIPTION
 help         h      Lists all commands available.
 end          end    Exit the parser.
 add          a      Add <P1> to <P2> [... to <P5>]
 sub          s      Subtract <P2> from <P1>

In [1]> add 2 2 2 2
Out[1]> 8
In [2]> sub 99 44
Out[2]> 55
In [3]> help

COMMAND      ABBR   DESCRIPTION
 help         h      Lists all commands available.
 end          end    Exit the parser.
 add          a      Add <P1> to <P2> [... to <P5>]
 sub          s      Subtract <P2> from <P1>

Out[3]> 1
In [4]> end
Out[4]> 1

 * \endcode
 */


