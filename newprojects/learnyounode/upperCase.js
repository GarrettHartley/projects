var http = require('http')
var map = require('through2-map')

var server = http.createServer(function(req,res){
	if(req.method != 'POST')
		return res.end('send me a POST\n')

	req.pipe(map(function(part){
		return part.toString().toUpperCase()
})).pipe(res)
})

server.listen(Number(process.argv[2]))

