#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

static bool get_args (int, char **, msg_t *, bool *, bool *);

int
main (int argc, char **argv)
{
  // variable declaration
  msg_t msg;
  bool p, frame;

  // init msg
  make_default_msg (&msg);

  if (!get_args (argc, argv, &msg, &p, &frame))
    {
      return EXIT_FAILURE;
    }
  // frame htype means no server or relay options i guess
  if (frame)
    {
      memset (&msg.options[7], 0, 11);
    }
  dump_msg (stdout, &msg, sizeof (msg_t));
  if (p)
    {
      msg_t buf;
      // printf ("\n");
      int s;
      if ((s = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
        {
          perror ("Failed to socket\n");
          printf ("Failed to socket\n");
          exit (EXIT_FAILURE);
        }
      // struct timeval timeout;
      // timeout.tv_sec = 3;
      // setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, &timeout,
      //             sizeof (struct timeval));
      // connect()
      struct sockaddr_in server;
      memset (&server, 0, sizeof (server));
      server.sin_family = AF_INET;
      server.sin_port = htons (strtol (get_port (), NULL, 10));
      uint8_t *adbuf = (uint8_t *)&server.sin_addr.s_addr;
      adbuf[0] = 127; // default server addr
      adbuf[1] = 0;   // default server addr
      adbuf[2] = 0;   // default server addr
      adbuf[3] = 1;


      if (sendto (s, &msg, sizeof (msg), 0, (const struct sockaddr *)&server,
                  sizeof (server)) 
          == -1)
        {
          perror ("Failed to send\n");
          printf ("Failed to send\n");
          exit (EXIT_FAILURE);
        }
      recvfrom (s, &buf, sizeof (msg_t), 0, (struct sockaddr *)&server,
                        NULL);
      printf ("++++++++++++++++\nCLIENT RECEIVED:\n");
      dump_msg (stdout, &buf, sizeof (msg_t));
      printf ("++++++++++++++++\n");
      msg.options[6] = DHCPREQUEST;
      msg.options[7] = 50;
      msg.options[8] = 4;
      msg.options[9] = 10;
      msg.options[10] = 0;
      msg.options[11] = 2;
      msg.options[12] = ((uint8_t *)&buf.yiaddr)[3];
      msg.options[13] = 54;
      msg.options[14] = 4;
      msg.options[15] = 10;
      msg.options[16] = 0;
      msg.options[17] = 2;
      msg.options[18] = 0;
      
      dump_msg (stdout, &msg, sizeof (msg_t));
      if (sendto (s, &msg, sizeof (msg), 0, (const struct sockaddr *)&server, sizeof (server))  == -1)
        {
          perror ("Failed to send\n");
          exit (EXIT_FAILURE);
        }
      recvfrom (s, &buf, sizeof (msg_t), 0, (struct sockaddr *)&server,NULL);
      printf ("++++++++++++++++\nCLIENT RECEIVED:\n");
      dump_msg (stdout, &buf, sizeof (msg_t));
      printf ("++++++++++++++++\n");
      close (s);
    }
  return EXIT_SUCCESS;
}

static bool
get_args (int argc, char **argv, msg_t *msg, bool *p, bool *frame)
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
              msg->xid = atol (optarg);
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
              *frame = true;
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

              msg->options[7] = 50;   // request option id
              msg->options[8] = 4;    // request addr len
              msg->options[9] = 127;  // default request addr
              msg->options[10] = 0;   // default request addr
              msg->options[11] = 0;   // default request addr
              msg->options[12] = 2;   // default request addr
              msg->options[13] = 54;  // server option id
              msg->options[14] = 4;   // server addr len
              msg->options[15] = 127; // default server addr
              msg->options[16] = 0;   // default server addr
              msg->options[17] = 0;   // default server addr
              msg->options[18] = 1;   // default server addr
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
          if (optarg != NULL)
            {
              char *srest1, *srest2, *srest3, *srest4;
              msg->options[13] = 54; // server option id
              msg->options[14] = 4;  // server addr len
              msg->options[15] = strtol (optarg, &srest1, 10);
              msg->options[16] = strtol (srest1 + 1, &srest2, 10);
              msg->options[17] = strtol (srest2 + 1, &srest3, 10);
              msg->options[18] = strtol (srest3 + 1, &srest4, 10);
            }
          else
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
          if (optarg)
            {
              char *rrest1, *rrest2, *rrest3, *rrest4;
              msg->options[7] = 50; // request option id
              msg->options[8] = 4;  // request addr len
              msg->options[9] = strtol (optarg, &rrest1, 10);
              msg->options[10] = strtol (rrest1 + 1, &rrest2, 10);
              msg->options[11] = strtol (rrest2 + 1, &rrest3, 10);
              msg->options[12] = strtol (rrest3 + 1, &rrest4, 10);
            }
          else
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
          *p = true;
          break;
        default:
          return false;
        }
    }
  return true;
}
