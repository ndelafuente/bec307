"""
Module: Snake

A Python implementation of greedy snake
"""
import random
import tkinter as tk
from tkinter.font import Font
import unittest
import time

class Snake:
    """ This is the controller """
    def __init__(self):
        """ Initializes the snake game """
        # Define parameters
        self.NUM_ROWS = 30
        self.NUM_COLS = 30
        self.DEFAULT_STEP_TIME_MILLIS = 1000 # Time step modifier
        self.step_time_millis = self.DEFAULT_STEP_TIME_MILLIS # Actual time between steps

        # Create model
        self.model = SnakeModel(self.NUM_ROWS, self.NUM_COLS)

        # Create view
        self.view = SnakeView(self.NUM_ROWS, self.NUM_COLS)
        
        # Set up the key press event handlers
        self.view.set_up_handler(self.up_handler)
        self.view.set_down_handler(self.down_handler)
        self.view.set_right_handler(self.right_handler)
        self.view.set_left_handler(self.left_handler)

        # Set up the control handlers
        self.view.set_start_handler(self.start_handler)
        self.is_running = False
        self.view.set_pause_handler(self.pause_handler)
        self.view.set_step_handler(self.step_handler)
        self.view.set_reset_handler(self.reset_handler)
        self.view.set_quit_handler(self.quit_handler)
        self.view.set_step_speed_handler(self.step_speed_handler)
        self.view.set_wraparound_handler(self.wraparound_handler)
        
        
        # Prepare the window for the game
        self.reset_handler()

        # Start the simulation
        self.view.window.mainloop()
    
    def up_handler(self, event):
        """ Set the direction to up """
        self.model.direction = "Up"
    
    def down_handler(self, event):
        """ Set the direction to down """
        self.model.direction = "Down"
    
    def right_handler(self, event):
        """ Set the direction to right """
        self.model.direction = "Right"
    
    def left_handler(self, event):
        """ Set the direction to left """
        self.model.direction = "Left"

    def start_handler(self):
        """ Start simulation  """
        if not self.is_running:
            self.is_running = True
            self.view.schedule_next_step(self.step_time_millis, 
                                        self.continue_simulation)
        elif self.game_over:
            pass # do nothing
    
    def pause_handler(self):
        """ Pause simulation """
        if self.is_running:
            self.view.cancel_next_step()
            self.is_running = False
        
    def step_handler(self):
        """ Perform one step of simulation """
        if self.is_running:
            self.one_step()

    def reset_handler(self):
        """ Reset simulation """
        # Stop the game
        self.pause_handler()

        # Reset the model
        self.model.reset()

        # Reset the view
        self.view.reset()
        self.view.make_snake_head(self.model.snake_locations[0])
        self.view.make_food(self.model.food_location)

        # Reset the instance variables
        self.is_running = False
        self.game_over = False

    def quit_handler(self):
        """ Quit snake program """
        self.view.window.destroy()

    def step_speed_handler(self, value):
        """ Adjust simulation speed"""
        self.step_time_millis = self.DEFAULT_STEP_TIME_MILLIS // int(value)

    def wraparound_handler(self):
        """ Check to have wraparound feature """
        self.model.wraparound = self.view.wraparound.get()

    def continue_simulation(self):
        """ Perform another step of the simulation, and schedule
            the next step.
        """
        self.step_handler()
        self.view.schedule_next_step(self.step_time_millis, self.continue_simulation)
    
    def one_step(self):
        """ Simulate one step """
        # Update the model, checking if the game is over
        try:
            self.model.one_step() # Throws GameOver exception if game has ended

            # Update the view
            for cell in self.model.empty_cells:
                self.view.make_empty(cell)
            for cell in self.model.snake_locations:
                self.view.make_snake_body(cell)
            self.view.make_snake_head(self.model.snake_locations[0])
            self.view.make_food(self.model.food_location)
            self.update_score_frame()
        except GameOver:
            # Stop the game, display a game over message
            self.pause_handler()
            self.view.game_over_str.set("Game over")
            self.game_over = True

    def update_score_frame(self):
        """ Update the score frame """
        self.view.time += self.step_time_millis / 1000
        self.view.time_str.set(f"Time: {self.view.time:.2f}")
        self.view.points = self.model.points
        self.view.points_str.set(f"Points: {self.model.points}")
        self.view.pts_per_sec = self.model.points / self.view.time
        self.view.pts_per_sec_str.set(f"Points per sec: {self.view.pts_per_sec:.2f}")
        self.view.game_over_str.set("")

