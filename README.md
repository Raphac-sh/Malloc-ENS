# Malloc-ENS
Implémentation en C des fonctions malloc et free de la librarie standard, dans le
cadre d'un projet de L3 à l'ENS Saclay.

# Structure du projet
Le projet est composé des fichiers suivants :

Librairie :
- myalloc.h et myalloc.c qui implémentent les procédures malloc, free et realloc.
- tests.h et tests.c qui implémentent des fonctions de test pour la librairie.
- project.h qui correspond au header de librarie : il garde l'abstraction des 
variables statiques, constantes etc.. depuis le point de vue utilisateur.

Utilisateur :
- main.c qui appelle `#include "project.h"` et appelle les fonctions de test.

# Détails d'implémentation

On se contente pour l'instant d'allouer uniquement des petits blocs (128 octets),
dans un tableau statique. Afin d'optimiser la complexité de l'allocation, on maintient
une structure de liste chainée sur les blocs libres, avec un pointeur sur le premier.
On obtient alors toutes les primitives en temps raisonable, contrairement à la première
version.

# Compilation
Pour compiler le projet, utiliser gcc avec les options suivantes : 

`gcc -Wall -Wextra -Werror -Wshadow -ansi -pedantic main.c myalloc.c tests.c`

Un exemple d'exécution sur des fonctions de test est proposé dans le fichier test-example.
