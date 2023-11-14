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
  // variable declaration
  msg_t msg;
  bool p;

  // init msg
  make_default_msg (&msg);

  if (!get_args (argc, argv, &msg, &p))
    {
      return EXIT_FAILURE;
    }
  dump_msg (stdout, &msg, sizeof (msg_t));
  if (p)
    {
      printf ("\n");
      // TODO - Server stuff
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
          break;
        // t: use N as the hardware type (must be named in src/dhcp.h)
        //    [default ETH]
        case 't':
          //===================================================================
          switch (atoi (optarg))
            {
            case IEEE802:
              msg->htype = IEEE802;
              msg->hlen = IEEE802_LEN;
              break;
            case ARCNET:
              msg->htype = ARCNET;
              msg->hlen = ARCNET_LEN;
              break;
            case FRAME_RELAY:
              msg->htype = FRAME_RELAY;
              msg->hlen = FRAME_LEN;
              break;
            case FIBRE:
              msg->htype = FIBRE;
              msg->hlen = FIBRE_LEN;
              break;
            case ATM:
              msg->htype = ATM;
              msg->hlen = ATM_LEN;
              break;
            case ETH: // ETH gets wrapped up as the default val
            default:
              msg->htype = ETH;
              msg->hlen = ETH_LEN;
              break;
            }
          //===================================================================
          break;
        // c: use N as the hardware address (chaddr)
        //    [default 0x010203040506]
        case 'c':;
          if (optarg != NULL)
            {
              for (int i = 0; i < strlen (optarg); i += 2)
                {
                  char temp_str[3];
                  // Copies two characters from optarg into temp_str
                  strncpy (temp_str, optarg + i, 2);
                  // Sets the value of chaddr[i / 2] to the converted value
                  // from temp_str
                  msg->chaddr[i / 2] = (uint8_t)strtol (temp_str, NULL, 16);
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
        case 's':
          if (false)
            {
              msg->options[13] = 54;  // server option id
              msg->options[14] = 4;   // server addr len
              msg->options[15] = 127; // default server addr
              msg->options[16] = 0;   // default server addr
              msg->options[17] = 0;   // default server addr
              msg->options[18] = 1;   // default server addr
            }
          break;
        // r: specify the requested IP DHCP option
        //    [default [127.0.0.2]
        case 'r':
          if (false)
            {
              msg->options[7] = 50;  // request option id
              msg->options[8] = 4;   // request addr len
              msg->options[9] = 127; // default request addr
              msg->options[10] = 0;  // default request addr
              msg->options[11] = 0;  // default request addr
              msg->options[12] = 2;  // default request addr
            }
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
