/*
 * csim.c
 *
 * The file simulates the behavior of a cache memory.
 * 
 * Authors: Phuong Mai and Nicolas de la Fuente
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include "cachelab.h"

// Struct to represent a cache line
typedef struct Line {
	bool valid; // the valid bit
	unsigned int line_order; // the order that the line was used
	unsigned int tag; // the tag portion of the address
} Line;

// Struct to represent a cache set
typedef struct Set {
	Line *line; // the lines of the set
} Set;

// Struct to represent a cache
typedef struct Cache {
	Set *set; // the sets of the cache
	unsigned int num_sets;
	unsigned int block_size; 
	unsigned int lines_per_set; 
} Cache;

// Struct to keep track of the result of cache accesses
typedef struct Result_Count {
	int hit_count;
	int miss_count;
	int eviction_count;
} Result_Count;

typedef unsigned long int mem_addr;

// forward declaration
void simulateCache(char *trace_file, int num_sets, int block_size, int lines_per_set, int verbose);
Cache initialize_cache(int num_sets, int block_size, int lines_per_set);
void deallocate_cache(Cache cache);
char access_cache(Cache cache, mem_addr address, int line_num);
void update_count(char result, Result_Count *res_count, int verbose);
void parse_address(mem_addr address, int num_sets, int block_size, int *tag, int *index, int *offset);
int create_right_mask(int length); 

/**
 * Prints out a reminder of how to run the program.
 *
 * @param executable_name Strign containing the name of the executable.
 */
void usage(char *executable_name) {
	printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", executable_name);
}

