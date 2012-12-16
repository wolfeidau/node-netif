#include <node.h>

using namespace v8;

#include <sys/socket.h>
#include <net/if.h>

Handle<Value> GetIFMacAddress(const Arguments& args) {
  HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
        return scope.Close(Undefined());
    }

    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    strcpy(s.ifr_name, "en0");

    if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
        int i;
        for (i = 0; i < 6; ++i) {
            printf(" %02x", (unsigned char) s.ifr_addr.sa_data[i]);
        }
        puts("\n");
    }

}

void Init(Handle<Object> target) {
  target->Set(String::NewSymbol("getIFMacAddress"),
      FunctionTemplate::New(Add)->GetFunction());
}

NODE_MODULE(netif, Init)