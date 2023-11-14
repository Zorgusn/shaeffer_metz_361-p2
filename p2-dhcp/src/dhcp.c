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
  // 0 out all bits
  memset (msg, 0, sizeof (msg_t));

  msg->op = BOOTREQUEST;  // default op
  msg->htype = ETH;       // default htype
  msg->hlen = ETH_LEN;    // default hlen (correlated to default htype)
  msg->hops = 0;          // 0 hops completed yet
  msg->xid = 42;          // def xid
  
  //default chaddr (correct endianness)
  uint8_t defarr[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
  for (int i = 0; i < 6; i++)
    {
      msg->chaddr[i] = defarr[i];
    }

  msg->options[0] = 0x63; // magic num
  msg->options[1] = 0x82; // magic num
  msg->options[2] = 0x53; // magic num
  msg->options[3] = 0x63; // magic num

  msg->options[4] = 53;             // message type option id
  msg->options[5] = 1;              // message type len
  msg->options[6] = DHCPDISCOVER;   // default message type
}