<?php

// Print HTML content
echo "<!DOCTYPE html>\n";
echo "<html>\n";
echo "<head>\n";
echo "<title>Hello CGI</title>\n";
echo "<style>\n";
echo "body {\n";
echo "    font-family: Arial, sans-serif;\n";
echo "    display: flex; /* Use flexbox layout */\n";
echo "    justify-content: center; /* Center horizontally */\n";
echo "    align-items: center; /* Center vertically */\n";
echo "    height: 100vh; /* Set full viewport height */\n";
echo "    margin: 0; /* Remove default margin */\n";
echo "}\n";
echo ".message {\n";
echo "    font-size: 100px;\n";
echo "    color: #333;\n";
echo "}\n";
echo "</style>\n";
echo "</head>\n";
echo "<body>\n";
echo "<div class=\"message\">Hello CGI !</div>\n";
echo "</body>\n";
echo "</html>\n";

?>
