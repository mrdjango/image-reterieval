from PIL import Image
import glob
import os


# Find all files matching the pattern ?_0_?.jpg
for filepath in glob.glob('dataset/*_0_*.png'):
    # Split the filename into prefix and suffix using '_0_' as the separator
    parts = filepath.split('_0_', 1)
    if len(parts) != 2:
        continue  # Skip if the filename doesn't contain '_0_'
    prefix, suffix = parts
    
    # Open the original image
    with Image.open(filepath) as img:
        # Rotate 90° clockwise and save
        img_90 = img.transpose(Image.ROTATE_270)  # 270° counterclockwise = 90° clockwise
        img_90.save(f"{prefix}_90_{suffix}")
        print(f"Saved {prefix}_90_{suffix}")
        
        # Rotate 180° and save
        img_180 = img.transpose(Image.ROTATE_180)
        img_180.save(f"{prefix}_180_{suffix}")
        print(f"Saved {prefix}_180_{suffix}")

        # Rotate 270° clockwise (90° counterclockwise) and save
        img_270 = img.transpose(Image.ROTATE_90)  # 90° counterclockwise = 270° clockwise
        img_270.save(f"{prefix}_270_{suffix}")
        print(f"Saved {prefix}_270_{suffix}")

print("All images rotated and saved successfully!")