#include <stdio.h>
#include <stdlib.h>

void die(const char* message) {
  puts(message);
  exit(1);
}

main() {
  char buf[100]="fnord";
  char buf2[100]="fnort";
  if (strcmp(buf,buf)) die("strcmp say a != a");
  if (strcmp(buf,buf2)>=0) die("strcmp said fnord > fnort");
  if (strcmp(buf2,buf)<=0) die("strcmp said fnort < fnord");
  return 0;
}
