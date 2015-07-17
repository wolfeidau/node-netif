#if !defined(_WIN32)
#include <sys/socket.h>
#include <sys/types.h>
#endif


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
#endif

#if defined(_WIN32)
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdlib.h>
#endif

#include <node.h>
#include <nan.h>
#include <string.h>

#include "netif.h"

using namespace v8;

NAN_METHOD(GetMacAddress) {
  NanScope();

  if (args.Length() < 1) {
    NanThrowTypeError("Wrong number of arguments");
  }

  if (!args[0]->IsString()) {
    NanThrowTypeError("First argument must be a string");
  }

  NanUtf8String device(args[0]);
  char formattedMacAddress[mac_addr_len];
  unsigned char macAddress[ether_addr_len];

#if defined(__APPLE_CC__) || defined(__APPLE__)
  char *messageBuffer = NULL;
  int mgmtInfoBase[ether_addr_len];
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

    NanThrowTypeError("Error opening interface");
    NanReturnUndefined();

  } else {

    // Get the size of the data available (store in len)
    if (sysctl(mgmtInfoBase, ether_addr_len, NULL, &length, NULL, 0) < 0) {

      NanThrowTypeError("sysctl mgmtInfoBase failure");
      NanReturnUndefined();
    } else {

      // Alloc memory based on above call
      if ((messageBuffer= (char *)malloc(length)) == NULL) {

        NanThrowTypeError("message buffer allocation failure");
        NanReturnUndefined();
      } else {

        // Get system information, store in buffer
        if (sysctl(mgmtInfoBase, ether_addr_len, messageBuffer, &length, NULL, 0) < 0){

          // Release the buffer memory
          free(messageBuffer);

          NanThrowTypeError("sysctl msgBuffer failure");
          NanReturnUndefined();
        }
      }
    }
  }

  // Map msgbuffer to interface message structure
  interfaceMsgStruct = (struct if_msghdr *) messageBuffer;

  // Map to link-level socket structure
  socketStruct = (struct sockaddr_dl *) (interfaceMsgStruct + 1);

  // Copy link layer address data in socket structure to an array
  memcpy(&macAddress, socketStruct->sdl_data + socketStruct->sdl_nlen, ether_addr_len);

  // Release the buffer memory
  free(messageBuffer);

  snprintf(formattedMacAddress, mac_addr_len, "%02X:%02X:%02X:%02X:%02X:%02X",
      macAddress[0], macAddress[1], macAddress[2],
      macAddress[3], macAddress[4], macAddress[5]);

#endif

#if defined(linux)

  struct ifreq s;

  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  // copy in the ethernet interface name
  strncpy(s.ifr_name, *device, IFNAMSIZ);

  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {

    // Copy link layer address data in socket structure to an array
    memcpy(&macAddress, &s.ifr_addr.sa_data, ether_addr_len);

    snprintf(formattedMacAddress, mac_addr_len, "%02X:%02X:%02X:%02X:%02X:%02X",
        macAddress[0], macAddress[1], macAddress[2],
        macAddress[3], macAddress[4], macAddress[5]);

  } else {

    // TODO lookup the ERR for this and return it to the user for example -1 EMFILE (Too many open files)
    NanThrowTypeError("Error opening interface");
    NanReturnUndefined();
  }

  // Close the file descriptor
  close(fd);

#endif

#if defined(__sun)

  struct ifreq s;

  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  // copy in the ethernet interface name
  strncpy(s.ifr_name, *device, IFNAMSIZ);

  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {

    // Copy link layer address data in socket structure to an array
    memcpy(&macAddress, &s.ifr_addr.sa_data, ether_addr_len);

    snprintf(formattedMacAddress, mac_addr_len, "%02X:%02X:%02X:%02X:%02X:%02X",
        macAddress[0], macAddress[1], macAddress[2],
        macAddress[3], macAddress[4], macAddress[5]);

  } else {

    // TODO lookup the ERR for this and return it to the user for example -1 EMFILE (Too many open files)
    NanThrowTypeError("error opening interface");
    NanReturnUndefined();
  }

  // Close the file descriptor
  close(fd);

#endif

#if defined(_WIN32)

  IP_ADAPTER_ADDRESSES adapterAddresses[16], *adapterAddress;
  ULONG size = sizeof(adapterAddresses);
  ULONG flags = GAA_FLAG_SKIP_UNICAST | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST;

  if (ERROR_SUCCESS == GetAdaptersAddresses(AF_UNSPEC, flags, NULL, &(adapterAddresses[0]), &size)) {
    adapterAddress = &adapterAddresses[0];

    while (NULL != adapterAddress) {
      if (0 != strcmp(adapterAddress->AdapterName, *device)) {
        break;
      }
      adapterAddress = adapterAddress->Next;
    }

    if (NULL == adapterAddress) {
      NanThrowTypeError("unknown interface");
      NanReturnUndefined();
    }
    if (adapterAddress->PhysicalAddressLength != ether_addr_len) {
      NanThrowTypeError("address length mismatch");
      NanReturnUndefined();
    }

    // Copy link layer address data in socket structure to an array
    memcpy(&macAddress, adapterAddress->PhysicalAddress, ether_addr_len);

    sprintf(formattedMacAddress, "%02X:%02X:%02X:%02X:%02X:%02X",
        macAddress[0], macAddress[1], macAddress[2],
        macAddress[3], macAddress[4], macAddress[5]);

  } else {

    // TODO lookup the ERR for this and return it to the user
    NanThrowTypeError("error obtaining adapter addresses");
    NanReturnUndefined();
  }

#endif

  // Copy mac address to a v8 string
  NanReturnValue(NanNew<String>(formattedMacAddress));
}

void Init(Handle<Object> target) {
  target->Set(NanNew<String>("getMacAddress"),
      NanNew<FunctionTemplate>(GetMacAddress)->GetFunction());
}

NODE_MODULE(netif, Init)
