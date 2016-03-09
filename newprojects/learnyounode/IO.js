var fs = require('fs')
var buffer = new Buffer(fs.readFileSync(process.argv[2]));

console.log(buffer.toString().split("\n").length - 1)
