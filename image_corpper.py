import cv2
import pandas as pd
import numpy as np

# Configuration
ROTATE_180 = True   # Set to True for 180° rotation, False to disable
SQUARE = True       # Set to True to map to 32x32 square, False to keep original shape
CSV_FILE = 'coords.csv'
IMAGE_FILE = 'capture8.jpg'
SQUARE_SIZE = 64    # Size of the output square

# Get user input
# place_name = input("Enter place name: ").strip()
# place_name = "(7, 7)"
i = 1 #row
a = [(i,1), (i,2), (i,3), (i,4), (i,5), (i,6), (i,7)]
for j in a:
    place_name = str(j).replace(" ", "").replace("_", "")
    print(place_name)

    # Read CSV file
    df = pd.read_csv(CSV_FILE)
    df.columns = [col.strip() for col in df.columns]  # Clean column names

    # Find coordinates for the specified place
    place_row = df[df['place'].str.strip() == place_name]
    if place_row.empty:
        print(f"Place '{place_name}' not found in CSV!")
        exit()

    # Extract coordinates in correct order
    coords = place_row.iloc[0, 1:].values.astype(float)
    src_points = np.array([
        [coords[0], coords[1]],  # x1,y1 (top-left)
        [coords[2], coords[3]],  # x2,y2 (bottom-left)
        [coords[4], coords[5]],  # x3,y3 (bottom-right)
        [coords[6], coords[7]]   # x4,y4 (top-right)
    ], dtype=np.float32)

    # Read image
    image = cv2.imread(IMAGE_FILE)
    if image is None:
        print(f"Error loading image: {IMAGE_FILE}")
        exit()

    if SQUARE:
        # Define destination points for 32x32 square
        dst_points = np.array([
            [0, 0],                      # top-left
            [0, SQUARE_SIZE-1],          # bottom-left
            [SQUARE_SIZE-1, SQUARE_SIZE-1],  # bottom-right
            [SQUARE_SIZE-1, 0]           # top-right
        ], dtype=np.float32)
        
        # Calculate perspective transform matrix
        matrix = cv2.getPerspectiveTransform(src_points, dst_points)
        
        # Apply perspective transformation
        result = cv2.warpPerspective(image, matrix, (SQUARE_SIZE, SQUARE_SIZE))
        
        # Convert to BGRA (for potential rotation with transparency)
        result = cv2.cvtColor(result, cv2.COLOR_BGR2BGRA)
    else:
        # Create mask for the quadrilateral
        mask = np.zeros(image.shape[:2], dtype=np.uint8)
        cv2.fillPoly(mask, [src_points.astype(int)], 255)
        
        # Get bounding rectangle of the quadrilateral
        x, y, w, h = cv2.boundingRect(src_points.astype(int))
        cropped = image[y:y+h, x:x+w]
        cropped_mask = mask[y:y+h, x:x+w]
        
        # Create transparent version
        result = cv2.cvtColor(cropped, cv2.COLOR_BGR2BGRA)
        result[:, :, 3] = cropped_mask  # Set alpha channel

    # Rotate 180° if flag is set
    if ROTATE_180:
        result = cv2.rotate(result, cv2.ROTATE_180)

    # Save result
    output_filename = f"{place_name.replace(' ', '_')}.png"
    cv2.imwrite(output_filename, result)
    print(f"Saved cropped image as {output_filename}")
