#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // booléan
#include <math.h>



// Structure pour représenter un sommet avec ses coordonnées
typedef struct {
    float x;
    float y; 
} Coord;

// Structure pour représenter un graphe
typedef struct {
    int nbSommets; // Nombre de sommets
    Coord* sommets; // Tableau des sommets
    float** adjacencyMatrix; // Matrice d'adjacence (float pour une précision en mètre des distances)
} Graph;


// Fonction pour initialiser un graphe
void initGraph(Graph* graph, int nbSommets) {
    graph->nbSommets = nbSommets;
    graph->sommets = (Coord*)malloc(nbSommets * sizeof(Coord));
    graph->adjacencyMatrix = (float**)calloc(nbSommets, sizeof(float*));
    for (int i = 0; i < nbSommets; i++) {
        graph->adjacencyMatrix[i] = (float*)calloc(nbSommets, sizeof(float)); // initialisation du poids des arêtes à 0
    }
}


// Fonction pour ajouter une arête entre deux sommets
void addEdge(Graph* graph, int source, int destination, float distance) {
    graph->adjacencyMatrix[source][destination] = distance; // le poids est la distance entre 2 villes
    graph->adjacencyMatrix[destination][source] = distance;
}

// Calcul de la distance entre deux villes 
float calculer_distance(float x1, float y1, float x2, float y2) {
    float R = 6371.0;  // Rayon de la Terre en kilomètres

    // Conversion en radians
    float lat1 = x1 * 3.141593 / 180;
    float lon1 = y1 * 3.141593 / 180;
    float lat2 = x2 * 3.141593 / 180;
    float lon2 = y2 * 3.141593 / 180;

    float dlon = lon2 - lon1; // Différence entre les coordonnées
    float dlat = lat2 - lat1;

    float a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a)); // Formule de Haversine pour calculer la distance entre deux points de la Terre

    float distance = R * c;

    return distance;
}


// Fonction pour trouver l'index du sommet le plus proche du sous-graphe
int PlusProcheSommet(float distances[], bool visited[], int nbSommets) {
    float minDistance = 100000.0; // Grand nombre
    int minIndex = -1;
    for (int i = 0; i < nbSommets; i++) {  // boucle pour trouver le minimum
        if (!visited[i] && distances[i] < minDistance) {
            minDistance = distances[i];
            minIndex = i;
        }
    }
    return minIndex; 
}


// Fonction pour afficher l'arbre de recouvrement minimal
void printGraph(int parent[], Graph* graph) {
    printf("Arbre de recouvrement minimal :\n");
    for (int i = 1; i < graph->nbSommets; i++) {
        printf("%d - %d\n", parent[i], i); // On affiche à qui est relié chaque noeud (que les indices)
    }
}

// Fonction pour exécuter l'algorithme de Prim centré sur Paris
void AlgoPrim(Graph* graph, int startCoord, float** mat) {
    int parent[graph->nbSommets];
    float distances[graph->nbSommets];
    bool visited[graph->nbSommets]; // tableau booléen pour savoir si un noeud à un certain indice à déjà été visité

    // Initialiation
    for (int i = 0; i < graph->nbSommets; i++) { 
        distances[i] = 100000.0; // Grand nombre
        visited[i] = false;
    }

    distances[startCoord] = 0.0; // Distance à lui-même nul
    parent[startCoord] = -1; // Pas de parent pour Paris

    // Pour chaque sommet
    for (int i = 0; i < graph->nbSommets; i++) {
        int actuelCoord = PlusProcheSommet(distances, visited, graph->nbSommets);
        visited[actuelCoord] = true; // On change de sommet donc il devient visité

        // Mise à jour des distances et parent pour ce sommet si la distance entre les 2 n'est pas nul, que le sommet n'est pas visité et que la distance est inférieur à la distance minimale
        for (int j = 0; j < graph->nbSommets; j++) {
            if (graph->adjacencyMatrix[actuelCoord][j] != 0 && !visited[j] && graph->adjacencyMatrix[actuelCoord][j] < distances[j]) { 
                parent[j] = actuelCoord;
                distances[j] = graph->adjacencyMatrix[actuelCoord][j];
            }
        }
        printf("\n");
        if (i != 0) { // le premier sommet (Paris) n'a pas de parent
            mat[parent[actuelCoord]][actuelCoord] = graph->adjacencyMatrix[actuelCoord][parent[actuelCoord]];
            mat[actuelCoord][parent[actuelCoord]] = graph->adjacencyMatrix[actuelCoord][parent[actuelCoord]];   // on inscrit la distance séparant les deux sommets dans mat   
        }
    }

    printGraph(parent, graph); // Print du graph (pas de nom de villes car pas communiqué par le Python)
}


