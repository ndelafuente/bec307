"""
Module: lifeIteration4

Author:
John Glick
Department of Computer Science
University of San Diego

Description:
A Python implementation of Conway's game of life, using Tkinter, and implemented
using the model-view-controller design pattern.

Iteration 4: 
Connect user actions performed on the controls (in the LifeView class)
to handler functions in the controller (the Life class).

For now, the handler functions are just stubs that print a message indicating
they have been called.  This allows validating that the connection between
a user action and a function in the controller has been made.
"""

import tkinter as tk

class Life:
    """ The controller. """
    def __init__(self):
        """ Initializes the game of life """
        # Define parameters
        self.NUM_ROWS = 30
        self.NUM_COLS = 30

        # Create view
        self.view = LifeView(self.NUM_ROWS, self.NUM_COLS)

        # Set up the control
        
        # Start
        self.view.set_start_handler(self.start_handler)
        
        # Pause
        self.view.set_pause_handler(self.pause_handler)

        # Step
        self.view.set_step_handler(self.step_handler)

        # Reset 
        self.view.set_reset_handler(self.reset_handler)

        # Quit
        self.view.set_quit_handler(self.quit_handler)

        # Step speed
        self.view.set_step_speed_handler(self.step_speed_handler)

        # Cell clicks.  (Note that a separate handler function is defined for 
        # each cell.)
        for r in range(self.NUM_ROWS):
            for c in range(self.NUM_COLS):
                def handler(event, row = r, column = c):
                    self.cell_click_handler(row, column)
                self.view.set_cell_click_handler(r, c, handler)

        # Start the simulation
        self.view.window.mainloop()

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
                
    def cell_click_handler(self, row, column):
        """ Cell click """
        print("Cell click: row = %d col = %d" % (row, column))

class LifeView:
    """ The view """

    def __init__(self, num_rows, num_cols):
        """ Initialize view of the game """
        # Constants
        self.CELL_SIZE = 20
        self.CONTROL_FRAME_HEIGHT = 100

        # Size of grid
        self.num_rows = num_rows
        self.num_cols = num_cols

        # Create window
        self.window = tk.Tk()
        self.window.title("Game of Life")

        # Create frame for grid of cells, and put cells in the frame
        self.grid_frame = tk.Frame(self.window, height = num_rows * self.CELL_SIZE,
                                width = num_cols * self.CELL_SIZE)
        self.grid_frame.grid(row = 1, column = 1)
        self.cells = self.add_cells()

        # Create frame for controls
        self.control_frame = tk.Frame(self.window, width = num_cols * self.CELL_SIZE, 
                                height = self.CONTROL_FRAME_HEIGHT)
        self.control_frame.grid(row = 2, column = 1)
        self.control_frame.grid_propagate(False)
        (self.start_button, self.pause_button, 
         self.step_button, self.step_speed_slider, 
         self.reset_button, self.quit_button) = self.add_control()            

    def add_cells(self):
        """ Add cells to the view """
        cells = []
        for r in range(self.num_rows):
            row = []
            for c in range(self.num_cols):
                frame = tk.Frame(self.grid_frame, width = self.CELL_SIZE, 
                         height = self.CELL_SIZE, borderwidth = 1, 
                         relief = "solid")
                frame.grid(row = r, column = c)
                row.append(frame)
            cells.append(row)
        return cells

    def add_control(self):
        """ 
        Create control buttons and slider, and add them to the control frame 
        """
        start_button = tk.Button(self.control_frame, text="Start")
        start_button.grid(row=1, column=1)
        pause_button = tk.Button(self.control_frame, text="Pause")
        pause_button.grid(row=1, column=2)
        step_button = tk.Button(self.control_frame, text="Step")
        step_button.grid(row=1, column=3)
        step_speed_slider = tk.Scale(self.control_frame, from_=1, to=10, 
                    label="Step Speed", showvalue=0, orient=tk.HORIZONTAL)
        step_speed_slider.grid(row=1, column=4)
        reset_button = tk.Button(self.control_frame, text="Reset")
        reset_button.grid(row=1, column=5)
        quit_button = tk.Button(self.control_frame, text="Quit")
        quit_button.grid(row=1, column=6)

        # Vertically center the controls in the control frame
        self.control_frame.grid_rowconfigure(1, weight = 1) 

        # Horizontally center the controls in the control frame
        self.control_frame.grid_columnconfigure(0, weight = 1) 
        self.control_frame.grid_columnconfigure(7, weight = 1) 
                                                            
        return (start_button, pause_button, step_button, step_speed_slider, 
                reset_button, quit_button)

    def set_cell_click_handler(self, row, column, handler):
        """ set handler for clicking on cell in row, column to the function handler """
        self.cells[row][column].bind('<Button-1>', handler)

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

if __name__ == "__main__":
    game_of_life = Life()
