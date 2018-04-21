#include <stdio.h>
#include <stdlib.h>
#include <fnode/service.h>

#define MAX_CMD_LEN 4 * 1024

char cmd_buffer[MAX_CMD_LEN];
const char * cmd_format = "./play.sh \"%s\"";

void makeSound(char const * data) {
  sprintf(cmd_buffer, cmd_format, data);
  system(cmd_buffer);
}

void handler(uint32_t cmd, char const * data) {
  makeSound(data);
}

int main(int argc, char ** argv) {
  // Creating service
  fnode_service_t * service = fnode_service_create("alice007", "TDAC");

  makeSound("курлы, курлы, курлы, курлы");
  makeSound("курлы, курлы, курлы, курлы");
  makeSound("курлы, курлы, курлы, курлы");

  if ( !service ) {
    fprintf(stderr, "Failed to create TDAC service\n");
    return -1;
  }

  fnode_service_reg_handler(service, handler);

  for(;;) {
    fnode_service_update(service);
  }

  fnode_service_release(service);
  return 0;
}
