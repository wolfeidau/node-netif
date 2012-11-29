var  netif = require('../build/Release/netif.node'); // require('netif');

exports['awesome'] = {
  setUp: function(done) {
    // setup here
    done();
  },
  'no args': function(test) {
    test.expect(1);
    // tests here
    //test.ok(netif.getIFMacAddress('eth0'), "should not error out");
    test.ok(netif.getIFMacAddress('net0'), "should not error out");
    test.done();
  }
};
