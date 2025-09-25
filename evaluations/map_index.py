import string
from pprint import pprint
from directions import eval_list

path_list = []
input_dir = "/home/hossein/CharRecognition/image-reterieval/test_results_new"

for fol in range(1, 7):
    for row in range(1, 8):
        for col in range(1, 7):
            path_list.append(f"{input_dir}/{fol}/({row},{col}).txt")
            
import re

def find_first_best_match(text: str):
    """
    Finds the first best match in the given recognition log text.
    
    Returns:
        dict with {letter, rotation, distance} or None if not found
    """
    # Regex to capture: Best match: <letter> (rotation: <deg>) : <distance>
    match = re.search(r"Best match:\s+(\S+)\s+\(rotation:\s+([^)]+)\)\s*.*?\n", text)
    
    if match:
        letter = match.group(1)
        rotation = match.group(2)
        
        # Extract the numeric distance from Top 5 matches (same line as best match)
        distance_match = re.search(
            rf"{re.escape(letter)} \(rotation: {re.escape(rotation)}\):\s*(\d+)", text
        )
        distance = int(distance_match.group(1)) if distance_match else None

        return {
            "letter": letter,
            "rotation": rotation,
            "distance": distance
        }
    return None

count_files = 0
error_files = 0
no_match_found = 0
mismatch_count = 0
best_results = []

for i, p in enumerate(path_list[:248]):
    try:
        with open(p, 'r', encoding='utf-8') as f:
            content = f.read()
            result = find_first_best_match(content)
            if result:
                # print(f"{p} -> {result}")
                # print(f"Expected: {eval_list[i]}")
                if not result["letter"] == eval_list[i][0]:
                    print("\n-\n========================\n")
                    print(f"{p} -> MISMATCH! Found: {result}, Expected: {eval_list[i]}")
                    print("\n++++++++++++++++++++++++++\n")
                    mismatch_count += 1
                best_results.append((p, result, result["distance"]))
            else:
                print(f"{p} -> No match found")
                no_match_found += 1
            count_files += 1
    except FileNotFoundError:
        print(f"File not found: {p}")
        error_files += 1
    except Exception as e:
        print(f"Error processing {p}: {e}")
        error_files += 1

# Summary
print("\n=== Summary ===")
print(f"Total files processed: {count_files}")
print(f"Files with errors: {error_files}")
print(f"Files with no matches found: {no_match_found}")
print(f"Files with mismatches: {mismatch_count}")
print(f"top five best results: {sorted(best_results, key=lambda x: x[2])[:5]}")