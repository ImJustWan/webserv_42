import sys
import os
import cgi
import cgitb
cgitb.enable()

def sum_ascii(string):
	total = 0
	for char in string:
		total += ord(char)
	return total

def calculate_compatibility(name1, name2):
	name1 = name1.lower()
	name2 = name2.lower()
	
	name1count = sum_ascii(name1)
	name2count = sum_ascii(name2)

	ascii_diff = abs(name1count - name2count)

	compatibility_score = 100 - (ascii_diff % 100)
	
	if {'marie', 'tiffany'} == {name1, name2}:
		compatibility_score = 1000
	
	return compatibility_score

def main():

	print("HTTP/1.1 200 OK")
	print("Content-type: text/html\n")

	form = cgi.FieldStorage()
	name1 = form.getvalue("nameOne", "")
	name2 = form.getvalue("nameTwo", "")

	if name1 == "" or name2 == "":
		print("Please provide both names.")
		return 

	compatibility = calculate_compatibility(name1, name2)

	print("<!DOCTYPE html>")
	print("<html lang=\"en\">")
	print("<head>")
	print("<meta charset=\"UTF-8\">")
	print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
	print("<title>LOVE Test</title>")
	print("<link href=\"/forms/love_form/style.css\" rel=\"stylesheet\">")
	print("<link rel=\"icon\" type=\"image/x-icon\" href=\"/forms/love_form/favicon.ico\">")
	print("<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">")
	print("<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>")
	print("<link href=\"https://fonts.googleapis.com/css2?family=Roboto+Mono:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;1,100;1,200;1,300;1,400;1,500;1,600;1,700&display=swap\" rel=\"stylesheet\">")
	print("</head>")
	print("<div class=\"header_wrapper\">")
	print("<header>")
	print("<h1><strong><a href=\"../../index.html\">Webserv</a></strong></h1>")
	print("</header>")
	print("<nav>")
	print("<ul>")
	print ("<a href=\"../../index.html\">Home</a>")
	print ("<a href=\"/forms/love_form/\">Love test</a>")
	print ("<a href=\"/forms/upload_form/index.html\">Upload form</a>")
	print ("<a href=\"/forms/kill_form/index.html\">Delete form</a>")
	print("</ul>")
	print("</nav>")
	print("</div>")
	print("<main>")
	print("<section class=\"window\">")
	print("<h3>LOVE test version 2.09-3~dellulu</h3>")
	print("<section class=\"gnu_wrapper\">")
	print("<section class=\"gnu\">")
	print("<h1>Compatibility test</h1>")
	print("<h2>Because a web server is about connexion</h2><br>")

	if compatibility <= 33:
		print("<p>Oh oh! (╯°□°)╯︵ ┻━┻<br>")
		print("Looks like your love is doomed...<br><br>")
		print(f"Compatibility between <strong style=\"color: #ffffff\">{name1}</strong> and <strong style=\"color: #ffffff\">{name2}</strong>: {compatibility}%</p>")
	elif 33 < compatibility < 66:
		print("<p>¯\_(ツ)_/¯ okayyyy ¯\_(ツ)_/¯<br>")
		print("This could work if you try <br><br>")
		print(f"Compatibility between <strong style=\"color: #ffffff\">{name1}</strong> and <strong style=\"color: #ffffff\">{name2}</strong>: {compatibility}%</p>")
	else:
		print("<p> <3 Love is in the air <3 <br>")
		print("You can already book the wedding venue!<br><br>")
		print(f"Compatibility between <strong style=\"color: #ffffff\">{name1}</strong> and <strong style=\"color: #ffffff\">{name2}</strong>: {compatibility}%</p>")

	print("<section class=\"cta\">")
	print("<br>")
	print("<a href=\"/forms/love_form/\">")
	print("[Back to Love Test]")
	print("</a>")
	print("<br>")
	print("</section>")
	print("</section>")
	print("</section>")
	print("<p>The Webserv Corporation© hereby disclaims any liability or accountability pertaining to the outcomes, be they favorable or adverse, of relationships in any form.</p>")
	print("</section>")
	print("</main>")
	print("<footer>")
	print("<div>")
	print("<p>Webserv ltd.<br>")
	print("+33 01 23 45 67 89<br>")
	print("tgibier@student.42.fr<br>")
	print("mrony@student.42.fr<br>")
	print("92 boulevard Bessieres<br>")
	print("75017 Paris<br>")
	print("France</p>")
	print("</div>")
	print("<div>")
	print("<p>Sitemap <br>")
	print("<br><p>")
	print("<a href=\"index.html\">Get</a><br>")
	print("<a href=\"index.html\">Post</a><br>")
	print("<a href=\"index.html\">Upload</a><br></p>")
	print("</div>")
	print("<div>")
	print("<p>Social medias:<br>")
	print("<br>")
	print("<a href=\"https://facebook.com\" target=\"_blank\"><img src=\"/images/svg/icons8-facebook.svg\" class=\"socials\" alt=\"facebook\"></a>")
	print("<a href=\"https://instagram.com\" target=\"_blank\"><img src=\"/images/svg/icons8-instagram.svg\" class=\"socials\" alt=\"instagram\"></a>")
	print("<a href=\"https://twitter.com\" target=\"_blank\"><img src=\"/images/svg/icons8-twitter.svg\" class=\"socials\" alt=\"twitter\"></a>")
	print("<a href=\"https://tiktok.com\" target=\"_blank\"><img src=\"/images/svg/icons8-tiktok.svg\" class=\"socials\" alt=\"tiktok\"></a><br>")
	print("<a href=\"https://reddit.com\" target=\"_blank\"><img src=\"/images/svg/icons8-reddit.svg\" class=\"socials\" alt=\"reddit\"></a>")
	print("<a href=\"/form/index.html\" target=\"_blank\"><img src=\"/images/svg/icons8-tinder.svg\" class=\"tinder\" alt=\"tinder\"></a>")
	print("<a href=\"https://youtube.com\" target=\"_blank\"><img src=\"/images/svg/icons8-youtube-logo.svg\" class=\"socials\" alt=\"youtube\"></a>")
	print("<a href=\"https://pinterest.com\" target=\"_blank\"><img src=\"/images/svg/icons8-pinterest.svg\" class=\"socials\" alt=\"pinterest\"></a>")
	print("</div>")
	print("<div><p>Privacy policy</p></div>")
	print("<div><p>Legal notices</p></div>")
	print("<div><p>CGU</p></div>")
	print("</footer>")
	print("</body>")
	print("</html>")

if __name__ == "__main__":
	main()
