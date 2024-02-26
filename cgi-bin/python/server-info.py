#!/usr/bin/env python

import os

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
request = ""
request += "<HTML>"
request += "<HEAD><TITLE>About this Server</TITLE></HEAD>"
request += "<BODY><H1>About this Server</H1>"
request += "<HR><PRE>"
request += "Server Name:      " + os.environ.get('SERVER_NAME', 'Unknown') + "<BR>"
request += "Running on Port:  " + os.environ.get('SERVER_PORT', 'Unknown') + "<BR>"
request += "Server Software:  " + os.environ.get('SERVER_SOFTWARE', 'Unknown') + "<BR>"
request += "Server Protocol:  " + os.environ.get('SERVER_PROTOCOL', 'Unknown') + "<BR>"
request += "CGI Revision:     " + os.environ.get('GATEWAY_INTERFACE', 'Unknown') + "<BR>"
request += "Redirect status   " + os.environ.get('REDIRECT_STATUS', 'Unknown') + "<BR>"
request += "Request Method    " + os.environ.get('REQUEST_METHOD', 'Unknown') + "<BR>"
request += "Script Name       " + os.environ.get('SCRIPT_NAME', 'Unknown') + "<BR>"
request += "Path Info         " + os.environ.get('PATH_INFO', 'Unknown') + "<BR>"
request += "Query String      " + os.environ.get('QUERY_STRING', 'Unknown') + "<BR>"
request += "root              " + os.environ.get('DOCUMENT_ROOT', 'Unknown') + "<BR>"
request += "<HR></PRE>"
request += "</BODY></HTML>"
print("Content-Length: ", len(request), "\n")
print(request)
