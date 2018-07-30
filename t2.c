#include <alloca.h>
#include <string.h>

main() {
  char* c=alloca(1234);
  strcpy(c,"fnord");
}
