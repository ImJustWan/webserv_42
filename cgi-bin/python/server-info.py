#!/usr/bin/env python

import os

# print("HTTP/1.1 200 OK")
# print("Content-type: text/html\n")
print("<HTML>")
print("<HEAD><TITLE>About this Server</TITLE></HEAD>")
print("<BODY><H1>About this Server</H1>")
print("<HR><PRE>")
print("Server Name:      ", os.environ.get('SERVER_NAME', 'Unknown'), "<BR>")
print("Running on Port:  ", os.environ.get('SERVER_PORT', 'Unknown'), "<BR>")
print("Server Software:  ", os.environ.get('SERVER_SOFTWARE', 'Unknown'), "<BR>")
print("Server Protocol:  ", os.environ.get('SERVER_PROTOCOL', 'Unknown'), "<BR>")
print("CGI Revision:     ", os.environ.get('GATEWAY_INTERFACE', 'Unknown'), "<BR>")
print("<HR></PRE>")
print("</BODY></HTML>")