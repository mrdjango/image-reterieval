import os
import shutil
from pathlib import Path

def rename_and_move_images():
    """
    Move all images from old_dataset subfolders to dataset folder
    with naming schema: {folder_name}_0_{count}.jpg
    """
    
    # Define source and destination directories
    source_dir = Path("old_dataset")
    dest_dir = Path("dataset")
    
    # Create destination directory if it doesn't exist
    dest_dir.mkdir(exist_ok=True)
    
    # Supported image extensions
    image_extensions = {'.jpg', '.jpeg', '.png', '.bmp', '.tiff', '.tif'}
    
    # Counter for tracking total images processed
    total_images = 0
    
    # Process each subfolder in dataset_letters_digital
    for folder_name in os.listdir(source_dir):
        folder_path = source_dir / folder_name
        
        # Skip if not a directory
        if not folder_path.is_dir():
            continue
            
        print(f"Processing folder: {folder_name}")
        
        # Get all image files in the current folder
        image_files = []
        for file in folder_path.iterdir():
            if file.is_file() and file.suffix.lower() in image_extensions:
                image_files.append(file)
        
        # Sort files to ensure consistent ordering
        image_files.sort()
        
        # Take only the first image from each folder
        if image_files:
            image_file = image_files[-1]  # Take the first image
            # Create new filename with schema: {folder_name}_0_1.jpg
            new_filename = f"{folder_name}_0_1.jpg"
            dest_path = dest_dir / new_filename
            
            # Copy the file to destination with new name
            try:
                shutil.copy2(image_file, dest_path)
                print(f"  Moved: {image_file.name} -> {new_filename}")
                total_images += 1
            except Exception as e:
                print(f"  Error moving {image_file.name}: {e}")
        else:
            print(f"  No images found in folder: {folder_name}")
    
    print(f"\nTotal images processed: {total_images}")
    print(f"All images moved to: {dest_dir.absolute()}")

if __name__ == "__main__":
    rename_and_move_images()
