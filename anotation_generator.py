import tkinter as tk
from tkinter import filedialog, simpledialog
from PIL import Image, ImageTk
import csv
import os

class ImageCoordinatePicker:
    def __init__(self, root):  # Renamed from 'init' to '__init__'
        self.root = root
        self.root.title("Image Coordinate Picker")
        self.root.geometry("1000x700")
        
        # Create widgets
        self.canvas = tk.Canvas(root, cursor="cross")
        self.canvas.pack(fill=tk.BOTH, expand=True)
        
        self.status_bar = tk.Label(root, text="Click on the image", bd=1, relief=tk.SUNKEN, anchor=tk.W)
        self.status_bar.pack(side=tk.BOTTOM, fill=tk.X)
        
        # Setup menu
        menubar = tk.Menu(root)
        filemenu = tk.Menu(menubar, tearoff=0)
        filemenu.add_command(label="Open", command=self.open_image)
        filemenu.add_command(label="Exit", command=root.quit)
        menubar.add_cascade(label="File", menu=filemenu)
        
        annotatemenu = tk.Menu(menubar, tearoff=0)
        annotatemenu.add_command(label="Start Annotation", command=self.start_annotation)
        annotatemenu.add_command(label="Cancel Annotation", command=self.cancel_annotation)
        annotatemenu.add_command(label="Finish Annotation", command=self.finish_annotation)
        menubar.add_cascade(label="Annotate", menu=annotatemenu)
        
        root.config(menu=menubar)
        
        # Initialize variables
        self.image = None
        self.photo_image = None
        self.scale_factor = 1.0
        self.annotation_in_progress = False
        self.current_place = ""
        self.current_points = []
        self.csv_file = "coords.csv"
        
        # Create CSV file if it doesn't exist
        self.create_csv_file()

    def create_csv_file(self):
        if not os.path.exists(self.csv_file):
            with open(self.csv_file, 'w', newline='') as file:
                writer = csv.writer(file)
                writer.writerow(['place', 'x1', 'y1', 'x2', 'y2', 'x3', 'y3', 'x4', 'y4'])

    def open_image(self):
        if self.annotation_in_progress:
            self.cancel_annotation()
            
        # Open file dialog with an initial directory
        file_path = filedialog.askopenfilename(
            initialdir=os.path.expanduser("./dataset/"),  # Set the initial directory to the user's home directory
            filetypes=[
                ("Image files", "*.jpg;*.jpeg;*.png;*.bmp;*.gif"),  # Filter for image files
                ("All files", "*.*")  # Option to show all files
            ]
        )
        if not file_path:
            return
            
        # Load image and get dimensions
        self.image = Image.open(file_path)
        self.original_width, self.original_height = self.image.size
        
        # Scale image if too large
        self.scale_factor = 1.0
        max_size = 800
        if self.original_width > max_size or self.original_height > max_size:
            self.scale_factor = max_size / max(self.original_width, self.original_height)
            new_size = (int(self.original_width * self.scale_factor), 
                        int(self.original_height * self.scale_factor))
            self.image = self.image.resize(new_size, Image.LANCZOS)
        
        # Display image
        self.photo_image = ImageTk.PhotoImage(self.image)
        self.canvas.config(width=self.photo_image.width(), height=self.photo_image.height())
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.photo_image)
        
        # Bind click event
        self.canvas.bind("<Button-1>", self.on_canvas_click)

    def on_canvas_click(self, event):
        # Calculate original coordinates
        x_scaled = event.x
        y_scaled = event.y
        x_original = int(x_scaled / self.scale_factor)
        y_original = int(y_scaled / self.scale_factor)
        
        if self.annotation_in_progress:
            self.handle_annotation_click(x_original, y_original, x_scaled, y_scaled)
        else:
            self.status_bar.config(
                text=f"Original coordinates: ({x_original}, {y_original}) | "
                     f"Displayed coordinates: ({x_scaled}, {y_scaled})"
            )
    def start_annotation(self):
        if not self.image:
            self.status_bar.config(text="Please open an image first!")
            return
            
        self.cancel_annotation()  # Reset any previous annotation
        place = simpledialog.askstring("Place Name", "Enter place name:")
        if not place:
            return
            
        self.annotation_in_progress = True
        self.current_place = place
        self.current_points = []
        self.status_bar.config(text=f"Annotation started for '{place}'. Click for point 1 (x1, y1)")

    def cancel_annotation(self):
        self.annotation_in_progress = False
        self.current_place = ""
        self.current_points = []
        self.canvas.delete("annotation")
        self.status_bar.config(text="Annotation cancelled")

    def finish_annotation(self):
        if self.annotation_in_progress and len(self.current_points) > 0:
            self.status_bar.config(text="Complete the current annotation first!")
            return
            
        self.annotation_in_progress = False
        self.status_bar.config(text="Annotation finished. Ready for new tasks.")

    def handle_annotation_click(self, x_orig, y_orig, x_scaled, y_scaled):
        point_number = len(self.current_points) + 1
        
        # Add point to current annotation
        self.current_points.append((x_orig, y_orig))
        
        # Draw marker on canvas
        marker_size = 5
        marker = self.canvas.create_oval(
            x_scaled - marker_size, y_scaled - marker_size,
            x_scaled + marker_size, y_scaled + marker_size,
            fill='red', outline='black', tags="annotation"
        )
        self.canvas.create_text(
            x_scaled + 10, y_scaled, 
            text=str(point_number), 
            fill='red', tags="annotation"
        )
        
        # Connect points with lines
        if len(self.current_points) > 1:
            prev_point = self.current_points[-2]
            prev_x = int(prev_point[0] * self.scale_factor)
            prev_y = int(prev_point[1] * self.scale_factor)
            self.canvas.create_line(
                prev_x, prev_y, x_scaled, y_scaled,
                fill='blue', width=2, tags="annotation"
            )
            
            # Close the polygon after 4 points
            if len(self.current_points) == 4:
                first_point = self.current_points[0]
                first_x = int(first_point[0] * self.scale_factor)
                first_y = int(first_point[1] * self.scale_factor)
                self.canvas.create_line(
                    x_scaled, y_scaled, first_x, first_y,
                    fill='blue', width=2, tags="annotation"
                )
        
        # Update status
        if point_number < 4:
            self.status_bar.config(
                text=f"Place '{self.current_place}': Click for point {point_number+1} (x{point_number+1}, y{point_number+1})"
            )
        else:
            self.status_bar.config(
                text=f"Place '{self.current_place}' completed! Select 'Start Annotation' for next place or 'Finish Annotation' to exit"
            )
            self.save_to_csv()
            self.annotation_in_progress = False

    def save_to_csv(self):
        if len(self.current_points) != 4:
            return
            
        with open(self.csv_file, 'a', newline='') as file:
            writer = csv.writer(file)
            row = [self.current_place]
            for point in self.current_points:
                row.extend(point)
            writer.writerow(row)
            
        # Reset for next annotation
        self.current_place = ""
        self.current_points = []

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageCoordinatePicker(root)
    root.mainloop()