class SnakeView:
    def __init__(self, num_rows, num_cols):
        """ Initialize view of the game """
        # Constants
        self.CELL_SIZE = 20
        self.CONTROL_FRAME_HEIGHT = 100
        self.SCORE_FRAME_WIDTH = 200

        # Size of grid
        self.num_rows = num_rows
        self.num_cols = num_cols

        # Create window
        self.window = tk.Tk()
        self.window.title("Snake")

        # Create frame for grid of cells, and put cells in the frame
        self.grid_frame = tk.Frame(self.window, height = num_rows * self.CELL_SIZE,
                                width = num_cols * self.CELL_SIZE)
        self.grid_frame.grid(row = 1, column = 1) # use grid layout manager
        self.cells = self.add_cells()

        # Create frame for controls
        self.control_frame = tk.Frame(self.window, width = num_cols * self.CELL_SIZE, 
                                height = self.CONTROL_FRAME_HEIGHT, borderwidth = 1, relief = "solid")
        self.control_frame.grid(row = 2, column = 1, columnspan = 2, sticky = 'NESW') # use grid layout manager 
        self.control_frame.grid_propagate(False)
        (self.start_button, self.pause_button, 
         self.step_button, self.step_speed_slider, 
         self.reset_button, self.quit_button, self.wraparound_checkbox) = self.add_control() 

        # Create frame for the score
        self.score_frame = tk.Frame(self.window, height = num_rows * self.CELL_SIZE,
                                width = self.SCORE_FRAME_WIDTH)
        self.score_frame.grid(row = 1, column = 2) # use grid layout manager
        self.score_frame.grid_propagate(False)
        (self.score_label, self.points_label, self.time_label, 
         self.pts_per_sec_label, self.game_over_label) = self.add_score()
        

    def add_cells(self):
        """ Add cells to the grid frame """
        cells = []
        for r in range(self.num_rows):
            row = []
            for c in range(self.num_cols):
                frame = tk.Frame(self.grid_frame, width = self.CELL_SIZE, 
                        height = self.CELL_SIZE, borderwidth = 1, 
                        relief = "solid") 
                frame.grid(row = r, column = c) # use grid layout manager
                row.append(frame)
            cells.append(row)
        return cells
    
    def add_control(self):
        """ 
        Create control buttons and slider, and add them to the control frame 
        """
        # Start button
        start_button = tk.Button(self.control_frame, text = "Start")
        start_button.grid(row=1, column=1)

        # Pause button
        pause_button = tk.Button(self.control_frame, text = "Pause")
        pause_button.grid(row=1, column=2)

        # Step button
        step_button = tk.Button(self.control_frame, text = "Step")
        step_button.grid(row=1, column=3)

        # Step speed slider
        step_speed_slider = tk.Scale(self.control_frame, from_=1, to=10, 
                    label = "Step Speed", showvalue=0, orient=tk.HORIZONTAL)
        step_speed_slider.grid(row=1, column=4)

        # Reset button
        reset_button = tk.Button(self.control_frame, text = "Reset")
        reset_button.grid(row=1, column=5)

        # Quit button
        quit_button = tk.Button(self.control_frame, text = "Quit")
        quit_button.grid(row=1, column=6)
        
        # Wraparound checkbox
        self.wraparound = tk.BooleanVar()
        self.wraparound.set(False)
        wraparound_checkbox = tk.Checkbutton(self.control_frame, text = "Wraparound", var = self.wraparound)
        wraparound_checkbox.grid(row = 1, column = 7)

        # Vertically center the controls in the control frame
        self.control_frame.grid_rowconfigure(1, weight = 1) 

        # Horizontally center the controls in the control frame
        self.control_frame.grid_columnconfigure(0, weight = 1)
        self.control_frame.grid_columnconfigure(1, weight = 1)
        self.control_frame.grid_columnconfigure(2, weight = 1)
        self.control_frame.grid_columnconfigure(3, weight = 1)
        self.control_frame.grid_columnconfigure(4, weight = 1)
        self.control_frame.grid_columnconfigure(5, weight = 1) 
        self.control_frame.grid_columnconfigure(6, weight = 1)
        self.control_frame.grid_columnconfigure(7, weight = 1) 
                                                            
        return (start_button, pause_button, step_button, step_speed_slider, 
                reset_button, quit_button, wraparound_checkbox)

    def add_score(self):
        """
        Create score labels and add them to the score frame
        """

        # Create the variables to display the information in the labels
        self.points = 0 # Initialize the score as zero
        self.points_str = tk.StringVar()
        self.points_str.set(f"Points: {self.points}")
        self.time = 0.00 # Initialize the time as zero
        self.time_str = tk.StringVar()
        self.time_str.set(f"Time: {self.time:.2f}")
        self.pts_per_sec = 0.00 # Initialize the points per second as zero
        self.pts_per_sec_str = tk.StringVar()
        self.pts_per_sec_str.set(f"Points per sec: {self.pts_per_sec:.2f}")
        self.game_over_str = tk.StringVar()
        self.game_over_str.set("") # Initialize the game over message as hidden

        # Create the labels and place them in the grid
        score_label = tk.Label(self.score_frame, text = "Score", font = ("Times", 20))
        score_label.grid(row = 1, column = 1, pady = 15)
        points_label = tk.Label(self.score_frame, textvariable = self.points_str, 
                font = ("Helvetica", 15), borderwidth = 1, relief = "solid")
        points_label.grid(row = 2, column = 1, pady = 10)
        time_label = tk.Label(self.score_frame, textvariable = self.time_str, 
                font = ("Helvetica", 15), borderwidth = 1, relief = "solid")
        time_label.grid(row = 3, column = 1, pady = 10)
        pts_per_sec_label = tk.Label(self.score_frame, textvariable = self.pts_per_sec_str, 
                font = ("Helvetica", 15), borderwidth = 1, relief = "solid")
        pts_per_sec_label.grid(row = 4, column = 1, pady = 10)
        game_over_label = tk.Label(self.score_frame, textvariable = self.game_over_str, 
                font = ("Times", 20))
        game_over_label.grid(row = 5, column = 1, pady = 15)

        # Center the labels in the frame
        self.score_frame.grid_columnconfigure(1, weight = 1)

        return (score_label, points_label, time_label, pts_per_sec_label, game_over_label)
    
    def set_up_handler(self, handler):
        """ set handler for up key input to the function handler """
        self.window.bind('<Up>', handler)
    
    def set_down_handler(self, handler):
        """ set handler for down key input to the function handler """
        self.window.bind('<Down>', handler)
    
    def set_right_handler(self, handler):
        """ set handler for right key input to the function handler """
        self.window.bind('<Right>', handler)
    
    def set_left_handler(self, handler):
        """ set handler for left to the function handler """
        self.window.bind('<Left>', handler)
    
    def set_start_handler(self, handler):
        """ set handler for clicking on start button to the function handler """
        self.start_button.configure(command = handler)

    def set_pause_handler(self, handler):
        """ set handler for clicking on pause button to the function handler """
        self.pause_button.configure(command = handler)

    def set_step_handler(self, handler):
        """ set handler for clicking on step button to the function handler """
        self.step_button.configure(command = handler)

    def set_reset_handler(self, handler):
        """ set handler for clicking on reset button to the function handler """
        self.reset_button.configure(command = handler)

    def set_quit_handler(self, handler):
        """ set handler for clicking on quit button to the function handler """
        self.quit_button.configure(command = handler)

    def set_step_speed_handler(self, handler):
        """ set handler for dragging the step speed slider to the function handler """
        self.step_speed_slider.configure(command = handler)
    
    def set_wraparound_handler(self, handler):
        """ set handler for checking the wraparound box to the the function handler """
        self.wraparound_checkbox.configure(command = handler)

    def make_snake_body(self, cell):
        """ display a cell as part of the snake's body """
        row = cell[0]
        column = cell[1]
        self.cells[row][column]['bg'] = 'blue'

    def make_snake_head(self, cell):
        """ display a cell as the snake's head """
        row = cell[0]
        column = cell[1]
        self.cells[row][column]['bg'] = 'black'

    def make_empty(self, cell):
        """ display a cell as empty """
        row = cell[0]
        column = cell[1]
        self.cells[row][column]['bg'] = 'white'

    def make_food(self, cell):
        """ display a cell as food """
        row = cell[0]
        column = cell[1]
        self.cells[row][column]['bg'] = 'red'
 
    def reset(self):
        """ reset all cells to empty and reset stats to 0 """
        for r in range(self.num_rows):
            for c in range(self.num_cols):
                self.make_empty((r, c))

        self.points = 0
        self.points_str.set("Points: 0")
        self.time = 0.00
        self.time_str.set("Time: 0.00")
        self.pts_per_sec = 0
        self.pts_per_sec_str.set("Points per sec: 0.00")
        self.game_over_str.set("")
            

    def schedule_next_step(self, step_time_millis, step_handler):
        """ schedule next step of the simulation """
        self.start_timer_object = self.window.after(step_time_millis, step_handler)

    def cancel_next_step(self):
        """ cancel the scheduled next step of simulation """
        self.window.after_cancel(self.start_timer_object)


