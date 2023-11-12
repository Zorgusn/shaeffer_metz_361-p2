#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "dhcp.h"
#include "format.h"

bool
check_magic_cookie (uint8_t *options)
{
  for (int i = 0; i < 4; i++)
    {
      if (options[i] != ((MAGIC_COOKIE >> (24 - (8 * i))) & 0xFF))
        {
          return false;
        }
    }
  return true;
}

void
dump_msg (FILE *output, msg_t *msg, size_t size)
{
  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "BOOTP Options\n");
  fprintf (output, "------------------------------------------------------\n");

  char *opstr = "INVALID OP";
  if (msg->op == BOOTREQUEST)
    {
      opstr = "BOOTREQUEST";
    }
  else if (msg->op == BOOTREPLY)
    {
      opstr = "BOOTREPLY";
    }
  fprintf (output, "Op Code (op) = %d [%s]\n", msg->op, opstr);

  char *htypestr = "INVALID HTYPE";
  if (msg->htype == ETH)
    {
      htypestr = "Ethernet (10Mb)";
    }
  else if (msg->htype == IEEE802)
    {
      htypestr = "IEEE 802 Networks";
    }
  else if (msg->htype == ARCNET)
    {
      htypestr = "ARCNET";
    }
  else if (msg->htype == FRAME_RELAY)
    {
      htypestr = "Frame Relay";
    }
  else if (msg->htype == FIBRE)
    {
      htypestr = "Fibre Channel";
    }
  else if (msg->htype == ATM)
    {
      htypestr = "Asynchronous Transmission Mode (ATM)";
    }
  fprintf (output, "Hardware Type (htype) = %d [%s]\n", msg->htype, htypestr);
  fprintf (output, "Hardware Address Length (hlen) = %d\n", msg->hlen);
  fprintf (output, "Hops (hops) = %d\n", msg->hops);
  fprintf (output, "Transaction ID (xid) = %d (0x%x)\n", msg->xid, msg->xid);

  uint16_t seconds = msg->secs;
  uint16_t days = seconds / 86400;
  seconds %= 86400;
  uint16_t hours = seconds / 3600;
  seconds %= 3600;
  uint16_t minutes = seconds / 60;
  seconds %= 60;
  fprintf (output, "Seconds (secs) = %d Days, %d:%02d:%02d\n", days, hours,
           minutes, seconds);
  fprintf (output, "Flags (flags) = %d\n", msg->flags);
  unsigned char *cibytes = (unsigned char *)&msg->ciaddr;
  fprintf (output, "Client IP Address (ciaddr) = %u.%u.%u.%u\n", cibytes[0],
           cibytes[1], cibytes[2], cibytes[3]);
  unsigned char *yibytes = (unsigned char *)&msg->yiaddr;
  fprintf (output, "Your IP Address (yiaddr) = %u.%u.%u.%u\n", yibytes[0],
           yibytes[1], yibytes[2], yibytes[3]);
  unsigned char *sibytes = (unsigned char *)&msg->siaddr;
  fprintf (output, "Server IP Address (siaddr) = %u.%u.%u.%u\n", sibytes[0],
           sibytes[1], sibytes[2], sibytes[3]);
  unsigned char *gibytes = (unsigned char *)&msg->giaddr;
  fprintf (output, "Relay IP Address (giaddr) = %u.%u.%u.%u\n", gibytes[0],
           gibytes[1], gibytes[2], gibytes[3]);
  fprintf (output, "Client Ethernet Address (chaddr) = ");

  size_t index = 0;
  while (index < msg->hlen)
    {
      fprintf (output, "%02" PRIx8, msg->chaddr[index++]);
    }
  fprintf (output, "\n");

  // TODO: Print out the DHCP fields as specified in the assignment
  if (!check_magic_cookie (msg->options))
    {
      return;
    }
  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "DHCP Options\n");
  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "Magic Cookie = [OK]\n");
  size_t opt_i = 4;
  while (opt_i < 312) {
    uint8_t code = msg->options[opt_i];
    if (code == 0) {
      opt_i += 1;
      continue;
    } else if (code == 255) {
      break;
    }
    uint8_t len = msg->options[opt_i + 1];
    uint8_t *value = &msg->options[opt_i + 2];
    printf("%d (len %d) = TODO at %ld\n", code, len, opt_i); //TODO - Print code names and values instead of number representation
    opt_i += 2 + (int)len;
  }
}
