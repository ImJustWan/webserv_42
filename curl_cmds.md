# List of Curl commands to test Webserv from the terminal

## Test Client_max_body_size
Tests if the server blocks requests that exceed the client body sze limit - set in configuration file;

+ For instance: set the body limit at 1000 bites and test
`curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorter or longer than body limit" localhost:8080`

+ For instance: set the body limit at 1000 bites and test
`curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - BODY IS HERE write something shorter or longer than body limit - " localhost:8080`

## Test GET request on a server/location that accepts GET requests and on another server/location that doesn't
+ Server/location that accepts GET requests
`curl -X GET "Content-Type: plain/text" --data "empty" localhost:8080/`
+ Server/location that doesn't accept GET requests
`curl -X GET "Content-Type: plain/text" --data "empty" 127.0.0.8:8080/`

## Test POST request
+ Same as previous request for Client_Max_Body_size
`curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorter or longer than body limit" localhost:8080`

+ Sending chunked request to Webslayyy
`curl localhost:8080 -H "Transfer-Encoding: chunked" -d @www/Way_of_Kings.jpg`

## Tests DELETE request on a server/location that accepts DELETE requests and on another server/location that doesn't
+ Server/location that accepts DELETE requests
`curl -X DELETE -H "Content-Type: plain/text" localhost:8080/images/png/delete/2.png`

+ Server/location that doesn't accepts DELETE requests
`curl -X DELETE -H "Content-Type: plain/text" localhost:8080`

## Test Unknown request
+ You can replace "PROUT" by anything that is not "GET" "POST" or "DELETE"
`curl -X PROUT "Content-Type: plain/text" localhost:8080`

## Test CGI
+ Test infinite loop to check if our server returns a timeout error
`curl -X GET "Content-Type: plain/text" --data "" localhost:8080/cgi-bin/python/loop.py`

+ Test GET CGI - python
`curl -X GET "Content-Type: plain/text" localhost:8080/cgi-bin/python/server-info.py`
+ Test GET CGI - perl
`curl -X GET "Content-Type: plain/text" localhost:8080/cgi-bin/python/server-info.pl`
+ Test GET CGI - php
`curl -X GET "Content-Type: plain/text" localhost:8080/cgi-bin/python/server-info.php`

+ Test POST CGI
`curl -X POST "Content-Type: plain/text" --data "nameOne=Marie&nameTwo=Tiffany" localhost:8080/cgi-bin/python/compatibility.py`

+ You can open several terminals to lauch a loop and another request at the same time to ensure the server is nonblocking when handling CGI.

## Siege instructions

+ Install Homebrew
`rm -rf $HOME/.brew && git clone --depth=1 https://github.com/Homebrew/brew $HOME/.brew && echo 'export PATH=$HOME/.brew/bin:$PATH' >> $HOME/.zshrc && source $HOME/.zshrc && brew update`

+ Install Siege
`brew install siege`

+ Launch siege on server listening on port 8080 :
`siege http://localhost:8080`
