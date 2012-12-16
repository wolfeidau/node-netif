#include <sys/socket.h>
#include <sys/sysctl.h>

// ether stuff for OSX
#ifdef __APPLE__
#include <net/if.h>
#include <net/if_dl.h>
#endif 

#ifdef __linux__
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#endif 

#include <net/ethernet.h>

#include <string.h>

#include <stdlib.h>
#include <stdio.h>

int main( int argc, const char* argv[] )
{
  if (argc != 2){
    printf("Usage: testingif [ethernet interface]\n");
    exit(1);
  }

  char *messageBuffer = NULL;
  const char *interface = argv[1];
  unsigned char       macAddress[ETHER_ADDR_LEN];

#ifdef __APPLE__
  int mgmtInfoBase[ETHER_ADDR_LEN];
  size_t              length;
  struct if_msghdr    *interfaceMsgStruct;
  struct sockaddr_dl  *socketStruct;

  // Setup the management Information Base (mib)
  mgmtInfoBase[0] = CTL_NET;        // Request network subsystem
  mgmtInfoBase[1] = AF_ROUTE;       // Routing table info
  mgmtInfoBase[2] = 0;
  mgmtInfoBase[3] = AF_LINK;        // Request link layer information
  mgmtInfoBase[4] = NET_RT_IFLIST;  // Request all configured interfaces

  // With all configured interfaces requested, get handle index
  if ((mgmtInfoBase[5] = if_nametoindex(interface)) == 0) { 
    printf("if_nametoindex failure\n");
    exit(1);
  } else {
    // Get the size of the data available (store in len)
    if (sysctl(mgmtInfoBase, ETHER_ADDR_LEN, NULL, &length, NULL, 0) < 0) {
      printf("sysctl mgmtInfoBase failurei\n");
      exit(1);
    } else {
      // Alloc memory based on above call
      if ((messageBuffer= malloc(length)) == NULL) {
        // error occured.
        printf("message buffer allocation failure\n");
        exit(1);
      } else {
        // Get system information, store in buffer
        if (sysctl(mgmtInfoBase, ETHER_ADDR_LEN, messageBuffer, &length, NULL, 0) < 0){
          printf("sysctl msgBuffer failure\n");
          free(messageBuffer);
          exit(1);
        }
      }
    }
  }


  // Map msgbuffer to interface message structure
  interfaceMsgStruct = (struct if_msghdr *) messageBuffer;

  // Map to link-level socket structure
  socketStruct = (struct sockaddr_dl *) (interfaceMsgStruct + 1);

  // Copy link layer address data in socket structure to an array
  memcpy(&macAddress, socketStruct->sdl_data + socketStruct->sdl_nlen, ETHER_ADDR_LEN);

#endif

#ifdef __linux__

  struct ifreq s;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  // copy in the ethernet interface name
  strcpy(s.ifr_name, interface);
  
  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {

    // Copy link layer address data in a socket structure to an array
    memcpy(&macAddress, &s.ifr_addr.sa_data, ETHER_ADDR_LEN);

  } else {
    printf("error opening interface\n");
    exit(1);
  }

#endif

  printf( "\nPrinting mac addresse for %s\n\n", interface);


  printf("%02X:%02X:%02X:%02X:%02X:%02X\n", 
      macAddress[0], macAddress[1], macAddress[2], 
      macAddress[3], macAddress[4], macAddress[5]);

  // Release the buffer memory
  free(messageBuffer);
}
