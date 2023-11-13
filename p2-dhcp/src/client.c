#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

static bool get_args (int, char **, msg_t *);

int
main (int argc, char **argv)
{
  msg_t msg;
  memset (&msg, 0, sizeof (msg_t));

  if (!get_args (argc, argv, &msg))
    {
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

static bool
get_args (int argc, char **argv, msg_t *msg)
{
  int option;

  while ((option = getopt (argc, argv, "x:t:c:m:s:r:p")) != -1)
    {
      switch (option)
        {
        // x: use N as the XID field (32-bit unsigned integer)
        //    [default 42]
        case 'x':
          if (optarg)
            {
              msg->xid = atoi (optarg);
            }
          else
            {
              msg->xid = 42;
            }
          break;
        // t: use N as the hardware type (must be named in src/dhcp.h)
        //    [default ETH]
        case 't':
          //===================================================================
          switch (atoi (optarg))
            {
            case IEEE802:
              msg->htype = IEEE802;
              break;
            case ARCNET:
              msg->htype = ARCNET;
              break;
            case FRAME_RELAY:
              msg->htype = FRAME_RELAY;
              break;
            case FIBRE:
              msg->htype = FIBRE;
              break;
            case ATM:
              msg->htype = ATM;
              break;
            case ETH: // ETH gets wrapped up as the default val
            default:
              msg->htype = ETH;
              break;
            }
          //===================================================================
          break;
        // c: use N as the hardware address (chaddr)
        //    [default 0x010203040506]
        case 'c':;
          long tmpchaddr = 0;
          if (optarg)
            {
              tmpchaddr = atol (optarg);
            }
          else
            {
              tmpchaddr = 0x010203040506;
            }
          uint8_t *tmparr = (uint8_t *)tmpchaddr;
          for (int i = 0; i < 16; i++)
            {
              msg->chaddr[i] = tmparr[i];
            }
          break;
        // m: create DHCP msg type M
        //    [default DHCPDISCOVER]
        case 'm':
          //===================================================================
          switch (atoi (optarg))
            {
            case DHCPOFFER:
              msg->options[6] = DHCPOFFER;
              break;
            case DHCPREQUEST:
              msg->options[6] = DHCPREQUEST;
              break;
            case DHCPDECLINE:
              msg->options[6] = DHCPDECLINE;
              break;
            case DHCPACK:
              msg->options[6] = DHCPACK;
              break;
            case DHCPNAK:
              msg->options[6] = DHCPNAK;
              break;
            case DHCPRELEASE:
              msg->options[6] = DHCPRELEASE;
              break;
            case DHCPDISCOVER:
            default:
              msg->options[6] = DHCPDISCOVER;
              break;
            }
          //===================================================================
          break;
        // s: specify the server IP DHCP option
        //    [default 127.0.0.1]
        case 's':;
          uint8_t saddr[4];
          if (optarg)
            {
              char *srest1, *srest2, *srest3, *srest4;
              saddr[0] = strtol (optarg, &srest1, 10);
              saddr[1] = strtol (srest1, &srest2, 10);
              saddr[2] = strtol (srest2, &srest3, 10);
              saddr[3] = strtol (srest3, &srest4, 10);
            }
          else
            {
              saddr[0] = 127;
              saddr[1] = 0;
              saddr[2] = 0;
              saddr[3] = 1;
            }
          msg->siaddr = *(uint32_t *)saddr;
          break;
        // r: specify the requested IP DHCP option
        //    [default [127.0.0.2]
        case 'r':
            // I think this is for giaddr?
            ;
          uint8_t gaddr[4];
          if (optarg)
            {
              char *grest1, *grest2, *grest3, *grest4;
              gaddr[0] = strtol (optarg, &grest1, 10);
              gaddr[1] = strtol (grest1, &grest2, 10);
              gaddr[2] = strtol (grest2, &grest3, 10);
              gaddr[3] = strtol (grest3, &grest4, 10);
            }
          else
            {
              gaddr[0] = 127;
              gaddr[1] = 0;
              gaddr[2] = 0;
              gaddr[3] = 2;
            }
          msg->giaddr = *(uint32_t *)gaddr;
          break;
        // p: initiate the protocol (send UDP packet)
        case 'p':
          // ignore for now
          break;
        default:
          return false;
        }
    }
  return true;
}
