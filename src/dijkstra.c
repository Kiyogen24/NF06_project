#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Structure pour une arete
typedef struct {
    int noeud;
    double distance;
} Arete;

// Structure pour représenter un noeud
typedef struct {
    int noeud;
    double distance;
} Noeud;

// Fonction Dijkstra qui permet de trouver le chemin le plus court entre 2 noeuds (vu en cours)
int Dijkstra(int depart, int arrivee, int numNoeuds, Arete** graphe) {

    double* distances = malloc(numNoeuds * sizeof(double)); // Tableau pour stocker les distances
    int* visite = malloc(numNoeuds * sizeof(int)); // Tableau pour marquer les noeuds visités
    int* precedent = malloc(numNoeuds * sizeof(int)); // Tableau pour stocker les noeuds précédents

    for (int i = 0; i < numNoeuds; i++) {
        distances[i] = INT_MAX; // Initialiser toutes les distances à une valeur maximale
        visite[i] = 0; // tous les noeuds sont non visités
        precedent[i] = -1; // pas encore de noeuds précédents
    }

    distances[depart] = 0; // distance du noeud de départ à 0

    if (numNoeuds > 1) {
        for (int i = 0; i < numNoeuds - 1; i++) {
            double distanceMin = INT_MAX; // Distance minimale initiale
            int noeudMin = -1; // Noeud avec la distance minimale

            for (int j = 0; j < numNoeuds; j++) {
                if (!visite[j] && distances[j] < distanceMin) {
                    distanceMin = distances[j]; // Mettre à jour la distance minimale
                    noeudMin = j; // on met à jour le noeud avec la distance minimale
                }
            }

            visite[noeudMin] = 1; // le noeud devient visité
            for (int j = 0; j < numNoeuds; j++) {
                if (!visite[j] && graphe[noeudMin][j].distance != -1) {
                    double nouvelleDistance = distances[noeudMin] + graphe[noeudMin][j].distance; // Calculer la nouvelle distance
                    if (nouvelleDistance < distances[j]) {
                        distances[j] = nouvelleDistance; // Mettre à jour la distance si elle est plus petite
                        precedent[j] = noeudMin; // Mettre à jour le noeud précédent
                    }
                }
            }
        }
    }

    
    if (distances[arrivee] == INT_MAX) {
        printf("RIEN"); // Si la distance totale est infinie, afficher "RIEN"
    } else {
        int noeudCourant = arrivee;
        while (noeudCourant != -1) {
            printf("%d ", noeudCourant); // Afficher le chemin de noeuds (chemin le plus court)
            noeudCourant = precedent[noeudCourant]; // Passer au noeud précédent (le chemin sera donc dans le sens inverse)
        }
        printf("\n");
        printf("%f", distances[arrivee]); // Afficher la distance totale
    }

    // Libérer les mémoires allouées
    free(distances); 
    free(visite);
    free(precedent); 

    return 0;
}

int main() {

    const char* txt_path = "./lib/graphe.txt"; // chemin d'accès au CSV

    // Extraire les aretes du graphe à partir du fichier de données
    FILE* fichier = fopen(txt_path, "r");
    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier.\n"); // Afficher un message d'erreur si le fichier ne peut pas être ouvert
        return 1;
    }

    int noeud1, noeud2;
    double distance;
    int val_noeud_max = 0;
    while (fscanf(fichier, "%d %d %lf", &noeud1, &noeud2, &distance) == 3) {
        if (noeud1 > val_noeud_max) {
            val_noeud_max = noeud1; // Mettre à jour la valeur maximale de noeud
        }
        if (noeud2 > val_noeud_max) {
            val_noeud_max = noeud2; // Mettre à jour la valeur maximale de noeud
        }
    }
    
    fscanf(fichier, "%d %d", &noeud1, &noeud2); // La dernière ligne contient le noeud de départ et d'arrivé
    int noeudDepart = noeud1; 
    int noeudArrivee = noeud2; 

    int numNoeuds = val_noeud_max + 1; // Nombre total de noeuds (+1 car il y a le noeud 0)
    Arete** graphe = malloc(numNoeuds * sizeof(Arete*)); // Tableau de pointeurs vers les arêtes
    for (int i = 0; i < numNoeuds; i++) {
        graphe[i] = malloc(numNoeuds * sizeof(Arete)); // Allouer de la mémoire pour chaque arête
        for (int j = 0; j < numNoeuds; j++) {
            graphe[i][j].noeud = -1; // Initialiser les noeuds à -1
            graphe[i][j].distance = -1; // Initialiser les distances à -1
        }
    }
    rewind(fichier); // On rénitialise le parcours du fichier pour lire à partir du début

    while (fscanf(fichier, "%d %d %lf", &noeud1, &noeud2, &distance) == 3) {
        graphe[noeud1][noeud2].noeud = noeud2; // Mettre à jour le noeud de destination
        graphe[noeud1][noeud2].distance = distance; // Mettre à jour la distance
    }
    fclose(fichier); 

    Dijkstra(noeudDepart, noeudArrivee, numNoeuds, graphe); // Appeler la fonction Dijkstra pour trouver le plus court chemin

    // Libérer la mémoire allouée pour le graphe
    for (int i = 0; i < numNoeuds; i++) {
        free(graphe[i]);
    }
    free(graphe);

    return 0;
}