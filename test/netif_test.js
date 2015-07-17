var netif = require('../lib/netif');

var os = require('os');

exports['netif'] = {
  setUp: function (done) {
    // setup here
    done();
  },
  'should look up interface given an ethernet interface name': function (test) {
    test.expect(1);
    // tests here
    switch (os.platform()) {
      case 'darwin':
        test.ok(netif.getMacAddress('en0'), "should not error out"); // osx
        break;
      case 'linux':
        if (process.env.TRAVIS) // cause travis is different..
            test.ok(netif.getMacAddress('venet0'), "should not error out"); // linux
	else
            test.ok(netif.getMacAddress('eth0'), "should not error out"); // linux
        break;
      case 'sunos':
        test.ok(netif.getMacAddress('net0'), "should not error out"); // solaris
        break;
      default:
        throw Error('unsupported os for this module.');
    }
    test.done();
  }
};
