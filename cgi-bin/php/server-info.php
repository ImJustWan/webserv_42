#!/usr/bin/php-cgi
<?php
echo "Content-type: text/html\n\n";

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
echo "<li><strong>Server IP Address:</strong> " . $_SERVER['SERVER_ADDR'] . "</li>";
echo "</ul>";
echo "</body>";
echo "</html>";
?>
