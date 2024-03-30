#include <stdio.h>

#include "../include/lexer.h"

int main()
{
  char *ptr = "//This is the main function. \nint main() {\n\tprintf(\"Hello, world!\"); // Print to the console.\n\treturn 0;\n}";
  char *input = ptr;

  input = match_comments(input);
  printf("%s\n", input);
  printf("%s\n", ptr);
  return 0;
}
