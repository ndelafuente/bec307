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

        # Create view
        self.view = SnakeView(self.NUM_ROWS, self.NUM_COLS)

        # Set up the event handlers
        self.view.set_up_handler(self.up_handler)
        self.view.set_down_handler(self.down_handler)
        self.view.set_right_handler(self.right_handler)
        self.view.set_left_handler(self.left_handler)
        self.view.set_start_handler(self.start_handler)
        self.view.set_pause_handler(self.pause_handler)
        self.view.set_step_handler(self.step_handler)
        self.view.set_reset_handler(self.reset_handler)
        self.view.set_quit_handler(self.quit_handler)
        self.view.set_step_speed_handler(self.step_speed_handler)
        self.view.set_wraparound_handler(self.wraparound_handler)

        # Start the simulation
        self.view.window.mainloop()
    
    def up_handler(self):
        """ Set the direction to up """
        print("Set direction to up")
    
    def down_handler(self):
        """ Set the direction to down """
        print("Set direction to down")
    
    def right_handler(self):
        """ Set the direction to right """
        print("Set direction to right")
    
    def left_handler(self):
        """ Set the direction to left """
        print("Set direction to left")

    def start_handler(self):
        """ Start simulation  """
        print("Start simulation")
    
    def pause_handler(self):
        """ Pause simulation """
        print("Pause simulation")
        
    def step_handler(self):
        """ Perform one step of simulation """
        print("One step")

    def reset_handler(self):
        """ Reset simulation """
        print("Reset simulation")

    def quit_handler(self):
        """ Quit life program """
        print("Quit program")

    def step_speed_handler(self, value):
        """ Adjust simulation speed"""
        print("Step speed: Value = %s" % (value))

    def wraparound_handler(self):
        """ Check to have wraparound feature """
        print(f"Wraparound is set to: {self.view.wraparound.get()}")

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
        start_button = tk.Button(self.control_frame, text = "Start")
        start_button.grid(row=1, column=1)
        pause_button = tk.Button(self.control_frame, text = "Pause")
        pause_button.grid(row=1, column=2)
        step_button = tk.Button(self.control_frame, text = "Step")
        step_button.grid(row=1, column=3)
        step_speed_slider = tk.Scale(self.control_frame, from_=1, to=10, 
                    label = "Step Speed", showvalue=0, orient=tk.HORIZONTAL)
        step_speed_slider.grid(row=1, column=4)
        reset_button = tk.Button(self.control_frame, text = "Reset")
        reset_button.grid(row=1, column=5)
        quit_button = tk.Button(self.control_frame, text = "Quit")
        quit_button.grid(row=1, column=6)
        
        # Checkbox variable
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
        self.game_over_str.set("GAME OVER") # Initialize the game over message as hidden

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
        self.wraparound_checkbox.configure(command = handler)

    def make_alive(self, row, column):
        """ Make cell in row, column alive """
        self.cells[row][column]['bg'] = 'black'

    def make_dead(self, row, column):
        """ Make cell in row, column dead """
        self.cells[row][column]['bg'] = 'white'

    def reset(self):
        """ reset all cells to dead """
        for r in range(self.num_rows):
            for c in range(self.num_cols):
                self.make_dead(r, c)

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

        self.empty_cells = []
        for r in range(self.num_rows):
            for c in range(self.num_cols):
                self.empty_cells.append((r, c))

        # Randomly choose the snake head location from the list of empty cells
        self.snake_locations = [self.random_pop(self.empty_cells)] # The snake head will always be the first in the list


        # Randomly choose the snake head location from the list of empty cells
        self.food_location = self.random_pop(self.empty_cells)
        

        self.points_scored = 0
        self.direction = None
        self.wraparound = False 
        """ Figure out how to update this """

    def random_pop(self, list_to_search):
        """ Remove and return a random item from list_to_search """
        random_elem = random.choice(list_to_search)
        list_to_search.remove(random_elem)
        return random_elem

    def is_game_over(self):
        """ Checks if the snake has collided with a wall (no wraparound) or itself """
        game_over = False
        if not self.wraparound:
            game_over = self.new_head[0] < 0 or self.new_head[1] < 0 or self.new_head[0] > self.num_rows or self.new_head[1] > self.num_cols
        elif self.wraparound:
            if self.new_head[0] < 0:
                self.new_head[0] = self.num_rows - 1
            elif self.new_head[1] < 0:
                self.new_head[1] = self.num_cols - 1
            elif self.new_head[0] > self.num_rows:
                self.new_head[0] = 0
            elif self.new_head[1] > self.num_cols:
                self.new_head[1] = 0
        return game_over or self.snake_locations.count(self.new_head) > 1
            

    def is_eating(self):
        return self.food_location == self.new_head

    def one_step(self):
        """ Simulates one time step of simulation """
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

        if self.is_game_over():
            return False
        elif self.is_eating():
            self.snake_locations.insert(0, self.food_location)
            self.food_location = self.random_pop(self.empty_cells)
            self.points_scored += 1
        else:
            last = self.snake_locations.pop(-1)
            self.empty_cells.append(last)
            self.snake_locations.append(self.new_head)
            self.empty_cells.remove(self.new_head)
        return True
    
class SnakeModelTest(unittest.TestCase):

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

        self.new_snake_locations = [self.model.food_location, snake_location]

    def test_one_step(self):
        self.model.one_step()
        self.assertEqual(self.model.snake_locations, self.new_snake_locations)

if __name__ == "__main__":
   snake_game = Snake()
