#include <stdlib.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

int
main (int argc, char **argv)
{
  if (!get_args (argc, argv))
    {
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

static bool
get_args (int argc, char **argv)
{
  int option;

  while ((option = getopt (argc, argv, "x:t:c:m:s:r:p")) != -1)
    {
      switch (option)
        {
        case 'p':
          //nothing for now
          break;
        default:
          return false;
        }
    }
  return true;
}
