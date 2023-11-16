#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include <netdb.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

static bool get_args (int, char **);

static bool debug = false;

int
main (int argc, char **argv)
{
  // printf("Starting\n");
  get_args (argc, argv);
  int s;
  struct sockaddr_in client, server;
  // char buf[32];
  msg_t buf;
  if ((s = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      perror ("Socket creation failed\n");
      exit (EXIT_FAILURE);
    }
  struct timeval timeout;
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  // int opt = 1;
  if (setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                  sizeof (struct timeval))
      < 0)
    {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }
  // if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof (opt)) <
  // 0)
  //   {
  //     perror ("setsockopt");
  //     exit (EXIT_FAILURE);
  //   }
  // if (setsockopt (s, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof (opt)) <
  // 0)
  //   {
  //     perror ("setsockopt");
  //     exit (EXIT_FAILURE);
  //   }
  memset (&server, 0, sizeof (server));
  server.sin_family = AF_INET;
  server.sin_port = htons (strtol (get_port (), NULL, 10));
  uint8_t *adbuf = (uint8_t *)&server.sin_addr.s_addr;
  adbuf[0] = 127; // default server addr
  adbuf[1] = 0;   // default server addr
  adbuf[2] = 0;   // default server addr
  adbuf[3] = 1;
  server.sin_addr.s_addr = server.sin_addr.s_addr;
  if (bind (s, (struct sockaddr *)&server, sizeof (server)) < 0)
    {
      perror ("Bind failed\n");
      printf ("Bind failed %d\n", errno);
      exit (EXIT_FAILURE);
    }
  fflush (stdout);
  // size_t running = -11;
  bool offers[10];
  memset (offers, 0, 10);
  bool acks[10];
  memset (acks, 0, 10);
  while (true)
    {
      // printf("Listening for client\n");
      fflush (stdout);
      memset (&client, 0, sizeof (client));
      socklen_t addrlen = sizeof (client);

      int n = recvfrom (s, &buf, sizeof (msg_t), 0, (struct sockaddr *)&client,
                        &addrlen);
      if (n < 0)
        {
          close (s); // timed out
          return EXIT_SUCCESS;
        }
      else
        {
          printf ("++++++++++++++++\nSERVER RECEIVED:\n");
          fflush (stdout);
          dump_msg (stdout, &buf, sizeof (msg_t));
        }
      // msg_t temp;
      // make_default_msg(&temp);
      int type = buf.options[6];
      if (type == DHCPDISCOVER)
        {
          buf.options[6] = DHCPOFFER;
          uint8_t *temp_yi = (uint8_t *)&buf.yiaddr;
          temp_yi[0] = 10;
          temp_yi[1] = 0;
          temp_yi[2] = 2;
          int off_ind = 0;
          while (off_ind < 10 && offers[off_ind])
            {
              off_ind++;
            }
          if (off_ind == 10)
            {
              buf.options[6] = DHCPNAK;
              memset(temp_yi, 0, 4);
              // break; // SEND NAK
            }
          else
            {
              temp_yi[3] = off_ind + 1;
              offers[off_ind] = true;
            }
        }
      else if (type == DHCPREQUEST)
        {
          buf.options[6] = DHCPACK;
          uint8_t *temp_yi = (uint8_t *)&buf.yiaddr;
          temp_yi[0] = 10; 
          temp_yi[1] = 0;
          temp_yi[2] = 2;
          int opt_i = 7;
          while (opt_i < 312)
            {
              uint8_t code = buf.options[opt_i];
              if (code == 0)
                {
                  opt_i += 1;
                  continue;
                }
              else if (code == 255)
                {
                  break;
                } else if (code == 50) {
                  uint8_t *value = &buf.options[opt_i + 2];
                  uint8_t req = value[3];
                  if (!acks[req-1]) {
                    temp_yi[3] = req;
                    acks[req-1] = true;
                  }
                  break;
                } else {
                  
                  uint8_t len = buf.options[opt_i + 1];
                  opt_i += len + 2;
                }
            }
        }
      buf.op = BOOTREPLY;
      if (sendto (s, &buf, sizeof (buf), 0, (struct sockaddr *)&client,
                  addrlen)
          == -1)
        {
          perror ("Failed to send\n");
          exit (EXIT_FAILURE);
        }
    }
  if (debug)
    fprintf (stderr, "Shutting down\n");

  close (s);
  return EXIT_SUCCESS;
}

static bool
get_args (int argc, char **argv)
{
  int ch = 0;
  while ((ch = getopt (argc, argv, "dh")) != -1)
    {
      switch (ch)
        {
        case 'd':
          debug = true;
          break;
        default:
          return false;
        }
    }
  return true;
}
