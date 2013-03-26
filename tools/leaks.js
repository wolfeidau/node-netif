var netif = require('../build/Debug/netif');

var i = 0;

for (; i < 1000; i++) {
  netif.getMacAddress('en0');
}

console.log("ran " + i + " times.");
