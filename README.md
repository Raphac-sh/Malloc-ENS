# Malloc-ENS
Implémentation en C des fonctions malloc et free de la librarie standard.

# Structure du projet
Le projet est composé des fichiers suivants :

Librairie :
- myalloc.h et myalloc.c qui implémentent les procédures malloc, free et realloc.
- tests.h et tests.c qui implémentent des fonctions de test pour la librairie.
- project.h qui correspond au header de librarie : il garde l'abstraction des 
variables statiques, constantes etc.. depuis le point de vue utilisateur.

Utilisateur :
- main.c qui appelle `#include "project.h"` et appelle les fonctions de test.

# Compilation
Pour compiler le projet, utiliser gcc avec les options suivantes : 

`gcc -Wall -Wextra -Werror -Wshadow -ansi -pedantic main.c myalloc.c tests.c`

Un exemple d'exécution est proposé dans le fichier test-example.
