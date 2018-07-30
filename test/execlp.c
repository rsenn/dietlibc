#include <unistd.h>

main() {
  execlp("gcc","-v",0);
}
