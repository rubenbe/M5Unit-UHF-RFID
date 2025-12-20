#include "my_helpers.h"

#ifdef TEST
int main() {
  const auto x = MyHelper::format_csv(1000, "ABCDEF12345678");
  printf("%s\n", x.c_str());
  return 0;
}
#endif
