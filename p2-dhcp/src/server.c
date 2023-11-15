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
          close (s); //timed out
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
      if (type == DHCPDISCOVER) {
        buf.options[6] = DHCPOFFER;
      } else if (type == DHCPREQUEST) {
        buf.options[6] = DHCPACK;
      }
      buf.op = BOOTREPLY;
      if (sendto (s, &buf, sizeof (buf), 0, (struct sockaddr *)&client, addrlen) == -1)
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