int main() {
    Graph graph;

    const char* file_path = "./lib/villes_select.txt"; // chemin d'accès au txt

    // On ouvre le fichier txt où il y a les coordonnées des villes séléctionnées
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier !\n");
        return 1;
    }

    char line[400]; // le nombre de lignes max actuellement est de 250 mais on alloue plus de mémoire pour si on ajoute des villes
    int linenbber = 0;
    while (fgets(line, sizeof(line), file)){ // On parcours une fois le fichier pour avoir le nombre de lignes
        linenbber++;
    }
    int nb_villes = linenbber / 2; // le nombre de villes est le nombre de lignes divisé par 2
    initGraph(&graph, nb_villes); // On créé le graph avec le nombre de villes

    linenbber = 0; // On rénitalise le nombre de villes et le parcours dans le fichier
    rewind(file);

    // Boucle parcourant chaque lignes du fichier
    while (fgets(line, sizeof(line), file)) {
        
        // Supprimer le caractère de saut de ligne à la fin de la ligne
        line[strcspn(line, "\n")] = 0;

        // Ajouter les coordonnées pour chaque sommets
        if (linenbber % 2 == 0) {
            sscanf(line, "%f", &graph.sommets[linenbber / 2].x); // sscanf lis les informations obtenus par fgets (comme scranf mais avec les strings)
        } else {
            sscanf(line, "%f", &graph.sommets[linenbber / 2].y);
        }

        linenbber++;
    }

    fclose(file);

    
    

    // Ajouter les arêtes (le poids est égale à la distance entre les 2 sommets)
    for (int i = 0; i < nb_villes; i++) {
        for (int j = 0; j < nb_villes; j++) {
            if (i != j) {
                addEdge(&graph, i, j, calculer_distance(graph.sommets[i].x, graph.sommets[i].y, graph.sommets[j].x, graph.sommets[j].y));
            }
        }
    }
    

    float** mat = (float**)calloc(nb_villes, sizeof(float*)); // matrice initialisé à 0 qui va être print dans un CSV

    for (int i = 0; i < nb_villes; i++) {
        mat[i] = (float*)calloc(nb_villes, sizeof(float)); // allocation pour chaque liste de la matrice
    }

    // Exécuter l'algorithme de Prim centré sur Paris
    AlgoPrim(&graph, 0, mat);

    // Afficher la matrice mat
    for (int i = 0; i < nb_villes; i++) {
        for (int j = 0; j < nb_villes; j++) {
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }

    const char* csv_path = "./lib/mat.csv"; // chemin d'accès au CSV

    // Fichier CSV de destination
    FILE *new_csv = fopen(csv_path, "w");
    if (new_csv == NULL) {
        printf("Erreur d'ouverture du fichier !\n");
        return 1;
    }

    // Print de la matrice dans le CSV
    for (int i = 0; i < nb_villes; i++) {
        for (int j = 0; j < nb_villes; j++) {
            fprintf(new_csv, "%f", mat[i][j]);
            fprintf(new_csv," ");
        }
        fprintf(new_csv, "\n");
    }
    fclose(new_csv);


    // Libérer la mémoire allouée pour la matrice mat
    for (int i = 0; i < nb_villes; i++) {
        free(mat[i]);
    }
    free(mat);

    // Libérer la mémoire allouée pour le graphe
    free(graph.sommets);
    for (int i = 0; i < graph.nbSommets; i++) {
        free(graph.adjacencyMatrix[i]);
    }
    free(graph.adjacencyMatrix);

    return 0;
}
