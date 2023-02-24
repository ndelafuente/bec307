"""
File: fractal_tree.py

Author: Katrina Baha and Nicolas de la Fuente

Date: 23 March 2020

Description: Program that recursively displays a fractal tree.
"""
import tkinter as tk
import math

class FractalTree:
    def __init__(self):
        '''Initialize the fractal object.'''

        # Create window, canvas, control frame, buttons
        self.window_size = 400
        self.window = tk.Tk()
        self.window.title("Fractal Tree")
        self.canvas = tk.Canvas(self.window, width = self.window_size, height = self.window_size, 
                        borderwidth = 1, relief = 'solid')
        self.canvas.grid(row = 1, column = 1)

        # Create control frame
        self.button_frame = tk.Frame(self.window, width = self.window_size, height = 50)
        self.button_frame.grid(row = 2, column = 1)
        self.button_frame.grid_propagate(False)

        # Create buttons
        self.advance_button = tk.Button(self.button_frame, text = "Advance", width = 6, command = self.advance)
        self.advance_button.grid(row = 1, column = 1)
        self.reset_button = tk.Button(self.button_frame, text = "Reset", width = 6,  command = self.reset)
        self.reset_button.grid(row = 1, column = 2)
        self.quit_button = tk.Button(self.button_frame, text = "Quit", width = 6,  command = self.quit)
        self.quit_button.grid(row = 1, column = 3)

        # Spacing out the buttons
        self.button_frame.grid_rowconfigure(1, weight = 1)
        self.button_frame.grid_columnconfigure(1, weight = 1)
        self.button_frame.grid_columnconfigure(2, weight = 1)
        self.button_frame.grid_columnconfigure(3, weight = 1)

        # Setting the branch specifications
        self.SIZE = self.window_size // 3
        self.length_modifier = 0.58
        self.angle = math.pi / 5

        # Set canvas to original state
        self.reset()

        # Start event loop
        tk.mainloop()

    def advance(self):
        '''Advance one level of recursion'''
        # Increment the levels of recursion
        self.current_levels_of_recursion += 1

        # Clear the canvas
        self.canvas.delete('all')

        # Call the recursive function, only drawing the first branch
        self.draw_fractal(self.window_size // 2, self.window_size, math.pi / 2, 
                    self.window_size / 3, self.current_levels_of_recursion)
    
    def draw_fractal(self, start_point_x, start_point_y, angle, size, levels_of_recursion):
        '''Draw two branches starting at enpoint (x, y) equidistant from angle'''
        # Draw the branch
        end_point_x = start_point_x - int(math.cos(angle) * size)
        end_point_y = start_point_y - int(math.sin(angle) * size)
        self.canvas.create_line(start_point_x, start_point_y, end_point_x, end_point_y)

        if levels_of_recursion == 0:
            return
        else:
            '''Call the function recursively'''
            # For the left branch
            self.draw_fractal(end_point_x, end_point_y, angle - self.angle, 
                        size * self.length_modifier, levels_of_recursion - 1)
            # For the right branch
            self.draw_fractal(end_point_x, end_point_y, angle + self.angle, 
                        size * self.length_modifier, levels_of_recursion - 1)

    def reset(self):
        '''Reset the canvas to 0 levels of recursion'''
        # Clear the canvas
        self.canvas.delete("all")

        # Reset the levels of recurion
        self.current_levels_of_recursion = 0

        # Call the recursive function, only drawing the first branch
        self.draw_fractal(self.window_size // 2, self.window_size, math.pi / 2, 
                    self.window_size / 3, self.current_levels_of_recursion)

    def quit(self):
        '''Quit the program'''
        self.window.destroy()

    
if __name__ == "__main__":
    # Create GUI
    FractalTree()