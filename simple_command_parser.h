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
 * \param   cmd_func    Function pointer using the #cmd_func_t type for the
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

