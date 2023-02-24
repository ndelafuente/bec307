/*
 * Header file for circular queue of HistoryListItems.
 */
#ifndef __HISTQUEUE_H__
#define __HISTQUEUE_H__

#include "parse_args.h"

#define MAXHIST 10   // max number of commands in history list

/**
 * A struct to keep information one command in the history of 
 * command executed
 */
struct HistoryEntry {              
    unsigned int cmd_num;
    char cmdline[MAXLINE]; // command line for this process
};

// You can use "HistoryEntry" instead of "struct HistoryEntry"
typedef struct HistoryEntry HistoryEntry;

/**
 * Add an entry to the history
 *
 * @param cmdline The command line text to be added
 */
void add_history_entry(char *cmdline);

/**
 * Get a history entry with a certain cmdnum
 *
 * @param entry_num The cmdnum to search for
 *
 * @return The cmdline value for that entry, or NULL if it was not found
 */
char *get_history_entry(unsigned int cmd_num);


/**
 * Print the current history
 */
void print_history();

#endif
