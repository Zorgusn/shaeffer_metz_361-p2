#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

static bool get_args (int, char **, msg_t *, bool *p);

int
main (int argc, char **argv)
{
  msg_t msg;
  bool p;
  memset (&msg, 0, sizeof (msg_t));
  make_default_msg(&msg);

  if (!get_args (argc, argv, &msg, &p))
    {
      return EXIT_FAILURE;
    }
  dump_msg(stdout, &msg, sizeof(msg_t));
  if (p) {
    printf("\n");
    //TODO - Server stuff
  }

  return EXIT_SUCCESS;
}

static bool
get_args (int argc, char **argv, msg_t *msg, bool *p)
{
  int option;

  while ((option = getopt (argc, argv, "x:t:c:m:s:r:p")) != -1)
    {
      switch (option)
        {
        // x: use N as the XID field (32-bit unsigned integer)
        //    [default 42]
        case 'x':
          if (optarg != NULL)
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
        case 'c':; //TODO - SHOULD CHANGE HLEN
          long tmpchaddr = 0;
          if (optarg != NULL)
            {
              tmpchaddr = strtol (optarg, NULL, 16);
              int length = -1;
              long temp = tmpchaddr;
              while (temp != 0) {
                temp /= 10;
                length++;
              }
              msg->hlen = length; //TODO - Check hardware type for length?
              for (int i = 0; i < length; i++)
                {
                  msg->chaddr[length - 1 - i] = ((uint8_t *)&tmpchaddr)[i];
                }
            }
          else
            {
              uint8_t defarr[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
              for (int i = 0; i < 6; i++)
                {
                  msg->chaddr[i] = defarr[i];
                }
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
        case 's':; //TODO - THIS IS NOT SIADDR
          uint8_t saddr[4];
          if (optarg != NULL)
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
          // msg->siaddr = *(uint32_t *)saddr;
          break;
        // r: specify the requested IP DHCP option
        //    [default [127.0.0.2]
        case 'r': //TODO - THIS IS NOT GIADDR
            ;
          uint8_t gaddr[4];
          if (optarg != NULL)
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
          // msg->giaddr = *(uint32_t *)gaddr;
          break;
        // p: initiate the protocol (send UDP packet)
        case 'p':
          // ignore for now
          *p = true;
          break;
        default:
          return false;
        }
    }
  return true;
}
