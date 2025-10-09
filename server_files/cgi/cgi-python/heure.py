#!/usr/bin/env python3
import datetime

# Étape 1: Afficher l'en-tête HTTP obligatoire
# print("Content-type: text/html\n")

# Étape 2: Générer le contenu HTML
now = datetime.datetime.now()
print("<html>")
print("<head><title>CGI Python</title></head>")
print("<body>")
print("<h1>Heure Actuelle du Serveur (CGI Python)</h1>")
print(f"<p>Il est actuellement : <b>{now.strftime('%H:%M:%S')}</b></p>")
print("</body>")
print("</html>")
