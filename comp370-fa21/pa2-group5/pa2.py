# Name: pa2.py
# Authors:
# 	Christian Gideon
# 	Nico de la Fuente
# Date: 22 October 2021
# Description: This program takes in an NFA to output an equivalent DFA to a file.

from queue import Empty, Queue

class NFA:
	""" Simulates an NFA """

	def __init__(self, nfa_filename):
		"""
		Initializes NFA from the file whose name is
		nfa_filename.  (So you should create an internal representation
		of the nfa.)
		"""

		data = open(nfa_filename).read().splitlines()

		# Separate the file data - same as pa1
		self.num_states = 2 ** int(data[0]) # 2^n is the power set for all possible states in the NFA
		self.alphabet = data[1] + 'e'
		raw_transitions = data[2:-3]
		self.start_state = data[-2]
		self.accept_states = data[-1].split()

		# Reformat the raw transitions
		raw_transitions = [line.split() for line in raw_transitions]
		self.transitions = self.init_transition_dict(raw_transitions)


	def toDFA(self, dfa_filename):
		"""
		Converts the "self" NFA into an equivalent DFA
		and writes it to the file whose name is dfa_filename.
		The format of the DFA file must have the same format
		as described in the first programming assignment (pa1).
		This file must be able to be opened and simulated by your
		pa1 program.

		This function should not read in the NFA file again.  It should
		create the DFA from the internal representation of the NFA that you 
		created in __init__.
		"""


		dfa_start_state = self.add_epsilons([self.start_state])

		# @note A DFA state is a set of possible NFA states and can be referred to by a unique "key"
		transitions_queue = Queue() # DFA states to be examined
		nfa_to_dfa_states = [] # DFA states that have already been examined
		self.final_dfa_transitions = {} # The transition dictionary for the DFA

		# Add the start state to the queue and DFA state list
		transitions_queue.put(dfa_start_state)
		nfa_to_dfa_states.append(dfa_start_state)

		state_num = 0 # the unique key for the final DFA state
		while not transitions_queue.empty():
			self.final_dfa_transitions[state_num] = {}

			# Get the next dfa state to be examined
			current_dfa_state = transitions_queue.get()
			
			# For each letter in the alphabet (except epsilon)
			for letter in self.alphabet[:-1]:
				# Find the next DFA state by checking all possible states of the NFA
				next_dfa_state = self.get_next_dfa_state(current_dfa_state, letter)

				# If state hasn't been explored, add to parse queue
				if next_dfa_state not in nfa_to_dfa_states: 
					nfa_to_dfa_states.append(next_dfa_state)
					transitions_queue.put(next_dfa_state)
				
				# Add the next DFA state to the transition dict
				self.final_dfa_transitions[state_num][letter] = nfa_to_dfa_states.index(next_dfa_state)

			state_num += 1

		# Find each DFA state that contains an NFA accept state
		self.dfa_accept_states = set()
		for state in nfa_to_dfa_states:
			for accept_state in self.accept_states:
				if accept_state in state:
					state_num = nfa_to_dfa_states.index(state)
					self.dfa_accept_states.add(str(state_num))

		# Write out to file
		self.write_to_dfa_file(dfa_filename)
		

		
	def init_transition_dict(self, transitions):
		"""
		Initializes a dictionary that represents the
		transitions of the DFA.
		"""

		t_dict = {} # transition dict
		# Initialize sub-dictionary for the current state
		for n in range(1, self.num_states + 1):
			t_dict[str(n)] = {symbol: [] for symbol in self.alphabet}
			
		for raw_t in transitions:
			# Parse the raw transition
			in_state = raw_t[0]
			input = raw_t[1].strip("'")
			out_state = raw_t[2]

			# Add the transition to the dictionary
			t_dict[in_state][input].append(out_state)
		
		return t_dict


	def write_to_dfa_file(self, dfa_filename):
		"""
		Write the DFA definition in the correct format to a file

		@param dfa_filename The file to write to
		"""
		with open(dfa_filename, 'w') as dfa_file:
			
			dfa_file.write(f"{len(self.final_dfa_transitions)}\n")
			dfa_file.write(f"{self.alphabet[ : -1]}\n")

			# Write out the transitions
			for state, transitions in self.final_dfa_transitions.items():
				dfa_state_in = int(state) + 1
				for input in transitions:
					dfa_state_out = int(transitions[input]) + 1
					dfa_file.write(f"{dfa_state_in} '{input}' {dfa_state_out}\n")

			dfa_file.write(f"1\n")
			dfa_file.write(' '.join([str(int(a)+1) for a in self.dfa_accept_states]))

	def get_next_dfa_state(self, current_dfa_state, letter):
		"""
		Transition to the next DFA state
		@param current_dfa_state The current DFA state
		@param letter The letter currently being read
		@return The next DFA state
		"""
		next_dfa_state = [] # a list for all next possible NFA states
		for dfa_state in current_dfa_state:
			for next_possible_state in self.transitions[dfa_state][letter]:
				if next_possible_state not in next_dfa_state:
					next_dfa_state.append(next_possible_state)
		
		self.add_epsilons(next_dfa_state)
		next_dfa_state.sort()

		return next_dfa_state


	def add_epsilons(self, current_dfa_state):
		"""
		Update the current list of states with any states reachable through 
		epsilon transitions
		@param current_dfa_state The current list of possible NFA states
		@return The updated state list
		"""
		for state in current_dfa_state: # Iterate over current states
			for e_state in self.transitions[state]["e"]:
				if e_state not in current_dfa_state: # If new state via epsilon then add to current states
					current_dfa_state.append(e_state)

		return current_dfa_state