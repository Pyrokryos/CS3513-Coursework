#include <stdbool.h>
#include <stdio.h>

#include "../include/lexer.h"

int main()
{
  char *ptr = "     \t  \n//This is the main function. \nint main() {\n\tprintf(\"Hello, world!\"); // Print to the console.\n\treturn 0;\n}";
  char *input = ptr;

  input = ignore_whitespace(ptr);
  printf("After ignoring whitespace: \n%s\n", input);
  return 0;
}
