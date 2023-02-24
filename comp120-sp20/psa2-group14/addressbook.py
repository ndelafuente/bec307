# File: addressbook.py
# Author: Nico de la Fuente and Katrina Baha
# Date: 02/18/20
# Description: This program creates a class AddressBook which runs a GUI where the
#       user can create, view, and edit a list of addresses, load a list from a file,
#       or save the one they created. 

import tkinter as tk

class AddressBook:

    def __init__(self):
        """ Constructor for AddressBook class """
        # Create main window
        self.window = tk.Tk()
        self.window.title("AddressBook")


        # Add entry frame
        self.entry_frame = tk.Frame(self.window)
        self.entry_frame.grid(row = 1)

        # Add name label
        self.name_label = tk.Label(self.entry_frame, bg = 'white', text = "Name")
        self.name_label.grid(row = 1, column = 1, sticky = "E")
        # Add name field
        self.name = tk.Entry(self.entry_frame, bg = 'white', width = 40)
        self.name.grid(row = 1, column = 2, columnspan = 6, sticky = 'W')

        # Add street label
        self.street_label = tk.Label(self.entry_frame, bg = 'white', text = "Street")
        self.street_label.grid(row = 2, column = 1, sticky = "E")
        # Add street field
        self.street = tk.Entry(self.entry_frame, bg = 'white', width = 40)
        self.street.grid(row = 2, column = 2, columnspan = 6, sticky = 'W')

        # Add city label
        self.city_label = tk.Label(self.entry_frame, bg = 'white', text = "City")
        self.city_label.grid(row = 3, column = 1, sticky = "E")
        # Add city field
        self.city = tk.Entry(self.entry_frame, bg = 'white', width = 19)
        self.city.grid(row = 3, column = 2)

        # Add state label
        self.state_label = tk.Label(self.entry_frame, bg = 'white', text = "State")
        self.state_label.grid(row = 3, column = 3, sticky = 'E')
        # Add state field
        self.state = tk.Entry(self.entry_frame, bg = 'white', width = 5)
        self.state.grid(row = 3, column = 4)

        # Add zip label
        self.zip_label = tk.Label(self.entry_frame, bg = 'white', text = "Zip")
        self.zip_label.grid(row = 3, column = 5, sticky = 'E')
        # Add zip field
        self.zip = tk.Entry(self.entry_frame, bg = 'white', width = 5)
        self.zip.grid(row = 3, column = 6)


        # Add button frame
        self.button_frame = tk.Frame(self.window)
        self.button_frame.grid(row = 2)

        # Add add button
        self.add_button = tk.Button(self.button_frame, bg = 'green', 
                                        text = "Add", command = self.add_entries)
        self.add_button.grid(row = 1, column = 1)

        # Add first button
        self.first_button = tk.Button(self.button_frame, bg = 'green', 
                                        text = "First", command = self.go_to_first)
        self.first_button.grid(row = 1, column = 3)

        # Add previous button
        self.prev_button = tk.Button(self.button_frame, bg = 'green', 
                                        text = "Prev", command = self.go_to_prev)
        self.prev_button.grid(row = 1, column = 4)

        # Add next button
        self.next_button = tk.Button(self.button_frame, bg = 'green', 
                                        text = "Next", command = self.go_to_next)
        self.next_button.grid(row = 1, column = 5)

        # Add last button
        self.last_button = tk.Button(self.button_frame, bg = 'green', 
                                        text = "Last", command = self.go_to_last)
        self.last_button.grid(row = 1, column = 6)


        # Add file frame
        self.file_frame = tk.Frame(self.window)
        self.file_frame.grid(row = 3)

        # Add filename label
        self.filename_label = tk.Label(self.file_frame, bg = 'white', text = "Filename")
        self.filename_label.grid(row = 1, column = 1, sticky = "W")

        # Add filename field
        self.filename = tk.Entry(self.file_frame, bg = 'white', width = 10)
        self.filename.grid(row = 1, column = 2)

        # Add load file button
        self.loadfile_button = tk.Button(self.file_frame, 
                                        text = "Load File", command = self.load_file)
        self.loadfile_button.grid(row = 1, column = 3)

        # Add save to file button
        self.save_button = tk.Button(self.file_frame, 
                                        text = "Save to File", command = self.save_to_file)
        self.save_button.grid(row = 1, column = 4)

        # Add quit button
        self.quit_button = tk.Button(self.file_frame, 
                                        text = "Quit", command = self.quit)
        self.quit_button.grid(row = 1, column = 5)

        # Add a delete button
        self.delete_button = tk.Button(self.button_frame, 
                                        text = "Delete", command = self.delete)
        self.delete_button.grid(row = 1, column = 2)

        
        # Initialize the address list and counter variable
        self.address_list = []
        self.count = 0

        # Start the GUI event loop
        self.window.mainloop()


    def add_entries(self):
        """Creates an address object and adds it to the address list"""
        address = Address([self.name.get(), self.street.get(), self.city.get(), self.state.get(), self.zip.get()])
        self.address_list.append(address)
        self.count = len(self.address_list) - 1
        
    def delete(self):
        """Delete the address currently being displayed"""
        if len(self.address_list) == 0:
            pass
        elif len(self.address_list) == 1: # Empty address list
            self.address_list.pop(self.count)
            self.put_text(Address(['', '', '', '', '']))
        elif self.count == len(self.address_list) - 1: # Last address
            self.address_list.pop(self.count)
            self.count -= 1
            self.put_text(self.address_list[self.count])
        else:
            self.address_list.pop(self.count)
            self.put_text(self.address_list[self.count])
        
    def go_to_first(self):
        """Display the first address in the list"""
        if(len(self.address_list) > 0):
            self.count = 0
            self.put_text(self.address_list[self.count])

    def go_to_prev(self):
        """Display the previous address in the list"""
        if self.count > 0:
            self.count -= 1
            self.put_text(self.address_list[self.count])

    def go_to_next(self):
        """Display the next address in the list"""
        if self.count < len(self.address_list) - 1:
            self.count += 1
            self.put_text(self.address_list[self.count])

    def go_to_last(self):
        """Display the last address in the list"""
        if(len(self.address_list) > 0):
            self.count = len(self.address_list) - 1
            self.put_text(self.address_list[self.count])

    def load_file(self):
        """Load an address list from a file"""
        try:
            # Open the file and reset the local address list
            file_data = open(self.filename.get(), "r")
            self.address_list = []

            # Read every line from the file
            line_list = []
            for line in file_data:
                line = line.rstrip()
                line_list.append(line)

            # Put the data into the appropriate locations
            for i in range(0, len(line_list), 5):
                name = line_list[i]
                street = line_list[i + 1]
                city = line_list[i + 2]
                state = line_list[i + 3]
                zipcode = line_list[i + 4]

                address = Address([name, street, city, state, zipcode])
                self.address_list.append(address)
            file_data.close() 

            # Display the first address
            self.count = 0
            self.go_to_first()
        except OSError:
            print("File cannot be opened. ")
    
    def save_to_file(self):
        """Save the local address list to a file"""
        try:
            file_data = open(self.filename.get(), "w")

            if(len(self.address_list) > 0):
                file_data.write(self.address_list[0].string())
                for i in range(1, len(self.address_list)):
                    file_data.write('\n' + self.address_list[i].string())
            else:
                print("Address list is empty, nothing will be saved to file\n")

            file_data.close()
        except OSError:
            print("File cannot be opened. ")
 
    def quit(self):
        """DESTROY everything"""
        self.window.destroy()
    
    def put_text(self, address):
        """Display address in the entry fields"""
        self.name.delete(0, tk.END) # Delete current text
        self.name.insert(0, address.name) # Insert new text
        self.street.delete(0, tk.END)
        self.street.insert(0, address.street)
        self.city.delete(0, tk.END)
        self.city.insert(0, address.city)
        self.state.delete(0, tk.END)
        self.state.insert(0, address.state)
        self.zip.delete(0, tk.END)
        self.zip.insert(0, address.zip)
        
        
class Address:
    
    def __init__(self, address):
        """Initialize the address object"""
        self.name = address[0]
        self.street = address[1]
        self.city = address[2]
        self.state = address[3]
        self.zip = address[4]

    def string(self):
        """Format the address as a string"""
        return "%s\n%s\n%s\n%s\n%s" % (self.name, self.street, self.city, self.state, self.zip)



if __name__ == "__main__":
    # Create GUI
    AddressBook()
