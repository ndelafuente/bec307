"""
Module: Snake

A Python implementation of greedy snake
"""
import random
import tkinter as tk
from tkinter.font import Font
from enum import Enum
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

        # Start the simulation
        self.view.window.mainloop()


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
        print("Wraparound checkbox checked")

        

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
        start_button = tk.Button(self.control_frame, text = "Start", command = self.start_handler)
        start_button.grid(row=1, column=1)
        pause_button = tk.Button(self.control_frame, text = "Pause", command = self.pause_handler)
        pause_button.grid(row=1, column=2)
        step_button = tk.Button(self.control_frame, text = "Step", command = self.step_handler)
        step_button.grid(row=1, column=3)
        step_speed_slider = tk.Scale(self.control_frame, from_=1, to=10, 
                    label = "Step Speed", showvalue=0, orient=tk.HORIZONTAL, command = self.step_speed_handler)
        step_speed_slider.grid(row=1, column=4)
        reset_button = tk.Button(self.control_frame, text = "Reset", command = self.reset_handler)
        reset_button.grid(row=1, column=5)
        quit_button = tk.Button(self.control_frame, text = "Quit", command = self.quit_handler)
        quit_button.grid(row=1, column=6)
        
        # Checkbox variable
        self.chkbox = tk.BooleanVar()
        self.chkbox.set(False)
        wraparound_checkbox = tk.Checkbutton(self.control_frame, text = "Wraparound", var = self.chkbox, command = self.wraparound_handler)
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






class SnakeModel:
    def __init__(self, num_rows, num_cols):
        """ initialize the model of the game """

if __name__ == "__main__":
   snake_game = Snake()
