#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>


#define MAX_DISTANCE 100   // Distance maximale entre l'hôpital et un patient
#define MAX_PATIENTS 10    // Nombre maximal de patients
#define MAX_NURSES 5       // Nombre maximal d'infirmières
#define FILENAME "C:/Users/Romain Goldenchtein/OneDrive/UTT/TC3/NF06/donnees.csv"  // Nom du fichier de sortie

// Structure pour stocker les distances entre l'hôpital et les patients
typedef struct {
    int distances[MAX_PATIENTS][MAX_PATIENTS];
} DistanceMatrix;


// Fonction pour générer aléatoirement les distances entre l'hôpital et les patients
void generateRandomData(int distances[MAX_PATIENTS][MAX_PATIENTS], int numPatients) {
    srand(time(NULL));

    for (int i = 0; i < numPatients; ++i) {
        for (int j = 0; j < numPatients; ++j) {
            if (i != j) {
                distances[i][j] = rand() % MAX_DISTANCE + 1;
            } else {
                distances[i][j] = 0;  // La distance de chaque patient à lui-même est 0
            }
        }
    }
}

// Fonction pour sauvegarder les données dans un fichier CSV
void saveDataToFile(int distances[MAX_PATIENTS][MAX_PATIENTS], int numPatients) {
    FILE *file = fopen(FILENAME, "w");

    if (file != NULL) {
        for (int i = 0; i < numPatients; ++i) {
            for (int j = 0; j < numPatients; ++j) {
                fprintf(file, "%d", distances[i][j]);

                if (j < numPatients - 1) {
                    fprintf(file, ",");
                }
            }
            fprintf(file, "\n");
        }

        fclose(file);
        printf("Données générées et enregistrées dans %s.\n", FILENAME);
    } else {
        printf("Erreur lors de l'ouverture du fichier %s.\n", FILENAME);
    }
}

// Fonction pour charger les données depuis un fichier CSV
void loadDataFromFile(DistanceMatrix *distances, int *numPatients) {
    FILE *file = fopen(FILENAME, "r");

    if (file != NULL) {
        // Lecture du nombre de patients
        fscanf(file, "%d", numPatients);

        // Lecture des distances depuis le fichier
        for (int i = 0; i < *numPatients; ++i) {
            for (int j = 0; j < *numPatients; ++j) {
                fscanf(file, "%d", &distances->distances[i][j]);
            }
        }

        fclose(file);
    } else {
        printf("Erreur lors de l'ouverture du fichier %s.\n", FILENAME);
        exit(EXIT_FAILURE);
    }
}

// Fonction pour calculer la distance totale pour une permutation donnée
int calculateTotalDistance(DistanceMatrix *distances, int *permutation, int numPatients) {
    int totalDistance = 0;

    for (int i = 0; i < numPatients - 1; ++i) {
        int patient1 = permutation[i];
        int patient2 = permutation[i + 1];
        totalDistance += distances->distances[patient1][patient2];
    }

    return totalDistance;
}

// Fonction récursive pour trouver l'ordre optimal de passage entre les patients pour chaque infirmière
void findOptimalOrder(DistanceMatrix *distances, int *currentOrder, int *visited, int currentIndex, int numPatients, int numNurses, int *optimalOrder, int *optimalDistance) {
    // Base case: If the current permutation is complete
    if (currentIndex == numPatients) {
        // Calculate the total distance for this permutation
        int totalDistance = calculateTotalDistance(distances, currentOrder, numPatients);

        // Update the optimal order if necessary
        if (totalDistance < *optimalDistance) {
            *optimalDistance = totalDistance;
            for (int i = 0; i < numPatients; ++i) {
                optimalOrder[i] = currentOrder[i];
            }
        }

        return;
    }

    // Recursive case: For each patient that has not been visited yet
    for (int i = 0; i < numPatients; ++i) {
        if (!visited[i]) {
            // Add the patient to the current order and mark it as visited
            currentOrder[currentIndex] = i;
            visited[i] = 1;

            // Recursively call findOptimalOrder with the updated current order and visited array
            findOptimalOrder(distances, currentOrder, visited, currentIndex + 1, numPatients, numNurses, optimalOrder, optimalDistance);

            // Backtrack: Remove the patient from the current order and mark it as unvisited
            visited[i] = 0;
        }
    }
}

int main() {
    int distances[MAX_PATIENTS][MAX_PATIENTS];
    int numPatients;
    int nbP;

    // Saisie du nombre de patients depuis l'utilisateur
    printf("Veuillez saisir le nombre de patients : ");
    scanf("%d", &numPatients);

    nbP = numPatients;

    if (numPatients > 0 && numPatients <= MAX_PATIENTS) {
        // Génération aléatoire des données
        generateRandomData(distances, numPatients);

        // Enregistrement des données dans un fichier CSV
        saveDataToFile(distances, numPatients);
    } else {
        printf("Le nombre de patients doit être compris entre 1 et %d.\n", MAX_PATIENTS);
        return 0;
    }

    // Charger les données depuis le fichier CSV
    DistanceMatrix data;
    loadDataFromFile(&data, &numPatients);


    printf("%d\n", numPatients);
    // Check if the number of patients is greater than or equal to the number of nurses
    if (nbP >= MAX_NURSES) {
        // Find the optimal order for each nurse
        int optimalOrder[MAX_PATIENTS];
        int optimalDistance = INT_MAX;
        int currentOrder[MAX_PATIENTS];
        int visited[MAX_PATIENTS] = {0};

        findOptimalOrder(&data, currentOrder, visited, 0, nbP, MAX_NURSES, optimalOrder, &optimalDistance);

        // Print the optimal order for each nurse
        printf("Ordre optimal de passage pour chaque infirmière:\n");
        for (int i = 0; i < MAX_NURSES; ++i) {
            printf("Infirmière %d : ", i + 1);
            for (int j = 0; j < nbP; ++j) {
                if (j % MAX_NURSES == i) {
                    printf("%d ", optimalOrder[j]);
                }
            }
            printf("\n");
        }
    } else {
        printf("Le nombre de patients doit être supérieur ou égal au nombre d'infirmières (%d).\n", MAX_NURSES);
    }

    return 0;
}
