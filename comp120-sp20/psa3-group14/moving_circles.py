"""
File: moving_circles.py 

Author: Katrina Baha and Nicolas de la Fuente

Date: 10 March 2020

Description: Program that gets two circle locations from the
user, then draws a line between them, and 
displays the distance between them midway along
the line.  The user can drag either circle around,
and the distance is kept updated.
"""

# Imports
import tkinter as tk
from enum import Enum
import math
    
class MovingCircles:
    def __init__(self):
        '''Initialize the GUI'''
        # Create main window
        self.window = tk.Tk()
        self.window.title("Moving Circles")

        # Size specifications for window and circle
        self.width = 400
        self.height = 400
        self.circle_r = 20
        self.circle_num = None

        # Create canvas and place it in the window
        self.canvas = tk.Canvas(self.window, width = self.width, height = self.height, 
                        borderwidth = 1, relief = 'solid')
        self.canvas.grid(row = 1, column = 1)

        # Set up handlers for mouse action
        self.canvas.bind("<ButtonPress-1>", self.mouse_click_handler)
        self.canvas.bind("<B1-Motion>", self.mouse_motion_handler)

        # Create and place instructional message in the window
        self.message = tk.StringVar()
        self.message_label = tk.Label(self.window, textvariable = self.message)
        self.message_label.grid(row = 2, column = 1)

        # Create a frame for the buttons
        self.button_frame = tk.Frame(self.window)
        self.button_frame.grid(row = 3, column = 1)

        # Create and place buttons in the frame
        self.clear_button = tk.Button(self.button_frame, text = "Clear", width = 6,  command = self.clear)
        self.clear_button.grid(row = 1, column = 1)
        self.quit_button = tk.Button(self.button_frame, text = "Quit", width = 6,  command = self.quit)
        self.quit_button.grid(row = 1, column = 2)
        
        # Set the program to default state
        self.reset()

        # Start event loop
        self.window.mainloop()

    def mouse_click_handler(self, event):
        '''Handle the mouse clicks within the canvas'''
        if self.state == State.WAITING_FOR_FIRST_CLICK:
            # Put the coordinates of mouse click into the appropriate variable
            self.circle1_x = event.x
            self.circle1_y = event.y

            # Create first circle
            self.circle1 = self.create_circle(self.circle1_x, self.circle1_y)

            # Change state
            self.message.set("Click to place the second circle")
            self.state = State.WAITING_FOR_SECOND_CLICK
            
        elif self.state == State.WAITING_FOR_SECOND_CLICK:
            # Put the coordinates of mouse click into the appropriate variable
            self.circle2_x = event.x 
            self.circle2_y = event.y

            # Create second circle at mouse click
            self.circle2 = self.create_circle(self.circle2_x, self.circle2_y)

            # Draw line and place distance label
            self.draw_line()

            # Change state
            self.message.set("Click and hold inside a circle to move it")
            self.state = State.WAITING_FOR_MOVEMENT
        
        elif self.state == State.WAITING_FOR_MOVEMENT:
            # Check which circle the mouse click is
            self.circle_num = self.which_circle(event.x, event.y)

            self.mouse_motion_handler(event)

    def mouse_motion_handler(self, event):
        '''Handle the mouse movement within the canvas'''
        if event.x > 0 and event.x < self.width and event.y > 0 and event.y < self.height:
            if self.circle_num == 1:
                # Move first circle to new location
                delta_x = event.x - self.circle1_x
                delta_y = event.y - self.circle1_y
                self.canvas.move(self.circle1, delta_x, delta_y)

                # Update the coordinates for the first circle
                self.circle1_x = event.x
                self.circle1_y = event.y
            elif self.circle_num == 2:
                # Move second circle to new location
                delta_x = event.x - self.circle2_x
                delta_y = event.y - self.circle2_y
                self.canvas.move(self.circle2, delta_x, delta_y)

                # Update the coordinates for the second circle
                self.circle2_x = event.x
                self.circle2_y = event.y
            elif self.circle_num == None:
                # Ignore miscellaneous movement events
                return

            # Redraw line and distance label
            self.canvas.delete('line', 'label')
            self.draw_line()
        
    
    def which_circle(self, x, y):
        '''Check which circle the coordinates are in'''
        if (x > self.circle1_x - self.circle_r and y > self.circle1_y - self.circle_r and
                x < self.circle1_x + self.circle_r and y < self.circle1_y + self.circle_r):
            return 1 # Circle 1
        elif (x > self.circle2_x - self.circle_r and y > self.circle2_y - self.circle_r and
                x < self.circle2_x + self.circle_r and y < self.circle2_y + self.circle_r):
            return 2 # Circle 2
        else:
            return 0 # Not in circles

    def create_circle(self, x, y):
        '''Create a circle at coordinates (x, y) with the standard radius'''
        return self.canvas.create_oval(x - self.circle_r, y - self.circle_r,
                        x + self.circle_r, y + self.circle_r, fill = 'red')
    
    def draw_line(self):
        '''Draw the line connecting the circles and place the distance label at the midpoint'''
        # Draw connecting line
        self.canvas.create_line(self.circle1_x, self.circle1_y, 
                        self.circle2_x, self.circle2_y, fill = 'red', tags = 'line')
        self.canvas.tag_lower('line')

        # Calculate distance between circles
        self.distance = math.sqrt((self.circle2_x - self.circle1_x) ** 2 +
                        (self.circle2_y - self.circle1_y) ** 2)
        self.distance = round(self.distance, 2)

        # Find midpoint of line
        label_x = (self.circle1_x + self.circle2_x) // 2
        label_y = (self.circle1_y + self.circle2_y) // 2

        # Place distance label on midpoint
        self.canvas.create_text(label_x, label_y, text = str(self.distance), tags = 'label')
    
    def reset(self):
        '''Set program to default state'''
        # Reset circle attributes
        self.circle_num = None
        self.circle1_x = None
        self.circle1_y = None
        self.circle2_x = None
        self.circle2_y = None

        # Reset state and message
        self.state = State.WAITING_FOR_FIRST_CLICK
        self.message.set("Click to place the first circle")
    
    def clear(self):
        '''Clear the canvas and reset the program'''
        self.canvas.delete("all")
        self.reset()

    def quit(self):
        '''DESTROY!'''
        self.window.destroy()

class State(Enum):
    '''Keep track of the state of the window'''
    WAITING_FOR_FIRST_CLICK = 1
    WAITING_FOR_SECOND_CLICK = 2
    WAITING_FOR_MOVEMENT = 3


if __name__ == "__main__":
    # Create GUI
    MovingCircles()