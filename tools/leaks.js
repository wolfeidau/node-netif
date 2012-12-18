var netif = require('../build/Debug/netif');


for (var i = 0; i < Number.MAX_VALUE; i++){
  process.nextTick(foo);
}

function foo() {
  netif.getIFMacAddress('en1');
}
