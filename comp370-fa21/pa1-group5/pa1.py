# Name: pa1.py
# Authors:
# 	Christian Gideon
# 	Nico de la Fuente
# Date: 01 October 2021
# Description: This program allows the construction and simulation of a DFA

import sys

class DFA:
	""" Simulates a DFA """

	def __init__(self, filename):
		"""
		Initializes DFA from the file whose name is
		filename

		@param filename: the DFA to simulate
		"""
		data = open(filename).read().splitlines()

		# Separate the file data
		num_states = int(data[0])
		alphabet = data[1]
		raw_transitions = data[2:-2]
		self.start_state = data[-2]
		self.accept_states = data[-1].split()

		# Reformat the raw transitions
		raw_transitions = [line.split() for line in raw_transitions]
		self.transitions = self.init_transition_dict(raw_transitions)

	def simulate(self, string):
		""" 
		Simulates the DFA on input str.  Returns
		True if str is in the language of the DFA,
		and False if not.

		@param string: the input string to simulate on the DFA
		@return true if string is in language, false otherwise
		"""

		current_state = self.start_state

		# For each character in the string
		for c in string:
			# Update the current state with the next appropriate state
			current_state = self.transitions[current_state][c]

		# Check whether the current state is an accepting state
		return current_state in self.accept_states

	def init_transition_dict(self, transitions):
		"""
		Initializes a dictionary that represents the
		transitions of the DFA.

		@param transitions: the transitions in the DFA
		@return t_dict: a formatted dictionary of the DFA transitions
		"""
		t_dict = {}

		for t in transitions:
			# Parse the transition
			state_in = t[0]
			input = t[1].strip("'")
			state_out = t[2]

			# Initialize sub-dictionary if necessary
			if state_in not in t_dict:
				t_dict[state_in] = {}
			
			# Add the transition to the dictionary
			t_dict[state_in][input] = state_out
		
		return t_dict