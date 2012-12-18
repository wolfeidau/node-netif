var netif = require('../build/Debug/netif.node');

exports['awesome'] = {
  setUp: function(done) {
    // setup here
    done();
  },
  'no args': function(test) {
    test.expect(1);
    // tests here
    test.ok(netif.getIFMacAddress('eth0'), "should not error out");
    test.done();
  }
};
