#include <sys/socket.h>
#include <sys/types.h>


#if defined(__APPLE_CC__) || defined(__APPLE__)
#include <sys/sysctl.h>

#undef EV_ERROR

#include <net/if.h>
#include <net/if_dl.h>
#include <net/ethernet.h>
#endif

#if defined(linux)
#include <sys/sysctl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdlib.h>
#include <net/ethernet.h>
#include <unistd.h>
#endif

#if defined(__sun)
#include <stdlib.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <unistd.h>
#include <stropts.h>
#define	ETHER_ADDR_LEN		6
#endif

#include <node.h>

using namespace v8;


Handle<Value> GetMacAddress(const Arguments& args) {

  HandleScope scope;

  if (args.Length() < 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }

  String::Utf8Value device(args[0]->ToString());

  //const char *interface = (char *) *device;
  unsigned char       macAddress[ETHER_ADDR_LEN];

#if defined(__APPLE_CC__) || defined(__APPLE__)
  char *messageBuffer = NULL;
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
  if ((mgmtInfoBase[5] = if_nametoindex((char *) *device)) == 0) {

    ThrowException(Exception::TypeError(String::New("Error opening interface")));
    return scope.Close(Undefined());

  } else {

    // Get the size of the data available (store in len)
    if (sysctl(mgmtInfoBase, ETHER_ADDR_LEN, NULL, &length, NULL, 0) < 0) {

      ThrowException(Exception::TypeError(String::New("sysctl mgmtInfoBase failure")));
      return scope.Close(Undefined());
    } else {

      // Alloc memory based on above call
      if ((messageBuffer= (char *)malloc(length)) == NULL) {

        ThrowException(Exception::TypeError(String::New("message buffer allocation failure")));
        return scope.Close(Undefined());
      } else {

        // Get system information, store in buffer
        if (sysctl(mgmtInfoBase, ETHER_ADDR_LEN, messageBuffer, &length, NULL, 0) < 0){

          // Release the buffer memory
          free(messageBuffer);

          ThrowException(Exception::TypeError(String::New("sysctl msgBuffer failure")));
          return scope.Close(Undefined());
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

  // Release the buffer memory
  free(messageBuffer);

#endif

#if defined(linux)

  struct ifreq s;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  // copy in the ethernet interface name
  strcpy(s.ifr_name, (char *) *device);

  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {

    // Copy link layer address data in a socket structure to an array
    memcpy(&macAddress, &s.ifr_addr.sa_data, ETHER_ADDR_LEN);

  } else {

    // TODO lookup the ERR for this and return it to the user for example -1 EMFILE (Too many open files)
    ThrowException(Exception::TypeError(String::New("Error opening interface")));
    return scope.Close(Undefined());
  }

  // Close the file descriptor
  close(fd);

#endif

#if defined(__sun)

  struct ifreq s;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  // copy in the ethernet interface name
  strcpy(s.ifr_name, (char *) *device);

  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {

    // Copy link layer address data in a socket structure to an array
    memcpy(&macAddress, &s.ifr_addr.sa_data, ETHER_ADDR_LEN);

  } else {

    // TODO lookup the ERR for this and return it to the user for example -1 EMFILE (Too many open files)
    ThrowException(Exception::TypeError(String::New("error opening interface")));
    return scope.Close(Undefined());
  }

  // Close the file descriptor
  close(fd);

#endif

  char       formattedMacAddress[17];

  sprintf(formattedMacAddress, "%02X:%02X:%02X:%02X:%02X:%02X",
      macAddress[0], macAddress[1], macAddress[2],
      macAddress[3], macAddress[4], macAddress[5]);

  // Copy mac address to a v8 string
  return scope.Close(String::New(formattedMacAddress));
}

void Init(Handle<Object> target) {
  target->Set(String::NewSymbol("getMacAddress"),
      FunctionTemplate::New(GetMacAddress)->GetFunction());
}

NODE_MODULE(netif, Init)
