import re


def remove_spaces(input_str):
    # Define the regular expression pattern for spaces.
    pattern = r'[ \t\n]+'

    # Remove spaces from the input string.
    refined_str = re.sub(pattern, '', input_str)

    # Return the refined string.
    return refined_str


def remove_comments(input_str):
    # Define the regular expression pattern for comments.
    pattern = r'\/\/(?:[\'();,\\ \tA-Za-z0-9+\-*<>&.@/:=~|$!#%^_[\]{}"`?])*?\n'

    # Remove comments from the input string.
    refined_str = re.sub(pattern, '', input_str)

    # Return the refined string.
    return refined_str


input_str = '//This is the main function. \nint main() {\n\tprintf(\"Hello, world!\"); // Print to the console.\n\treturn 0;\n}'

input_str = remove_comments(input_str)
input_str = remove_spaces(input_str)

print(input_str)
