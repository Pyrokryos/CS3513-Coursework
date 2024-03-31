#include <stdbool.h>

#include "../include/lexer.h"
#include "../include/parser.h"

int main()
{
  char *ptr = "* let Sum(A) = Psum (A,Order A )\n\t\twhere rec Psum (T,N) = N eq 0 -> 0\n\t\t\t| Psum(T,N-1)+T N\nin Print ( Sum (1,2,3,4,5) )";

  TokenStream *stream = lex(ptr);
  display_list(stream);

  // parse(stream);
  free(stream);
  return 0;
}
