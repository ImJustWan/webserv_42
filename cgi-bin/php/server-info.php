#!/usr/bin/php-cgi
<?php
echo "HTTP/1.1 200 OK\n";
echo "Content-Type: text/html\n";
echo "Content-Length: 321\n\n";

echo "<!DOCTYPE html>";
echo "<html>";
echo "<head>";
echo "<title>Server Information</title>";
echo "</head>";
echo "<body>";
echo "<h1>Server Information</h1>";
echo "<ul>";
echo "<li><strong>Server Name:</strong> " . $_SERVER['SERVER_NAME'] . "</li>";
echo "<li><strong>Server Software:</strong> " . $_SERVER['SERVER_SOFTWARE'] . "</li>";
echo "<li><strong>Server Protocol:</strong> " . $_SERVER['SERVER_PROTOCOL'] . "</li>";
echo "<li><strong>Server Port:</strong> " . $_SERVER['SERVER_PORT'] . "</li>";
echo "</ul>";
echo "</body>";
echo "</html>";
?>
