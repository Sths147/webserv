#!/bin/bash

# En-tête HTTP obligatoire, suivi d'une ligne vide
# echo "Content-type: text/html"
# echo ""

# Contenu HTML dynamique
echo "<html><head><title>CGI Bash</title></head><body>"
echo "<h1>Date et Heure du Serveur</h1>"
echo "<p>La date d'aujourd'hui est : <b>$(date '+%Y-%m-%d')</b></p>"
echo "<p>L'heure est : <b>$(date '+%H:%M:%S')</b></p>"
echo "<hr>"
echo "</body></html>"
