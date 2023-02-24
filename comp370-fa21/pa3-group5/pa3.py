# Name: pa3.py
# Author(s): Christian Gideon and Nico de la Fuente
# Date: 12 November 2021
# Description: The RegEx class initializes a regular expression, then checks if a given string
#			   is in the language of the regular expression via the "simulate" method

from queue import Empty, Queue

class Node:
    def __init__(self, data):
        self.left = None
        self.right = None
        self.data = data

class InvalidExpression(Exception):
	pass

class RegEx:
	def __init__(self, regex_filename):
		""" 
		Initializes a RegEx object from the specifications
		in the file whose name is filename.
		"""
		regex_file = open(regex_filename).read().splitlines()
		self.alphabet = regex_file[0] + "e"
		self.regex = regex_file[1].replace(" ", "")
		self.regex = self.insert_concatenation() # make the concatenation explicit

		# The operator precedence dictonary
		precedences = { #  * > o > |
			"*": 1, # star
			"o": 2, # concatenation
			"|": 3, # union
			"(": 4, # parentheses
		}

		# Create a stack for the operands and the operators
		self.operands = []
		self.operators = []
		# For each symbol in the regular expression
		for symbol in self.regex:
			if symbol in self.alphabet:
				self.operands.append(Node(symbol))
			elif symbol == "(":
				self.operators.append(symbol)
			elif symbol in "*|o": # if the symbol is an operator
				# As long as there is an operator on the stack with a lower precedence than the current symbol
				while (len(self.operators) != 0 and precedences[self.operators[-1]] < precedences[symbol]):
					operator = self.operators.pop() # pop it off the stack
					self.add_tree_to_operands(operator) # and add a node with its operands to the operand stack
				
				# Now the operator that we scanned can go onto the stack
				self.operators.append(symbol)
			# If the current symbol is a closing paren and the open paren is on the operator stack
			elif symbol == ")" and "(" in self.operators:
					# Find the matching open parentheses
					current_operator = self.operators.pop()
					while current_operator != "(":
						self.add_tree_to_operands(current_operator)
						current_operator = self.operators.pop()
			else:
				raise InvalidExpression
		
		# Now clean up the rest of the operators on the stack
		while len(self.operators) != 0:
			self.add_tree_to_operands(self.operators.pop())

		# Convert the syntax tree to a DFA
		root = self.operands.pop()
		nfa = self.to_NFA(root)
		self.DFA = nfa.toDFA()

	def simulate(self, str):
		"""
		Returns True if the string str is in the language of
		the "self" regular expression.
		"""
		return self.DFA.simulate(str)

	def add_tree_to_operands(self, operator):
		"""
		Creates a tree node with the appropriate children for the specified
		operator and adds it to the operand stack
		"""
		try:
			syntax_tree_node = Node(operator)
			if operator == "*":
				assert len(self.operands) >= 1
				syntax_tree_node.left = self.operands.pop()
			else: # Ideally, either a "|" or "o" operator
				assert len(self.operands) >= 2
				syntax_tree_node.right = self.operands.pop()
				syntax_tree_node.left = self.operands.pop()
			self.operands.append(syntax_tree_node)
		except AssertionError:
			raise InvalidExpression

	def insert_concatenation(self):
		"""
		Make the implicit concatenation explicit by adding an 'o' wherever
		concatenation is implied
		"""
		concat = 'o'
		regex = ""

		# Pass over each symbol in the regular expression
		for i in range(len(self.regex) - 1):
			regex += self.regex[i]

			# If the current symbol is a letter, star or closing paren
			# and it is followed by another letter, or an open paren
			if (self.regex[i] in self.alphabet + '*' + ')'
				and self.regex[i + 1] in self.alphabet + '('):
					regex += concat # add the concatenation operator
		regex += self.regex[-1]

		return regex


	def to_NFA(self, root):
		"""
		Converts the "self" RegEx into an equivalent NFA.
		The format of the NFA will have the same format
		as described in the second programming assignment (pa2).

		This function will create the NFA from the internal representation
		of the RegEx that was created in __init__.
		"""

		# Create a representation of an NFA
		self.transitions = []
		self.start_states = []
		self.accept_states = []
		self.state_id = 1

		# Create the NFA through recursion
		self.to_NFA_recur(root)

		# Create a string representing the NFA
		NFA_string = (str(self.state_id - 1) + '\n' # num states
				   + self.alphabet[ : -1] + '\n'
				   + '\n'.join(self.transitions) + '\n\n'
				   + str(self.start_states[0]) + '\n'
				   + ' '.join(str(s) for s in self.accept_states)
				)
		
		return NFA(NFA_string)


	def to_NFA_recur(self, root):
		"""
		Recursively creates NFA transitions in an organized manner to be passed to
		the NFA class for further organization.
		"""

		# If there are no children -> leaf node
		if not root.left and not root.right:
			# Create two new states
			start_state = self.new_NFA_state()
			accept_state = self.new_NFA_state()

			# Create a transition from the start to accept state
			self.transitions.append(f"{start_state} '{root.data}' {accept_state}")

			# Add the start and accept states to the list
			self.start_states.append(start_state)
			self.accept_states.append(accept_state)
		elif not root.right: # Star
			self.to_NFA_star_construct(root.left)
		else:
			if root.data in "|": # Union
				self.to_NFA_union_construct(root.left, root.right)
			else: # Concatenation
				self.to_NFA_concat_construct(root.left, root.right)

	
	def to_NFA_star_construct(self, operand):
		"""
		Handle the NFA construct for the star operator
		"""
		# Handle the operand
		self.to_NFA_recur(operand)
		old_start_state = self.start_states.pop() # get the old start state

		# Create a new accepting start state
		new_start = self.new_NFA_state()
		self.start_states.append(new_start)
		self.accept_states.append(new_start)

		# Add epsilon transitions from each accept state to the old start state 
		for accept_state in self.accept_states:
			self.transitions.append(f"{accept_state} 'e' {old_start_state}")
		self.transitions.append(f"{new_start} 'e' {old_start_state}")
	
	def to_NFA_union_construct(self, left_operand, right_operand):
		"""
		Handle the NFA construct for the union operator
		"""
		# Handle the left operands
		self.to_NFA_recur(left_operand)

		# Copy over the current start and accept states for later
		left_start = self.start_states.pop()
		left_accept_states = self.accept_states.copy()
		self.accept_states = [] # clear

		# Handle the right operands now
		self.to_NFA_recur(right_operand)
		right_start = self.start_states.pop()

		# Add epsilon transitions from the new start state to the old start states
		new_start = self.new_NFA_state()
		self.transitions.append(f"{new_start} 'e' {left_start}")
		self.transitions.append(f"{new_start} 'e' {right_start}")

		self.start_states.append(new_start)
		self.accept_states.extend(left_accept_states)
	
	def to_NFA_concat_construct(self, left_operand, right_operand):
		"""
		Handle the NFA construct for the union operator
		"""
		# Handle the left operands
		self.to_NFA_recur(left_operand)

		# Copy over the current accept states for later
		left_accept_states = self.accept_states.copy()
		self.accept_states.clear() # clear

		# Handle the right operands now
		self.to_NFA_recur(right_operand)
		right_start = self.start_states.pop()

		# Add an epsilon transition from the left's accept states to the right's start state
		for accept_state in left_accept_states:
			self.transitions.append(f"{accept_state} 'e' {right_start}")

	def new_NFA_state(self):
		"""
		'Create' a new NFA state by incrementing and returning self.state_id
		"""
		new_state_id = self.state_id
		self.state_id += 1
		return new_state_id