class SnakeModel:
    def __init__(self, num_rows, num_cols):
        """ initialize the model of the game """

        self.num_rows = num_rows
        self.num_cols = num_cols

        self.reset()
        
    def reset(self):
        """ Reset the snake to starting form """

        # Initialize all cells as empty
        self.empty_cells = []
        for r in range(self.num_rows):
            for c in range(self.num_cols):
                self.empty_cells.append((r, c))

        # Randomly choose the snake head location from the list of empty cells
        self.snake_locations = [self.random_pop(self.empty_cells)] # The snake head will always be the first in the list

        # Randomly choose the snake head location from the list of empty cells
        self.food_location = self.random_pop(self.empty_cells)

        # Initialize the variables that will be used to keep track of game state and progress
        self.points = 0
        self.direction = self.initial_direction(self.snake_locations[0])
        self.wraparound = False
    
    def initial_direction(self, cell):
        """ Calculate the starting direction of the snake """
        row = cell[0]
        column = cell[1]

        # Find the greatest distance to a wall
        max_dist = max(row, column, self.num_rows - row, self.num_cols - column)
        
        if max_dist == row:
            return "Up"
        if max_dist == column:
            return "Left"
        if max_dist == self.num_rows - row:
            return "Down"
        if max_dist == self.num_cols - column:
            return "Right"

    def is_game_over(self):
        """ Checks if the snake has collided with a wall (no wraparound) or itself """
        
        # Next location of the snake
        new_row = self.new_head[0]
        new_column = self.new_head[1]
        
        # Check if the next move will lead the snake out of bounds
        if new_row < 0 or new_column < 0 or new_row >= self.num_rows or new_column >= self.num_cols:
            if not self.wraparound:
                return True
            elif self.wraparound:
                if new_row < 0:
                    new_row = self.num_rows - 1
                elif new_column < 0:
                    new_column = self.num_cols - 1
                elif new_row >= self.num_rows:
                    new_row = 0
                elif new_column >= self.num_cols:
                    new_column = 0
                self.new_head = (new_row, new_column)

        # Check if the next move will lead the snake to hit itself
        return self.new_head in self.snake_locations
            
    def is_eating(self):
        """ Returns true if the next move puts the snakes head in the same space as a piece of food """
        return self.food_location == self.new_head

    def one_step(self):
        """ Simulates one time step of simulation """

        # Calculate the next location of the snake's head
        new_row = self.snake_locations[0][0]
        new_column = self.snake_locations[0][1]
        if self.direction == "Up":
            self.new_head = (new_row - 1, new_column)
        elif self.direction == "Down":
            self.new_head = (new_row + 1, new_column)
        elif self.direction == "Left":
            self.new_head = (new_row, new_column - 1)
        else:
            self.new_head = (new_row, new_column + 1)

        # Evaluate the next location of the head
        if self.is_game_over():
            raise GameOver
        elif self.is_eating(): # Move forward and grow 1 unit
            self.snake_locations.insert(0, self.food_location)
            self.food_location = self.random_pop(self.empty_cells)
            self.points += 1
        else: # Simply move forward
            last = self.snake_locations.pop(-1)
            self.empty_cells.append(last)
            self.snake_locations.insert(0, self.new_head)
            self.empty_cells.remove(self.new_head)
    
    def random_pop(self, list_to_search):
        """ Remove and return a random item from list_to_search """
        random_elem = random.choice(list_to_search)
        list_to_search.remove(random_elem)
        return random_elem

class GameOver(Exception):
    """ An exception representing when the game is over """
    pass

class SnakeModelTest(unittest.TestCase):
    """ A test for the game model """
    def setUp(self):
        """     Initial state
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, S, F, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        """
        self.model = SnakeModel(10, 10)
        
        self.model.empty_cells = []
        for r in range(10):
            for c in range(10):
                self.model.empty_cells.append((r, c))
                
        self.model.food_location = (5, 4)
        self.model.empty_cells.remove(self.model.food_location)

        snake_location = (5, 3)
        self.model.empty_cells.remove(snake_location)
        self.model.snake_locations = [snake_location]
        
        self.model.direction = "Right"

        """   Correct next step
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, S, S, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        """

        self.correct_snake_locations = [self.model.food_location, snake_location]

    def test_one_step(self):
        self.model.one_step()
        self.assertEqual(self.model.snake_locations, self.correct_snake_locations)

if __name__ == "__main__":
   snake_game = Snake()
