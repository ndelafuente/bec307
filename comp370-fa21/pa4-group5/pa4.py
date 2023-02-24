# File: pa4.py
# Authors:
# 	Christian Gideon
# 	Nicolas de la Fuente
# Date Last Modified: December 9th, 2021
# Description: A basic lexical analyzer

from pa3 import *

class InvalidExpression(Exception):
	pass

class InvalidToken(Exception):
	""" 
	Raised if, while scanning for a token,
	the lexical analyzer cannot identify 
	a valid token, but there are still
	characters remaining in the input file
	"""
	pass

class Lex:
	def __init__(self, reg_ex_file, source_file):
		"""
		Initializes a lexical analyzer. reg_ex_file
		contains specifications of the types of tokens
		(see problem assignment for format), and source_file
		is the text file that tokens are returned from.
		"""
		
		# Parse the regular expression file
		with open(reg_ex_file) as in_file:
			lines = in_file.read().splitlines()
			token_pairs = [line.split(' "') for line in lines]

			self.regex_dict = {}
			for token, regex in token_pairs:
				regex = regex[ :-1] # Remove the quotation marks
				if token in self.regex_dict: # If token label is a duplicate, combine
					old_regex = self.regex_dict[token]
					regex = f"({old_regex})|({regex})"
				self.regex_dict[token] = regex

			# Map the token to its RegEx object
			for token, regex in self.regex_dict.items():
				# Parse out the alphabet
				alphabet = ""
				for c in regex: 
					if c not in alphabet + "|()*e ":
						alphabet += c

				self.regex_dict[token] = RegEx(alphabet, regex)

		# Parse the source file
		with open(source_file) as in_file:
			self.source = in_file.read().split()


	def next_token(self):
		"""
		Returns the next token from the source_file.
		The token is returned as a tuple with 2 items:
		the first item is the name of the token type (a string),
		and the second item is the specific value of the token (also
		a string).
		Raises EOFError exception if there are not more tokens in the
		file.
		Raises InvalidToken exception if a valid token cannot be identified,
		but there are characters remaining in the source file.
		"""

		# When no more tokens left
		if len(self.source) == 0:
			raise EOFError
		
		# Setting up variables to check longest token match
		longest_token = ""
		longest_token_id = ""
		longest_token_remaining = ""
		token = self.source.pop(0) # remove the first token from the queue

		# Iterate through all possible regular expressions
		for token_id, reg_ex in self.regex_dict.items():
			# Find the longest string in the token that's within our alphabet
			i = 0
			while i < len(token) and token[i] in reg_ex.alphabet:
				i += 1
			potential_token = token[0:i]

			# Remove characters from the end of the potential token until it accepts
			while not reg_ex.simulate(potential_token) and len(potential_token) > 0:
				potential_token = potential_token[ : -1]
				i -= 1

			# Tracking longest possible accepting string for this regular expression
			if reg_ex.simulate(potential_token) and len(potential_token) > len(longest_token):
				longest_token = potential_token
				longest_token_id = token_id
				longest_token_remaining = token[i: ] # What's left when removing a token from a string with no space

		# No regular expressions match with the token, invalid
		if len(longest_token) == 0:
			raise InvalidToken

		# Add what's left of current parsed token back into the queue
		if len(longest_token_remaining) > 0:
			self.source.insert(0, longest_token_remaining)

		return (longest_token_id, longest_token)
		

if __name__ == "__main__":
	num = 18   # can replace this with any number 1, ... 20.
			  # can also create your own test files.
	reg_ex_filename = f"regex{num}.txt" 
	source_filename = f"src{num}.txt"
	lex = Lex(reg_ex_filename, source_filename)
	
	try:
		while True:
			token = lex.next_token()
			print(token)

	except EOFError:
		pass
	except InvalidToken:
		print("Invalid token")