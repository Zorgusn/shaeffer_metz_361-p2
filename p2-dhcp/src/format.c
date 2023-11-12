#include <stdio.h>

#include "dhcp.h"
#include "format.h"

void
dump_msg (FILE *output, msg_t *msg, size_t size)
{
  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "BOOTP Options\n");
  fprintf (output, "------------------------------------------------------\n");

  char *opstr = "INVALID OP";
  if (msg->op == BOOTREQUEST) {
    opstr = "BOOTREQUEST";
  } else if (msg->op == BOOTREPLY) {
    opstr = "BOOTREPLY";
  }
  fprintf(output, "Op Code (op) = %d [%s]\n", msg->op, opstr);

  char *htypestr = "INVALID HTYPE";
  if (msg->htype == ETH) {
    htypestr = "Ethernet (10Mb)";
  } else if (msg->htype == IEEE802) {
    htypestr = "IEEE 802 Networks";
  } else if (msg->htype == ARCNET) {
    htypestr = "ARCNET";
  } else if (msg->htype == FRAME_RELAY) {
    htypestr = "Frame Relay";
  } else if (msg->htype == FIBRE) {
    htypestr = "Fibre Channel";
  } else if (msg->htype == ATM) {
    htypestr = "Asynchronous Transmission Mode (ATM)";
  } 
  fprintf(output, "Hardware Type (htype) = %d [%s]\n", msg->htype, htypestr);
  fprintf(output, "Hardware Address Length (hlen) = %d\n", msg->hlen);
  fprintf(output, "Hops (hops) = %d\n", msg->hops);
  fprintf(output, "Transaction ID (xid) = %d (0x%x)\n", msg->xid, msg->xid);
  
  uint16_t seconds = msg->secs;
  uint16_t days = seconds / 86400;
  seconds %= 86400;
  uint16_t hours = seconds / 3600;
  seconds %= 3600;
  uint16_t minutes = seconds / 60;
  seconds %= 60;
  fprintf(output, "Seconds (secs) = %d Days, %d:%d:%d\n", days, hours, minutes, seconds);
  fprintf(output, "Flags (flags) = %d\n", msg->flags);
  
  fprintf(output, "Client IP Address (ciaddr) = \n");
  fprintf(output, "Your IP Address (yiaddr) = \n");
  fprintf(output, "Server IP Address (siaddr) = \n");
  fprintf(output, "Relay IP Address (giaddr) = \n");
  fprintf(output, "Client Ethernet Address (chaddr) = %x \n", msg->chaddr);

  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "DHCP Options\n");
  fprintf (output, "------------------------------------------------------\n");

  // TODO: Print out the DHCP fields as specified in the assignment

}
