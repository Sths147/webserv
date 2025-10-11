#!/usr/bin/env python3
import time
import sys

# --- En-têtes HTTP Requis ---
# Indique que la réponse est du HTML
# print("Content-Type: text/html")
# print()
# ---------------------------

# Début du HTML
print("<!DOCTYPE html><html><head><title>Boucle CGI Protégée</title>")
# Rafraîchit la page chaque seconde. Chaque rafraîchissement relance le script.
print("<meta http-equiv='refresh' content='1'>")
print("</head><body>")

compteur = 0
LIMITE = 10

# La boucle de "travail" (while True)
while True:
    try:
        # 1. Générer le contenu
        print(f"<h1>Compteur : {compteur} / {LIMITE}</h1>")
        print("<p>Le script s'exécute. Veuillez attendre.</p>")

        # 2. Forcer l'envoi de la sortie au serveur
        sys.stdout.flush()

    except BrokenPipeError:
        # C'est la gestion de l'erreur ! Si le client ou le serveur ferme le tube,
        # on sort de la boucle proprement.
        print("")
        break

    # 3. Condition d'arrêt normale
    if compteur >= LIMITE:
        print("<h2>🏁 Limite atteinte. Arrêt normal du script.</h2>")
        break

    compteur += 1

    # 4. Simuler une tâche de longue durée
    time.sleep(1)

# Fin du HTML
print("</body></html>")
