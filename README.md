# Nuage de Points 2D Interactif

## Présentation du Projet

Ce projet a été développé dans le cadre du cours de **Programmation des Interfaces Graphiques et Interactives**. Il s'agit d'une application interactive de visualisation d'un nuage de points 2D avec OpenGL et GLFW.

## Fonctionnalités

- Génération aléatoire de 1000 points en mouvement sur un plan 2D
- Interaction utilisateur via la souris pour sélectionner un point virtuel
- Détection et visualisation en temps réel des k points les plus proches du point sélectionné
- Animation des points avec des mouvements fluides et rebonds sur les bords
- Possibilité de modifier dynamiquement la valeur de k pendant l'exécution

## Installation et Compilation

### Sous Linux

```bash
# Installation des dépendances (Ubuntu/Debian)
sudo apt-get install libglfw3-dev libgl1-mesa-dev

# Compilation
gcc -Wall cloud_of_point_2D.c -lglfw -lGL -lX11 -lm -o cloud_of_point_2D
```

## Contrôles

| Action | Description |
|--------|-------------|
| **Clic gauche + déplacement souris** | Sélectionne un point virtuel et affiche les k points les plus proches |
| **Pavé numérique +** | Augmente la valeur de k (nombre de points à considérer) |
| **Pavé numérique -** | Diminue la valeur de k (minimum 1) |

## Détails d'Implémentation

Le programme est structuré autour des éléments suivants :

- **Structure Point** qui stocke la position (x, y) et la vélocité (v_x, v_y) de chaque point
- **Algorithme de recherche** des k plus proches voisins basé sur le calcul de distance euclidienne
- **Rendu graphique** avec OpenGL pour visualiser les points, les connexions, et le cercle englobant
- **Gestion des événements** utilisateur via les callbacks GLFW
- **Animation fluide** grâce à une boucle de rendu contrôlée par le temps