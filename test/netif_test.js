var netif = require('../lib/netif');

exports['netif'] = {
  setUp: function(done) {
    // setup here
    done();
  },
  'should look up interface given an ethernet interface name': function(test) {
    test.expect(1);
    // tests here
    test.ok(netif.getMacAddress('eth0'), "should not error out"); // linux
    //test.ok(netif.getMacAddress('net0'), "should not error out"); // solaris
    //test.ok(netif.getMacAddress('en0'), "should not error out"); // osx
    test.done();
  }
};
