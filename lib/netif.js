/*
 * netif
 * https://github.com/wolfeidau/netif
 *
 * Copyright (c) 2012 Mark Wolfe
 * Licensed under the MIT license.
 */
var bindings = require('bindings')('netif');

/*
 * Locate the mac address for a given interface name.
 *
 * @param interfaceName - The name for the interface for example eth0 in linux.
 */
module.exports.getMacAddress = function(interfaceName) {
  return bindings.getMacAddress(interfaceName);
}
