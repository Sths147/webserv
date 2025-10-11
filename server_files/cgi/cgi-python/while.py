#!/usr/bin/env python3
import time
import sys
import os

# 🛑 CECI EST CRUCIAL POUR VOTRE SERVEUR WEB !
# --- En-têtes HTTP Requis (Décommentés) ---
# print("Content-Type: text/html")
# print() # Ligne vide OBLIGATOIRE pour séparer l'en-tête du corps
# ------------------------------------------

# Début du HTML
print("<!DOCTYPE html><html><head><title>Boucle CGI Robuste</title>")
print("<meta http-equiv='refresh' content='1'>")
print("</head><body>")

compteur = 0
LIMITE = 10
print(f"<p>PID du script: {os.getpid()}</p>")

# La boucle de "travail" (while True)
while True:
    try:
        # 1. Générer le contenu
        print(f"<h1>Compteur : {compteur} / {LIMITE}</h1>")
        print(f"<p>Le script s'exécute depuis {compteur} secondes.</p>")

        # 2. Forcer l'envoi de la sortie au serveur
        sys.stdout.flush()

    # --- Gestion des Erreurs (pour la robustesse) ---
    except BrokenPipeError:
        # Si le serveur (votre WebServ) coupe la connexion (timeout, déconnexion),
        # on évite le crash du script.
        print("", file=sys.stderr)
        break

    except KeyboardInterrupt:
        # Pour les tests manuels avec Ctrl+C
        print("\nInterruption manuelle (Ctrl+C). Arrêt.", file=sys.stderr)
        break
    # -----------------------------------------------

    # 3. Condition d'arrêt normale
    if compteur >= LIMITE:
        print("<h2>🏁 Limite atteinte. Arrêt normal du script.</h2>")
        break

    compteur += 1

    # 4. Simuler une tâche de longue durée
    time.sleep(1)

# Fin du HTML
print("</body></html>")
