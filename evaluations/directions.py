import string
from pprint import pprint
eval_list = []

numbers = "0123456789"
uppercase = string.ascii_uppercase
lowercase = string.ascii_lowercase
all_characters = [numbers, uppercase , lowercase]

def get_directions(entities, direction):
    for i in entities:
        en_direction = (i, direction)
        eval_list.append(en_direction)

for chars in all_characters:
        for direction in range(3, -1, -1):
            direction = direction * 90
            get_directions(chars, direction)
        
# pprint(eval_list)
        