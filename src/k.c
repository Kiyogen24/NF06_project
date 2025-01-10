#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// Structure pour représenter un sommet avec ses coordonnées
typedef struct {
    float x;
    float y;
} Coordinate;

// Structure pour représenter une arête
typedef struct {
    int source;
    int destination;
    float weight; // distance
} Edge;

// Structure pour représenter un graphe
typedef struct {
    int nb_sommets;
    Coordinate* sommets;
    float** matrice_adjacence;
} Graph;

// Fonction pour créer un graphe avec le nombre de sommets spécifié
Graph* create_graph(int nb_sommets) {
    // Allocation de mémoire pour la structure du graphe
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->nb_sommets = nb_sommets;

    // Allocation de mémoire pour les sommets et la matrice d'adjacence
    graph->sommets = (Coordinate*)malloc(nb_sommets * sizeof(Coordinate));
    graph->matrice_adjacence = (float**)malloc(nb_sommets * sizeof(float*));

    // Initialisation de la matrice d'adjacence avec des zéros
    for (int i = 0; i < nb_sommets; i++) {
        graph->matrice_adjacence[i] = (float*)calloc(nb_sommets, sizeof(float));
    }

    return graph;
}

// Fonction pour ajouter une arête entre deux sommets avec un poids donné
void add_edge(Graph* graph, int source, int destination, float weight) {
    graph->matrice_adjacence[source][destination] = weight;
    graph->matrice_adjacence[destination][source] = weight;
}

// Fonction pour trouver la racine de l'ensemble contenant l'élément donné
int find(int* parent, int i) {
    if (parent[i] == i) {
        return i;
    }
    return find(parent, parent[i]);
}

// Fonction pour effectuer l'union de deux ensembles
void union_set(int* parent, int* rank, int x, int y) {
    int xroot = find(parent, x);
    int yroot = find(parent, y);

    if (rank[xroot] < rank[yroot]) {
        parent[xroot] = yroot;
    }
    else if (rank[xroot] > rank[yroot]) {
        parent[yroot] = xroot;
    }
    else {
        parent[yroot] = xroot;
        rank[xroot] += 1;
    }
}

// Fonction de comparaison pour trier les arêtes en fonction du poids
int compare_edges(const void* a, const void* b) {
    Edge* edge1 = (Edge*)a;
    Edge* edge2 = (Edge*)b;
    return (edge1->weight > edge2->weight) - (edge1->weight < edge2->weight);
}

// Fonction pour trouver l'arbre couvrant minimum en utilisant l'algorithme de Kruskal
Edge* kruskal(Graph* graph, int* num_edges) {
    int nb_sommets = graph->nb_sommets;

    // Création d'un tableau pour stocker toutes les arêtes
    Edge* edges = (Edge*)malloc(nb_sommets * (nb_sommets - 1) / 2 * sizeof(Edge));
    int edge_index = 0;

    // Remplissage du tableau des arêtes avec toutes les arêtes possibles et leurs poids
    for (int i = 0; i < nb_sommets; i++) {
        for (int j = i + 1; j < nb_sommets; j++) {
            float weight = graph->matrice_adjacence[i][j];
            edges[edge_index].source = i;
            edges[edge_index].destination = j;
            edges[edge_index].weight = weight;
            edge_index++;
        }
    }

    // Tri des arêtes en fonction du poids
    qsort(edges, edge_index, sizeof(Edge), compare_edges);

    // Tableaux pour l'algorithme Union-Find
    int* parent = (int*)malloc(nb_sommets * sizeof(int));
    int* rank = (int*)calloc(nb_sommets, sizeof(int));
    Edge* mst = (Edge*)malloc((nb_sommets - 1) * sizeof(Edge));

    // Initialisation de chaque élément comme un ensemble séparé
    for (int i = 0; i < nb_sommets; i++) {
        parent[i] = i;
    }

    int mst_index = 0;

    // Construction de l'arbre couvrant minimum
    for (int i = 0; i < edge_index; i++) {
        int x = edges[i].source;
        int y = edges[i].destination;
        int xroot = find(parent, x);
        int yroot = find(parent, y);

        if (xroot != yroot) {
            mst[mst_index] = edges[i];
            mst_index++;
            union_set(parent, rank, xroot, yroot);
        }
    }

    *num_edges = mst_index;
    free(parent);
    free(rank);
    free(edges);

    return mst;
}

