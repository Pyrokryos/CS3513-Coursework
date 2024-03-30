#include <stdbool.h>

#include "../include/lexer.h"

int main()
{
  char *ptr = "// This is the main function. \nint main() {\n\tprintf(\"Hello, world!\"); // Print to the console.\n\treturn 0;\n}";

  TokenStream *stream = lex(ptr);
  display_list(stream);
  return 0;
}
