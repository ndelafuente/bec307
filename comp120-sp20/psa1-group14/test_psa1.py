# File: test_psa1.py
# Author: John Glick    
# Date: February 2, 2020
# Description: Program that tests the correctness
#              of psa1.

import sys

# import the module containing psa1 solution
import statement_eval

statements_filename = "statements.txt"
correct_output_filename = "correct_results.txt"
results_filename = "results.txt"

if __name__ == "__main__":
    # Get the correct output.
    correct_output_file = open(correct_output_filename)
    correct_results = correct_output_file.read().split('\n')

    # Print message
    print("Checking your program.")
    print("Only incorrect outputs displayed\n")

    # Run the program using the statements file
    orig_std_output = sys.stdout
    sys.stdout = open(results_filename, "w")
    results = statement_eval.interpret_statements(statements_filename)
    sys.stdout.close()
    sys.stdout = orig_std_output
    if not results:
        results_file = open(results_filename)
        results = results_file.read().split('\n')  

    # Check the results
    num_incorrect = 0
    for i in range(len(correct_results)):
        if i < len(results):
            if correct_results[i] != results[i]:
                num_incorrect += 1
                print("Your output = '%s'. Correct output = '%s'" % 
                    (results[i], correct_results[i]))
        else:
            num_incorrect += 1
            print("Your output = NOTHING. Correct output = '%s'" % 
                    (correct_results[i]))

    if num_incorrect == 0:
        print("Everything correct.  Make sure you have followed all")
        print("program requirements, and then sync to repository")
        print("and post to Piazza (folder #psa1_submit) saying you are submitting")
    else:
        print("One or more incorrect.  Fix before submitting.")