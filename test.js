var addon = require('./build/x64/Debug/node-bb');
var dc = addon.DriverControlWrap();
//var proc = addon.ProcessWrap();
console.log('Current directory: ' + process.cwd());
//var status = proc.Attach(process.pid);
var status = dc.EnsureLoaded();
var buffer = new Buffer(0x1000);
status = dc.ReadMem(3512, 0x7ff670cc0000, buffer);
console.log('buffer: ' + buffer.toString())
