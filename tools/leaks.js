var netif = require('../build/Debug/netif');

var i = 0;

for (; i < 1000000; i++) {
  netif.getIFMacAddress('en1');
}

console.log("ran " + i + " times.");
