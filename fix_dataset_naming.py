#!/usr/bin/env python3
"""
Fix dataset naming inconsistencies by standardizing case and handling conflicts.
This script addresses the issue where uppercase and lowercase letters in filenames
are being treated as the same, causing file conflicts.
"""

import os
import shutil
from collections import defaultdict

def analyze_dataset_naming(dataset_path):
    """Analyze the naming patterns in the dataset folder."""
    print("=== DATASET NAMING ANALYSIS ===")
    
    files = os.listdir(dataset_path)
    
    # Group files by their base letter/character
    letter_groups = defaultdict(list)
    
    for file in files:
        if file == 'capture.jpg':
            continue
            
        # Extract the letter/character part (first character before underscore)
        if '_' in file:
            letter = file.split('_')[0]
            letter_groups[letter.lower()].append(file)
    
    # Find conflicts between upper and lowercase
    conflicts = []
    for letter_lower in letter_groups:
        upper_files = [f for f in letter_groups[letter_lower] if f.startswith(letter_lower.upper())]
        lower_files = [f for f in letter_groups[letter_lower] if f.startswith(letter_lower.lower())]
        
        if upper_files and lower_files:
            conflicts.append({
                'letter': letter_lower,
                'upper_files': upper_files,
                'lower_files': lower_files
            })
    
    print(f"Found {len(conflicts)} letter conflicts:")
    for conflict in conflicts:
        print(f"\nLetter '{conflict['letter'].upper()}' conflict:")
        print(f"  Uppercase files ({len(conflict['upper_files'])}): {conflict['upper_files'][:3]}{'...' if len(conflict['upper_files']) > 3 else ''}")
        print(f"  Lowercase files ({len(conflict['lower_files'])}): {conflict['lower_files'][:3]}{'...' if len(conflict['lower_files']) > 3 else ''}")
    
    return conflicts, letter_groups

def create_renaming_strategy(conflicts):
    """Create a strategy to resolve naming conflicts using _upper/_down suffixes."""
    print("\n=== RENAMING STRATEGY ===")
    
    renaming_plan = []
    
    for conflict in conflicts:
        letter = conflict['letter']
        upper_files = conflict['upper_files']
        lower_files = conflict['lower_files']
        
        print(f"\nProcessing letter '{letter.upper()}':")
        
        # Strategy: Use _upper and _lower suffixes to distinguish cases
        # Uppercase letters: B_180_1.png -> B_180_1_upper.png
        # Lowercase letters: b_180_1.jpg -> b_180_1_lower.jpg
        
        print(f"  Renaming uppercase '{letter.upper()}' files to use '_upper' suffix")
        for file in upper_files:
            # Check if already has _upper suffix
            if '_upper.' in file:
                print(f"    Skipping {file} - already has _upper suffix")
                continue
                
            # B_180_1.png -> B_180_1_upper.png
            parts = file.split('_', 2)  # Split into max 3 parts: ['B', '180', '1.png']
            if len(parts) >= 3:
                char_part = parts[0]  # Keep original case (uppercase)
                rotation_part = parts[1]
                number_ext = parts[2]
                new_name = f"{char_part}_{rotation_part}_{number_ext.split('.')[0]}_upper.{number_ext.split('.')[1]}"
                reason = f"Uppercase {letter.upper()} -> {char_part}_upper to avoid case conflict"
            else:
                new_name = file  # Fallback if format doesn't match
                reason = f"Uppercase {letter.upper()} -> unchanged (format mismatch)"
            
            renaming_plan.append({
                'old': file,
                'new': new_name,
                'reason': reason
            })
        
        print(f"  Renaming lowercase '{letter.lower()}' files to use '_lower' suffix")
        for file in lower_files:
            # Check if already has _lower suffix
            if '_lower.' in file:
                print(f"    Skipping {file} - already has _lower suffix")
                continue
                
            # b_180_1.jpg -> b_180_1_lower.jpg
            parts = file.split('_', 2)  # Split into max 3 parts: ['b', '180', '1.jpg']
            if len(parts) >= 3:
                char_part = parts[0]  # Keep lowercase as is
                rotation_part = parts[1]
                number_ext = parts[2]
                new_name = f"{char_part}_{rotation_part}_{number_ext.split('.')[0]}_lower.{number_ext.split('.')[1]}"
                reason = f"Lowercase {letter.lower()} -> {char_part}_lower to avoid case conflict"
            else:
                new_name = file  # Fallback if format doesn't match
                reason = f"Lowercase {letter.lower()} -> unchanged (format mismatch)"
            
            renaming_plan.append({
                'old': file,
                'new': new_name,
                'reason': reason
            })
    
    return renaming_plan

def execute_renaming(dataset_path, renaming_plan, dry_run=True):
    """Execute the renaming plan."""
    print(f"\n=== {'DRY RUN - ' if dry_run else ''}EXECUTING RENAMING ===")
    
    for item in renaming_plan:
        old_path = os.path.join(dataset_path, item['old'])
        new_path = os.path.join(dataset_path, item['new'])
        
        print(f"{'[DRY RUN] ' if dry_run else ''}Renaming: {item['old']} -> {item['new']}")
        print(f"  Reason: {item['reason']}")
        
        if not dry_run:
            if os.path.exists(old_path):
                if os.path.exists(new_path):
                    print(f"  WARNING: Target file {item['new']} already exists! Skipping.")
                else:
                    # Rename file (this removes the original and creates the new one)
                    shutil.move(old_path, new_path)
                    print(f" Renamed successfully (original {item['old']} removed)")
            else:
                print(f"  ERROR: Source file {item['old']} not found!")

def main():
    dataset_path = "./dataset"
    
    if not os.path.exists(dataset_path):
        print(f"Error: Dataset path {dataset_path} not found!")
        return
    
    # Step 1: Analyze current naming
    conflicts, _ = analyze_dataset_naming(dataset_path)
    
    if not conflicts:
        print("\n No case conflicts found in dataset naming!")
        return
    
    # Step 2: Create renaming strategy
    renaming_plan = create_renaming_strategy(conflicts)
    
    print("\n=== RENAMING PLAN SUMMARY ===")
    print(f"Total files to rename: {len(renaming_plan)}")
    
    # Step 3: Show dry run first
    execute_renaming(dataset_path, renaming_plan, dry_run=True)
    
    # Step 4: Ask for confirmation
    print(f"\n{'='*50}")
    print("The above shows what will be renamed.")
    response = input("Do you want to proceed with the actual renaming? (y/N): ")
    
    if response.lower() in ['y', 'yes']:
        execute_renaming(dataset_path, renaming_plan, dry_run=False)
        print("\n Renaming completed!")
        
        # Show final status
        print("\n=== FINAL VERIFICATION ===")
        analyze_dataset_naming(dataset_path)
    else:
        print("Renaming cancelled.")

if __name__ == "__main__":
    main()
