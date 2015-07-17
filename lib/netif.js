/*
 * netif
 * https://github.com/wolfeidau/netif
 *
 * Copyright (c) 2012 Mark Wolfe
 * Licensed under the MIT license.
 */
var binary = require('node-pre-gyp');
var os = require('os');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname, '../package.json')));
var binding = require(binding_path);

function getDefaultInterfaceName() {
  var interfaces = os.networkInterfaces();
  for (var name in interfaces) {
    if (!interfaces[name].length) continue;
    if (!(interfaces[name][0].internal)) return name;
  }
}

/*
 * Locate the mac address for a given interface name.
 *
 * @param interfaceName - The name for the interface for example eth0 in linux.
 */
module.exports.getMacAddress = function(interfaceName) {
  return binding.getMacAddress(interfaceName || getDefaultInterfaceName());
}
