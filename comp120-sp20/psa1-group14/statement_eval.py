# File: statement-eval.py
# Authors: Nico de la Fuente, Katrina Baha
# Date: 2/15/2020
# Description: Program that reads and interprets a
#    file containing simple expression and assignment
#    statements.

from pathlib import Path
import re  # For regular expressions

class BadStatement(Exception):
    pass

def interpret_statements(filename):
    """
    Function that reads statements from the file whose
    name is filename, and prints the result of each statement,
    formatted exactly as described in the psa1 problem
    statement.  
    interpret_statements must use the interpret_one_statement function,
    which appears next in this file.

    Algorithm:
    Step 1: Get the file name
    Step 2: Try to open the file (if unable print message)
    Step 3: Create a dictionary to store variables and values
    Step 4: Create a for loop to store the lines in a list
    Step 5: Remove all comments from the file
    Step 6: If line is not empty
        Step A: Split statement into tokens
        Step B: Call interpret_one_statement on the list of tokens
        Step C: If it is a bad line, print an appropriate message
        Step D: Print the result of the statement
    Step 7: Close file and end the program
    """
    try:
        statement_file = open(filename, 'r')
        variables = {}
        line_num = 0
        for line in statement_file:
            # remove comments
            hashtag = line.find("#")
            if hashtag >= 0:
                line = line[0:hashtag]

            # splitt the line into tokens
            tokens = line.split()

            # increment the line count
            line_num += 1

            # if the line is not empty try to evaluate the statement
            if len(tokens) > 0:
                print("Line %d:" % line_num, end = " ")
                try:
                    key = tokens[0] # the variable name
                    variables[key] = interpret_one_statement(tokens, variables)
                    print("%s = %.6f" % (key, variables[key]))
                except BadStatement:
                    print("Invalid statement")

        statement_file.close()
    except OSError:
        print("Bad filename. Program ending. ")





def interpret_one_statement(tokens, variables):
    """
    Function that interprets one statment.  tokens is a list of
    strings that are the tokens of the statement.  For example, if
    the statement is "xyz = salary + time - 150", then tokens would be
    ["xyz", "=", "salary", "+", "time", "-", "150"].
    variables is a dictionary that maps previously assigned variables to 
    their values.
    This function should return the value that is assigned.

    Algorithm:
    Step 1: Check if the statement is valid
    Step 2: Evaluate the statement
    Step 3: Return the result
    """
    
    if is_valid(tokens, variables):
        sum_of_values = 0
        # iterate over the entire statement by operator-operand pairs
        for i in range(1, len(tokens) - 1, 2):
            operand = tokens[i+1]
            operator = tokens[i]

            # convert variable name to its value
            if operand in variables:
                operand = variables[operand]
            
            # do the maths
            if operator == "-":
                sum_of_values -= float(operand)
            elif operator in ['=', '+']:
                sum_of_values += float(operand)
        
        return sum_of_values
    else:
        raise BadStatement

def is_valid(tokens, variables):
    """
    Function that checks if all of the tokens in the statement are valid. If 
    there is an invalid token, it will return false which will raise the 
    BadStatement exception in the interpret_statements function.

    Algorithm:
    Step 1: Check if the variable is a valid variable name
    Step 2: Check if the operands are either numbers or pre-defined variables
    Step 3: Check if the operators are valid
    Step 4: Check if the syntax is correct
    """

    # validate the variable name
    if not re.fullmatch(r"[a-zA-Z][a-zA-Z0-9]*", tokens[0]):
        return False

    # validate the statement length
    if len(tokens) < 3:
        return False
    
    # validate last token
    if re.match(r"(\+|-)", tokens[-1]):
        return False

    # validate assignment operator
    if tokens[1] != '=':
        return False

    for i in range(2, len(tokens) - 1, 2):

        # validate the operand
        operand = tokens[i]
        try:
            float(operand) # throws exception if operand is not number
        except ValueError:
            if operand not in variables:
                return False
        
        # validate the operator
        operator = tokens[i + 1]
        if operator not in ['+', '-']:
            return False

    return True
        
        



if __name__ == "__main__":
    file_name = "statements.txt"  # you can create another file with statements
                                  # and change the name of this variable to that
                                  # filename.
    
    interpret_statements(file_name)