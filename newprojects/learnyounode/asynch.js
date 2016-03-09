var fs = require('fs')
var content;
//console.log(process.argv)
fs.readFile(process.argv[2],function(err,data){
	if(err){
		throw err;
	}
	content = data.toString().split("\n").length - 1;
	console.log(content);        

})
