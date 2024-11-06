# Malloc-ENS
Implémentation en C des fonctions malloc et free de la librarie standard, dans le
cadre d'un projet de L3 à l'ENS Saclay.

# Structure du projet
Le projet est composé des fichiers suivants :

Librairie :
- myalloc.h et myalloc.c qui implémentent les procédures malloc, free et realloc.
- tests.h et tests.c qui implémentent des fonctions de test pour la librairie, 
et les appelent.

Utilisateur :
- project.h qui correspond au header de librarie : il garde l'abstraction des 
variables statiques, constantes etc.. depuis le point de vue utilisateur.
- main.c qui appelle `#include "project.h"` et utilise les fonctions de la librairie
du point de vue utilisateur.

# Détails d'implémentation

Pour conserver la lisibilité du code, ainsi que dans une démarche de programmation
modulaire, j'ai choisi de garder les implémentations de la partie 2, et de distinguer les fonctions
"de gros blocs" et de "petits blocs" par les suffixes `_small` ou `_big`. Les fonctions
sans suffixes appellées par l'utilisateur regardent donc si le bloc est un "gros bloc",
implémenté par la structure `BigBloc`, ou un "petit bloc" implémenté par la structure `Bloc`,
et appellent la fonction avec le suffixe correspondant.

L'implémentation des fonctions de gros blocs transcrit simplement les directives imposées,
et ajoute donc les blocs libres à une liste `big_free`, pour pouvoir les réutiliser directement
ou les diviser ultérieurement. La fonction `my_realloc` a subit quant à elle de nombreuses modifications.
En effet, on se contentait auparavent de vérifier que la taille passée en argument n'était pas trop
grande. Désormais, on doit donc diminuer la taille du bloc en le divisant si possible, ou augmenter la
taille avec un séquence malloc-copy-free.

Par ailleurs, j'ai implémenté deux extensions : la fusion de blocs, et l'agrandissement du tableau de petits blocs.

Pour le premier, les printf utilisés lors du débugage rendent parfois impossible la fusion de deux blocs. Néanmoins,
un test de fusion est proposé. La fusion utilise des nouveaux champs introduts dans la structure de `BigBloc`, qui 
indiquent les blocs les plus proches en mémoire, ainsi que le prédécesseur dans la liste `big_free`.

Pour le second, lorsque le tableau `small_tab` est plein, on alloue un nouveau tableau avec sbrk, et
on fait pointer le dernier bloc vers le premier bloc du nouveau tableau. Je ne choisis pas de libérer les
tableaux lorsqu'ils ne contiennent plus de blocs occupés, puisque leur allocation est couteuse.

# Compilation
Pour compiler le projet, utiliser gcc avec les options suivantes : 

- test : 
`gcc -Wall -Wextra -Werror -Wshadow -ansi -pedantic myalloc.c tests.c`
- utilisation depuis main.c : 
`gcc -Wall -Wextra -Werror -Wshadow -ansi -pedantic myalloc.c main.c`

Deux fichiers `test_example` et `test_example_sans_fusion` sont présents, proposant
un exemple d'exécution des tests sur la librairie, avec ou sans l'implémentation de la fusion
de blocs.
