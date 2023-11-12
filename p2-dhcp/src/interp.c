#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "dhcp.h"
#include "format.h"


int
main (int argc, char **argv)
{
  // TODO: Open the file specified on the command line
  char *filename = argv[1];
  FILE *bin = fopen(filename, "r");
  
  // Note: The file contains a binary network packet

  // TODO: Determine the size of the packet in bytes
  // Note: You have used the fstat() function before
  struct stat stats;
  fstat(fileno(bin), &stats);
  size_t size = stats.st_size;

  // TODO: Define a uint8_t * variable
  // uint8_t *var;

  // TODO: Allocate enough space to hold the packet (store in your varaible)
  msg_t *packet = calloc(1, size);

  // TODO: Read the packet data from the file into your variable
  fread(packet, size, 1, bin);

  packet->xid = ntohl(packet->xid);
  packet->secs = ntohs(packet->secs);
  packet->flags = ntohs(packet->flags);
  packet->ciaddr = ntohl(packet->ciaddr);
  packet->yiaddr = ntohl(packet->yiaddr);
  packet->siaddr = ntohl(packet->siaddr);
  packet->giaddr = ntohl(packet->giaddr);

  // TODO: Call dump_msg() from format.c to print the packet data to stdout
  dump_msg(stdout, packet, size);
  // TODO: Don't forget to free the dynamically-allocated space
  free(packet);

  return EXIT_SUCCESS;
}
