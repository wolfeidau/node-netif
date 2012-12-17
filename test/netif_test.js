var netif = require('./build/Debug/netif.node');

exports['awesome'] = {
  setUp: function(done) {
    // setup here
    done();
  },
  'no args': function(test) {
    test.expect(1);
    // tests here
    test.equal(netif.awesome(), 'awesome', 'should be awesome.');
    test.done();
  }
};
