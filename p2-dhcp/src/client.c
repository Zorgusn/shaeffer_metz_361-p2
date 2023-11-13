#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

int
main (int argc, char **argv)
{
  msg_t msg;
  msg.op = 0;                   // TODO
  msg.htype = ETH;              // default
  msg.hlen = 0;                 // TODO
  msg.hops = 0;                 // TODO
  msg.xid = 42;                 // default
  msg.secs = 0;                 // TODO? 0 seconds elapsed?
  msg.flags = 0;                // TODO
  msg.ciaddr = 0;               // TODO client addr
  msg.yiaddr = 0;               // TODO your addr
  msg.siaddr = 0;               // TODO server addr
  msg.giaddr = 0;               // TODO relay addr
  memset (msg.chaddr, 0, 16);   // set to all 0s
  msg.chaddr[1] = 1;            // default val
  msg.chaddr[3] = 2;            // default val
  memset (msg.sname, 0, 64);    // TODO
  memset (msg.file, 0, 128);    // TODO
  memset (msg.options, 0, 312); // TODO

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
          msg->xid = atoi (optarg);
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
            case ETH:
              // noop: provided type same as default
            default:
              // noop: invalid htype provided, go with default
              break;
            }
          //===================================================================
          break;
        // c: use N as the hardware address (chaddr)
        //    [default 0102...]
        case 'c':
          // this should work?? hopefully
          *(msg->chaddr) = atol (optarg);
          break;
        // m: create DHCP msg type M
        //    [default DHCPDISCOVER]
        case 'm':
          // TODO: not sure where to put this in the options array
          break;
        // s: specify the server IP DHCP option
        //    [default 127.0.0.1]
        case 's':
          uint8_t addr[4];
          char *rest1, rest2, rest3, rest4;
          addr[0] = strtol(optarg, &rest1, 10); 
          addr[1] = strtol(rest1, &rest2, 10);
          addr[2] = strtol(rest2, &rest3, 10);
          addr[3] = strtol(rest3, &rest4, 10);
          msg->siaddr = (uint32_t) addr;
          break;
        // r: specify the requested IP DHCP option
        //    [default [127.0.0.2]
        case 'r':
          // I think this is for giaddr?
          uint8_t addr[4];
          char *rest1, rest2, rest3, rest4;
          addr[0] = strtol(optarg, &rest1, 10); 
          addr[1] = strtol(rest1, &rest2, 10);
          addr[2] = strtol(rest2, &rest3, 10);
          addr[3] = strtol(rest3, &rest4, 10);
          msg->giaddr = (uint32_t) addr; 
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
