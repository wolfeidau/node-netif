var netif = require('../build/Release/netif');

var i = 0;

for (; i < 100; i++) {
  netif.getMacAddress('eth0');
}

console.log("ran " + i + " times.");
