#!/usr/bin/env python3
import cgi, cgitb

form = cgi.FieldStorage()

username = form["username"].value
emailaddress = form["emailaddress"].value

print("<html>")
print("<head>")
print("<title> MY FIRST CGI FILE </title>")
print("</head>")
print("<body>")
print("<h3> This is HTML's Body Section </h3>")
print(username)
print(emailaddress)
print("</body>")
print("</html>")