int main(int argc, char *argv[]) {

	int verbose_mode = 0;
	int num_sets = 0;
	int block_size = 0;
	int lines_per_set = 0;
	char *trace_filename = NULL;
	
	opterr = 0;

	int c = -1;

	// Note: adding a colon after the letter states that this option should be
	// followed by an additional value (e.g. "-s 1")
	while ((c = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
		switch (c) {
			case 'h':
				// Print the program usage
				usage(argv[0]);
				break;
			case 'v':
				// Enable verbose mode
				verbose_mode = 1;
				break;
			case 's':
				// Specify the number of index bits
				num_sets = 1 << strtol(optarg, NULL, 10);
				break;
			case 'E':
				// Specify the number of lines per set
				lines_per_set = strtol(optarg, NULL, 10);
				break;
			case 'b':
				// Specify the number of block offset bits
				block_size = 1 << strtol(optarg, NULL, 10);
				break;
			case 't':
				// specify the trace filename
				trace_filename = optarg;
				break;
			case '?':
			default:
				// Invalid option
				usage(argv[0]);
				exit(1);
		}
	}
	// Ensure that the user specified the necessary values
	if (num_sets == 0 || block_size == 0 || lines_per_set == 0 || trace_filename == NULL ) {
		usage(argv[0]);
		exit(1);
	}
	if (verbose_mode) {
		printf("Verbose mode enabled.\n");
		printf("Trace filename: %s\n", trace_filename);
		printf("Number of sets: %d\n", num_sets);
	}
	simulateCache(trace_filename, num_sets, block_size, lines_per_set, verbose_mode);

    return 0;
}

/**
 * Simulates cache with the specified organization (S, E, B) on the given
 * trace file.
 *
 * @param trace_file Name of the file with the memory addresses.
 * @param num_sets Number of sets in the simulator.
 * @param block_size Number of bytes in each cache block.
 * @param lines_per_set Number of lines in each cache set.
 * @param verbose Whether to print out extra information about what the
 *   simulator is doing (1 = yes, 0 = no).
 */
void simulateCache(char *trace_file, int num_sets, int block_size, 
						int lines_per_set, int verbose) {
	// Initialize the number of hits, misses, and evictions to zero
	Result_Count res_count = {0, 0, 0};

	// Open the trace file
	FILE* f = fopen(trace_file, "r");
	if (f == NULL) {
		printf("ERROR: Unable to open file\n");
		exit(1);
	}
	
	// Initialze the cache
	Cache cache = initialize_cache(num_sets, block_size, lines_per_set);
	
	// Read each line from the trace file
	char access_type = 0;
	mem_addr address = 0x0;
	int size = 0, line_num = 0;
	while (fscanf(f, " %c %lx,%d", &access_type, &address, &size) == 3) {
		// Ignore instruction loads (I) as they do not affect the cache
		if (access_type == 'I') { continue; }
		
		// Print information for verbose mode
		if (verbose)
			printf("%c %lx,%d", access_type, address, size);
		
		// Access the cache with the address and update the result count
		char result = access_cache(cache, address, line_num);
		update_count(result, &res_count, verbose);

		// Access the cache a second time for modify (M) lines
		if (access_type == 'M') {
			result = access_cache(cache, address, line_num);
			update_count(result, &res_count, verbose);
		}
		
		// Increment the line number
		line_num++;

		if (verbose)
			printf("\n");
	}
	if (verbose)
		printf("\n");
	
	// Close the trace file and free the cache
	fclose(f);
	deallocate_cache(cache);	

    printSummary(res_count.hit_count, res_count.miss_count, res_count.eviction_count);
}

/**
 * Initialize a cache with the appropriate specifications.
 * 
 * @param num_sets The number of sets.
 * @param block_size The block size.
 * @param lines_per_set The number of lines per set.
 * 
 * @return The cache
 */
Cache initialize_cache(int num_sets, int block_size, int lines_per_set) {
	Cache cache = {calloc(num_sets, sizeof(Set)), num_sets, block_size, lines_per_set};

	Line empty_line = {false, 0, 0x0};
	for (int i = 0; i < num_sets; i++) {
		cache.set[i].line = calloc(lines_per_set, sizeof(Line));
		for (int k = 0; k < lines_per_set; k++) {
			cache.set[i].line[k] = empty_line;
		}
	}

	return cache;
}

/**
 * Deallocate the memory associated with the cache.
 * 
 * @param cache The cache to free.
 */
void deallocate_cache(Cache cache) {
	for (int i = 0; i < cache.num_sets; i++) {
		free(cache.set[i].line);
	}
	free(cache.set);
}

/**
 * Simulate a cache access.
 * 
 * @note Invalid lines will always be at the end of the set.
 * 
 * @param cache The cache to be accessed.
 * @param address The address to access with.
 * @param line_num The order of the access, to help with LRU evictions
 * 
 * @return A character representing the result of the cache access.
 * 	'M' = miss, 'E' = eviction, 'I' = miss + eviction
 */
char access_cache(Cache cache, mem_addr address, int line_num) {
	// Unpack the address
	int tag = 0, index = 0, offset = 0;
	parse_address(address, cache.num_sets, cache.block_size, &tag, &index, &offset);
	
	// Access the appropriate set
	Set set = cache.set[index];
	int LRU = 0;
	// Check each line in the set
	for (int i = 0; i < cache.lines_per_set; i++) {
		Line line = set.line[i];

		// If the line is not valid
		if (!line.valid) {
			// Set the valid bit to true, update the line_num and tag
			cache.set[index].line[i] = (Line) {true, line_num, tag};
			return 'M'; // The result is a miss
		}
		// If the line is valid and the tags match
		else if (line.valid && line.tag == tag) {
			// Update the line order
			cache.set[index].line[i].line_order = line_num;
			return 'H'; // The result is a hit
		}

		// Find the oldest (least recently used) line in the set
		if (line.line_order < set.line[LRU].line_order)
			LRU = i;
	}

	// If there were no invalid lines and no matching tags, the cache is full and
	// the least recently used line must be evicted to make room for the new line
	cache.set[index].line[LRU] = (Line) {true, line_num, tag};
	return 'I'; // The result is a miss & eviction (I = M + E or me)
}

/**
 * Update the result count.
 * 
 * @param result The result.
 * @param res_count The count.
 * @param verbose The verbose option.
 */
void update_count(char result, Result_Count *res_count, int verbose) {
	switch (result) {
		case 'H': // In the case of a hit
			res_count -> hit_count += 1;
			if (verbose)
				printf(" hit");
			break;
		case 'M': // In the case of a miss
			res_count -> miss_count += 1;
			if (verbose)
				printf(" miss");
			break;
		case 'I': // In the case of a miss + eviction
			res_count -> miss_count += 1;
			res_count -> eviction_count += 1;
			if (verbose)
				printf(" miss eviction");
			break;
	}
}

/**
 * Parse the address to get the tag, index, and offset.
 * 
 * @param address The address.
 * @param num_sets The number of sets in the cache.
 * @param block_size The size of the blocks in the cache.
 * @param tag The address tag.
 * @param index The address set index.
 * @param offset The address block offset.
 */
void parse_address(mem_addr address, int num_sets, int block_size, int *tag, int *index, int *offset) {
	// Calculate the number of bits for the block offset and set index
	int b = log(block_size) / log(2);
	int s = log(num_sets) / log(2);

	// Calculate the tag, index, and offset
	*tag = address >> (b + s);
	*index = (address >> b) & create_right_mask(s);
	*offset = address & create_right_mask(b);
}

/**
 * Create a right justified binary mask of a certain length.
 * 
 * @param length The length of the mask.
 * 
 * @return The binary mask.
 */
int create_right_mask(int length) {
	if (length == 0) return 0;
	int mask = 1;
	for (int i = 1; i < length; i++) {
		mask <<= 1;
		mask++;
	}
	return mask;
}

