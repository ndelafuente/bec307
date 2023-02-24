"""
Module: lifeIteration1

Author:
John Glick
Department of Computer Science
University of San Diego

Description:
A Python implementation of Conway's game of life, using Tkinter, and implemented
using the model-view-controller design pattern.

Iteration 1: 
Create a window that contains two frames: one for the life grid (shown in red, just
to highlight the frame), and one to display game control (shown in blue).

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

        # Start the simulation
        self.view.window.mainloop()

            
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
                                width = num_cols * self.CELL_SIZE, bg = 'red')
        self.grid_frame.grid(row = 1, column = 1) # use grid layout manager

        # Create frame for controls
        self.control_frame = tk.Frame(self.window, width = num_cols * self.CELL_SIZE, 
                                height = self.CONTROL_FRAME_HEIGHT, bg = 'blue')
        self.control_frame.grid(row = 2, column = 1) # use grid layout manager     
    
if __name__ == "__main__":
    # Play the game
    game_of_life = Life()