var netif = require('../lib/netif');

var os = require('os');
var interfaces;
var interfaceNames;

exports['netif'] = {
  setUp: function (done) {
    interfaces = os.networkInterfaces();
    interfaceNames = Object.keys(interfaces);
    console.log('\u2699 Interfaces: ' + interfaceNames.join(', '));
    done();
  },
  'should look up interface given an ethernet interface name': function (test) {
    test.expect((2 * interfaceNames.length) + 1);

    test.ok(interfaceNames.length > 0);

    interfaceNames.forEach(function (name) {
      var mac = netif.getMacAddress(name);
      test.ok(mac, "should return *something* for " + name);
      test.ok(/([0-9a-f]{2}:){5}[0-9a-f]{2}/i.test(mac), "should return a MAC address for " + name);
    });
    test.done();
  }
};
