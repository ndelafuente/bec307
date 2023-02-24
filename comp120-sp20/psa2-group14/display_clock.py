# File: addressbook.py
# Author: 
# Date:
# Description: 

import math 
import datetime
import tkinter as tk

class Display_Clock:
    def __init__(self):
        self.window = tk.Tk() # Create a window
        self.window.title("Current Time") # Set a title

        self.width = 200
        self.height = 200
        self.clock_r = self.width * 2 // 5
        self.second_hand_length = 0.80 * self.clock_r
        self.minute_hand_length = 0.65 * self.clock_r
        self.hour_hand_length = 0.50 * self.clock_r
        self.middle_x = self.width // 2
        self.middle_y = self.height // 2

        # create clock frame
        self.clock_display = tk.Canvas(self.window, width = self.width, height = self.height)
        self.clock_display.grid(row = 1, column = 1)

        # create clock face
        self.clock_display.create_oval((self.width // 2 - self.clock_r), (self.height // 2 - self.clock_r),
                            (self.width // 2 + self.clock_r), (self.height // 2 + self.clock_r))
        
        # create numbers
        self.clock_display.create_text(self.width // 2, (self.height // 2 - self.clock_r + 10), text = "12")
        self.clock_display.create_text((self.width // 2 + self.clock_r - 10), self.height // 2, text = "3")
        self.clock_display.create_text(self.width // 2, (self.height // 2 + self.clock_r - 10), text = "6")
        self.clock_display.create_text((self.width // 2 - self.clock_r + 10), self.height // 2, text = "9")
        
        # create digital display frame
        self.digital_frame = tk.Frame(self.window)
        self.digital_frame.grid(row = 2, column = 1)

        self.digital_clock = tk.Label(self.digital_frame, text = "")
        self.digital_clock.grid(row = 1, column = 1)

        # create button frame and buttons
        button_frame = tk.Frame(self.window)
        button_frame.grid(row = 3, column = 1)

        update_button = tk.Button(button_frame, text = "Update", command = self.update)
        update_button.grid(row = 1, column = 1)

        quit_button = tk.Button(button_frame, text = "Quit", command = self.quit)
        quit_button.grid(row = 1, column = 2)

        self.update()

        # start event loop
        self.window.mainloop()

    def update(self):
        self.current_time = datetime.datetime.now()
        self.hour = self.current_time.hour % 12
        self.minute = self.current_time.minute
        self.second = self.current_time.second

        self.digital_clock['text'] = "%d : %d : %d" % (self.hour, self.minute, self.second)

        self.hour_angle = self.hour * (math.pi / 6)
        self.minute_angle = self.minute * (math.pi / 30)
        self.second_angle = self.second * (math.pi / 30)

        self.hour_x = self.middle_x + math.sin(self.hour_angle) * self.hour_hand_length
        self.hour_y = self.middle_y - math.cos(self.hour_angle) * self.hour_hand_length
        self.minute_x = self.middle_x + math.sin(self.minute_angle) * self.minute_hand_length
        self.minute_y = self.middle_y - math.cos(self.minute_angle) * self.minute_hand_length
        self.second_x = self.middle_x + math.sin(self.second_angle) * self.second_hand_length
        self.second_y = self.middle_y - math.cos(self.second_angle) * self.second_hand_length
        
        self.clock_display.delete('hands')
        
        self.clock_display.create_line(self.middle_x, self.middle_y, 
                            self.hour_x, self.hour_y, fill = 'green', tag = 'hands')
        self.clock_display.create_line(self.middle_x, self.middle_y,
                            self.minute_x, self.minute_y, fill = 'blue', tag = 'hands')
        self.clock_display.create_line(self.middle_x, self.middle_y,
                            self.second_x, self.second_y, fill = 'red', tag = 'hands')

    
    def quit(self):
        self.window.destroy()
    
if __name__ == "__main__":
    Display_Clock()