// Fonction pour calculer la distance entre deux points sur la Terre
float calculate_distance(float x1, float y1, float x2, float y2) {
    float R = 6371.0;

    float lat1 = x1 * 3.141593 / 180;
    float lon1 = y1 * 3.141593 / 180;
    float lat2 = x2 * 3.141593 / 180;
    float lon2 = y2 * 3.141593 / 180;

    float dlat = lat2 - lat1;
    float dlon = lon2 - lon1;
    float a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    float distance = R * c;

    return distance;
}

// Fonction pour extraire les coordonnées à partir d'un fichier texte
Coordinate* extract_from_txt(const char* file_path, int* num_coordinates) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return NULL;
    }

    Coordinate* coordinates = NULL;
    char line[9900]; // le nombre de villes max actuellement est de 250 mais on alloue plus de mémoire pour si on ajoute des villes
    int line_index = 0;

    // Lecture des coordonnées depuis le fichier
    while (fgets(line, sizeof(line), file) != NULL) {
        float x, y;

        // Extraction de la coordonnée x
        if (sscanf(line, "%f", &x) != 1) {
            printf("Erreur lors de la lecture des coordonnées.\n");
            fclose(file);
            free(coordinates);
            return NULL;
        }

        // Extraction de la coordonnée y
        if (fgets(line, sizeof(line), file) == NULL || sscanf(line, "%f", &y) != 1) {
            printf("Erreur lors de la lecture des coordonnées.\n");
            fclose(file);
            free(coordinates);
            return NULL;
        }

        // Réallocation de mémoire pour le tableau de coordonnées
        coordinates = (Coordinate*)realloc(coordinates, (line_index + 1) * sizeof(Coordinate));
        coordinates[line_index].x = x;
        coordinates[line_index].y = y;
        line_index++;
    }

    fclose(file);
    *num_coordinates = line_index;

    return coordinates;
}

// Fonction pour écrire la matrice d'adjacence dans un fichier CSV
void write_to_csv(float** matrix, int nb_sommets, const char* file_path) {
    FILE* file = fopen(file_path, "w");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    // Écriture de la matrice dans le fichier CSV
    for (int i = 0; i < nb_sommets; i++) {
        for (int j = 0; j < nb_sommets; j++) {
            fprintf(file, "%.2f ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main() {

    const char* file_path = "C:/Users/Romain Goldenchtein/OneDrive/UTT/TC3/NF06/Projet/villes_select.txt";

    int num_coordinates;
    Coordinate* coordinates = extract_from_txt(file_path, &num_coordinates);
    if (coordinates == NULL) {
        return 1;
    }

    // Création d'un graphe et ajout des arêtes en fonction des distances entre les coordonnées
    Graph* graph = create_graph(num_coordinates);
    for (int i = 0; i < num_coordinates; i++) {
        for (int j = i + 1; j < num_coordinates; j++) {
            float weight = calculate_distance(coordinates[i].x, coordinates[i].y, coordinates[j].x, coordinates[j].y);
            add_edge(graph, i, j, weight);
        }
    }

    int num_edges;
    Edge* minimum_spanning_tree = kruskal(graph, &num_edges);

    // Création d'une matrice pour représenter l'arbre couvrant minimum
    float** matrix = (float**)malloc(num_coordinates * sizeof(float*));
    for (int i = 0; i < num_coordinates; i++) {
        matrix[i] = (float*)calloc(num_coordinates, sizeof(float));
    }

    // Remplissage de la matrice avec les poids de l'arbre couvrant minimum
    for (int i = 0; i < num_edges; i++) {
        int x = minimum_spanning_tree[i].source;
        int y = minimum_spanning_tree[i].destination;
        float weight = minimum_spanning_tree[i].weight;
        matrix[x][y] = weight;
        matrix[y][x] = weight;
    }

    const char* csv_path = "C:/Users/Romain Goldenchtein/OneDrive/UTT/TC3/NF06/Projet/mat.csv";

    // Écriture de la matrice dans un fichier CSV
    write_to_csv(matrix, num_coordinates, csv_path);

    // Libération de la mémoire allouée
    for (int i = 0; i < num_coordinates; i++) {
        free(graph->matrice_adjacence[i]);
    }
    free(graph->matrice_adjacence);
    free(graph->sommets);
    free(graph);
    free(coordinates);

    return 0;
}
