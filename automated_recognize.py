#!/usr/bin/env python3
"""
Automated Recognition Script
Runs the C++ recognize script on all images with pattern (x,y).png
and saves results to corresponding (x,y).txt files.
"""

import os
import subprocess
import sys

seq_num = 6
# Configuration - Modify these paths as needed
C_SCRIPT_PATH = "/home/hossein/CharRecognition/image-reterieval/src/build_cpu/recognize"           # Path to the C++ recognize executable
INPUT_IMAGE_DIR = f"/home/hossein/Downloads/grid/{seq_num}"         # Directory containing input images
OUTPUT_RESULTS_DIR = f"/home/hossein/CharRecognition/image-reterieval/test_results/{seq_num}"          # Directory to save output text files

def run_recognize_script(image_path, output_path, script_path):
    """
    Run the C++ recognize script on a given image and save output to file.
    
    Args:
        image_path (str): Path to the input image
        output_path (str): Path to save the output text file
        script_path (str): Path to the C++ recognize executable
    
    Returns:
        bool: True if successful, False otherwise
    """
    try:
        # Run the recognize script
        result = subprocess.run(
            [script_path, image_path],
            capture_output=True,
            text=True,
            check=True
        )
        
        # Write the output to the corresponding text file
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(result.stdout)
        
        print(f"Processed {str(image_path)} -> {output_path}")
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"Error processing : {e}")
        print(f"  stderr: {e.stderr}")
        return False
    except (OSError, IOError) as e:
        print(f"File error processing {image_path}: {e}")
        return False

def find_coordinate_images(directory):
    """
    Find all images with pattern (x,y).png in the given directory.
    
    Args:
        directory (str): Directory to search for images
    
    Returns:
        list: List of image file paths with coordinate pattern
    """
    coordinate_images = []
    
    if not os.path.exists(directory):
        print(f"Directory {directory} does not exist!")
        return coordinate_images
    
    # Look for images with pattern (x,y).png
    for file in os.listdir(directory):
        if file.endswith('.png') and file.startswith('(') and ')' in file:
            # Extract coordinates from filename like (1,1).png
            try:
                coord_part = file.split('.png')[0]  # Remove .png extension
                if coord_part.startswith('(') and coord_part.endswith(')'):
                    coord_content = coord_part[1:-1]  # Remove parentheses
                    if ',' in coord_content:
                        x, y = coord_content.split(',')
                        # Validate that x and y are numbers
                        int(x.strip())
                        int(y.strip())
                        coordinate_images.append(os.path.join(directory, file))
            except ValueError:
                # Skip files that don't match the expected pattern
                continue
    
    return sorted(coordinate_images)

def main():
    """
    Main function to process all coordinate images.
    """
    # Check if recognize script exists
    if not os.path.exists(C_SCRIPT_PATH):
        print(f"Error: {C_SCRIPT_PATH} script not found!")
        print("Please make sure the C++ recognize executable is compiled and available.")
        print("Or update the C_SCRIPT_PATH variable at the top of this script.")
        sys.exit(1)
    
    # Use configuration variables or command line arguments
    image_directory = INPUT_IMAGE_DIR
    output_directory = OUTPUT_RESULTS_DIR
    script_path = C_SCRIPT_PATH
    
    # Allow command line arguments to override defaults
    if len(sys.argv) > 1:
        script_path = sys.argv[1]
    if len(sys.argv) > 2:
        image_directory = sys.argv[2]
    if len(sys.argv) > 3:
        output_directory = sys.argv[3]
    
    print(f"Using C++ script: {script_path}")
    print(f"Looking for coordinate images in: {image_directory}")
    print(f"Output directory: {output_directory}")
    
    # Create output directory if it doesn't exist
    os.makedirs(output_directory, exist_ok=True)
    
    # Find all coordinate images
    coordinate_images = find_coordinate_images(image_directory)
    
    if not coordinate_images:
        print(f"No coordinate pattern images found in {image_directory}")
        print("Expected pattern: (x,y).png (e.g., (1,1).png, (7,6).png)")
        return
    
    print(f"Found {len(coordinate_images)} coordinate images to process:")
    for img in coordinate_images:
        print(f"  - {img}")
    
    # Process each image
    successful = 0
    failed = 0
    
    for image_path in coordinate_images:
        # Generate output filename
        image_name = os.path.basename(image_path)
        output_name = image_name.replace('.png', '.txt')
        output_path = os.path.join(output_directory, output_name)
        
        # Run recognition
        if run_recognize_script(image_path, output_path, script_path):
            successful += 1
        else:
            failed += 1
    
    # Summary
    print("\n=== Processing Complete ===")
    print(f"Successfully processed: {successful}")
    print(f"Failed: {failed}")
    print(f"Total: {successful + failed}")
    
    if failed > 0:
        print("\nSome images failed to process. Check the error messages above.")
        sys.exit(1)

if __name__ == "__main__":
    main()