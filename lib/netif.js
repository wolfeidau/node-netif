/*
 * netif
 * https://github.com/wolfeidau/netif
 *
 * Copyright (c) 2012 Mark Wolfe
 * Licensed under the MIT license.
 */
var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname, '../package.json')));
var binding = require(binding_path);

/*
 * Locate the mac address for a given interface name.
 *
 * @param interfaceName - The name for the interface for example eth0 in linux.
 */
module.exports.getMacAddress = function(interfaceName) {
  return binding.getMacAddress(interfaceName);
}
