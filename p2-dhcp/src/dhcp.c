#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dhcp.h"

// This dump_packet is provided code | DONT TOUCH
void
dump_packet (uint8_t *ptr, size_t size)
{
  size_t index = 0;
  while (index < size)
    {
      fprintf (stderr, " %02" PRIx8, ptr[index++]);
      if (index % 32 == 0)
        fprintf (stderr, "\n");
      else if (index % 16 == 0)
        fprintf (stderr, "  ");
      else if (index % 8 == 0)
        fprintf (stderr, " .");
    }
  if (index % 32 != 0)
    fprintf (stderr, "\n");
  fprintf (stderr, "\n");
}

void
make_default_msg (msg_t *msg)
{
  // memset (&msg, 0, sizeof (msg_t));
  msg->op = 1;
  msg->htype = 1;
  msg->hlen = 6;
  msg->hops = 0;
  msg->xid = 42;

  uint8_t defarr[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
  for (int i = 0; i < 6; i++)
    {
      msg->chaddr[i] = defarr[i];
    }

  msg->options[0] = 0x63;
  msg->options[1] = 0x82;
  msg->options[2] = 0x53;
  msg->options[3] = 0x63;

  msg->options[4] = 53;
  msg->options[5] = 1;
  msg->options[6] = DHCPDISCOVER;

  // uint8_t *saddr = (uint8_t *)&msg->siaddr;
  // saddr[0] = 127;
  // saddr[1] = 0;
  // saddr[2] = 0;
  // saddr[3] = 1;
  // msg->siaddr = *(uint32_t *)saddr;

  // uint8_t *gaddr = (uint8_t *)&msg->giaddr;
  // gaddr[0] = 127;
  // gaddr[1] = 0;
  // gaddr[2] = 0;
  // gaddr[3] = 2;
  // msg->giaddr = *(uint32_t *)gaddr;
}