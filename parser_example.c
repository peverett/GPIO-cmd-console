/**
 * \file
 *
 * \brief Main module to demonstrate the Simple Command Parser module.
 */

#include <stdio.h>
#include "simple_command_parser.h"

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
 * Main function
 *
 * Add two commands to the simple command parser (SCP). Then run the parse
 * loop.
 *
 * \return 0
 */
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