class NFA:
	""" Simulates an NFA """

	def __init__(self, nfa_string):
		"""
		Initializes NFA from a string representing the NFA
		"""

		data = nfa_string.splitlines()

		# Separate the file data - same as pa1
		self.num_states = int(data[0]) # 2^n is the power set for all possible states in the NFA
		self.alphabet = data[1] + 'e'
		raw_transitions = data[2:-3]
		self.start_state = data[-2]
		self.accept_states = data[-1].split()

		# Reformat the raw transitions
		raw_transitions = [line.split() for line in raw_transitions]
		self.transitions = self.init_transition_dict(raw_transitions)


	def toDFA(self):
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
		return DFA(self.write_to_dfa_string())
		
		
	def init_transition_dict(self, transitions):
		"""
		Initializes a dictionary that represents the
		transitions of the DFA.
		"""

		t_dict = {} # transition dict
		# Initialize sub-dictionary for the current state
			
		for raw_t in transitions:
			# Parse the raw transition
			in_state = raw_t[0]
			input = raw_t[1].strip("'")
			out_state = raw_t[2]

			if in_state not in t_dict:
				t_dict[in_state] = {symbol: [] for symbol in self.alphabet}

			# Add the transition to the dictionary, but if we're at the end
			# then add an extra state that just has empty transitions
			t_dict[in_state][input].append(out_state)
			if out_state not in t_dict:
				t_dict[str(int(out_state))] = {symbol: [] for symbol in self.alphabet}
		
		return t_dict


	def write_to_dfa_string(self):
		"""
		Write the DFA definition in the correct format to a string

		@return The DFA as a string
		"""
			
		dfa_str = f"{len(self.final_dfa_transitions)}\n"
		dfa_str += f"{self.alphabet[ : -1]}\n"

		# Write out the transitions
		for state, transitions in self.final_dfa_transitions.items():
			dfa_state_in = int(state) + 1
			for input in transitions:
				dfa_state_out = int(transitions[input]) + 1
				dfa_str += f"{dfa_state_in} '{input}' {dfa_state_out}\n"

		dfa_str += f"1\n"
		dfa_str += ' '.join([str(int(a)+1) for a in self.dfa_accept_states])

		return dfa_str


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


class DFA:
	""" Simulates a DFA """

	def __init__(self, dfa_str):
		"""
		Initializes DFA from a string

		@param dfa_str: a string representing the DFA definition
		"""
		data = dfa_str.splitlines()

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

		# For each symbol in the string
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