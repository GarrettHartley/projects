var fs = require('fs')
p = require('path')
var dir = process.argv[2]
//console.log('path '+dir)
var extension = '.'+process.argv[3]
//console.log("extension: "+extension)

var total = 0;
//console.log(process.argv)
fs.readdir(dir,function(err,fileNames){
//console.log(fileNames.toString())
if(err){
throw err;
}	
for(var i = 0; i < fileNames.length;i++){
	var fileName = fileNames[i];
		if(p.extname(fileName) === extension){
//		total+=1
		console.log(fileName)
		}	

	}	
//console.log(total)




});
