#! /usr/bin/python3

import cgi
import sys
import os
from http import cookies

cookie = cookies.BaseCookie()
if 'HTTP_COOKIE' in os.environ:
    cookie.load(os.environ["HTTP_COOKIE"])

cookies = False
if "name" in cookie:
    #cookie["name"]["expires"] = "Mon, 15 Jul 2013 07:42 42 GMT"
    cookie["name"]["max-age"] = -1
    cookie["name"]["samesite"] = "Strict"
    cookies = True
if "good" in cookie:
    #cookie["good"]["expires"] = "Mon, 15 Jul 2013 07:42 42 GMT"
    cookie["good"]["max-age"] = -1
    cookie["good"]["samesite"] = "Strict"
    cookies = True
if "total" in cookie:
    #cookie["total"]["expires"] = "Mon, 15 Jul 2013 07:42 42 GMT"
    cookie["total"]["max-age"] = -1
    cookie["total"]["samesite"] = "Strict"
    cookies = True

content = "<!DOCTYPE html>"
content += "<html><head><title>The 42 quizz to the Galaxy</title><meta charset=\"UTF-8\">"
content += "<link rel=\"icon\" type=\"image/png\" href=\"/favicon-32x32.png\" />"
content += "<link href=\"/css/cesar.css\" rel=\"stylesheet\">"
content += "<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\"><link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>"
content += "<link href=\"https://fonts.googleapis.com/css2?family=Dancing+Script&display=swap\" rel=\"stylesheet\"></head>"
content += "<body><header><h1>The 42 quizz to the Galaxy</h1></header>"
content += "<nav id =\"menu\"><a href=\"/html/page/index2.html\">Index</a>"
content += "<a href=\"/php/cesar_get.php\">Ave Cesar (get)</a>"
content += "<a href=\"/php/cesar_post.php\">Ave Cesar (post)</a><a href=\"/python/quizz.py\">The 42 Quizz</a>"
content += "<a href=\"/php/whoswho.php\">Who's who</a>"
content += "<a href=\"/html/kitty/kitty.html\">Kitty</a></nav>"
content += "<section id=\"main\"><p> Thank you for your visit.</p><p>We hope to see you soon.</p></section>"
content += "<footer><p><span class=\"sign\"><b>WebServ</b> by pjay and rertzer.</span>"
content += "<span class=\"devise\">Parce que tu le codes bien.</span></p></footer>"
content += "</body>"

if cookies:
    print(cookie, end="\r\n")
print("Content-Type: text/html", end="\r\n")
print("", end="\r\n")
print(content)
