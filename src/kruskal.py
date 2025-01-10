import math
import csv

chemin = "."


class Graph:
    """
    Classe pour représenter un graphe.
    """
    def __init__(self, nb_sommets):
        """
        Initialisation du graphe avec un nombre de sommets.
        """
        self.nb_sommets = nb_sommets
        self.sommets = [(0, 0) for _ in range(nb_sommets)]
        self.adjacency_matrix = [[0] * nb_sommets for _ in range(nb_sommets)] # on remarque la très forte ressemblance avec la structure du fichier algo_prim.c`





def add_edge(graph, source, destination, weight):
    """
    Ajoute une arête entre deux sommets avec un poids donné.
    """
    graph.adjacency_matrix[source][destination] = weight
    graph.adjacency_matrix[destination][source] = weight


def find(parent, i):
    """
    Trouve le parent d'un nœud dans la structure de données.
    """
    if parent[i] == i:
        return i
    return find(parent, parent[i])




def union(parent, rank, x, y):
    """
    Effectue l'union de deux ensembles dans la structure de données.
    """
    # Trouver les racines des ensembles de x et y
    xroot = find(parent, x)
    yroot = find(parent, y)

    # Unir les ensembles en fonction de la hauteur
    if rank[xroot] < rank[yroot]:
        parent[xroot] = yroot
    elif rank[xroot] > rank[yroot]:
        parent[yroot] = xroot
    else:
        parent[yroot] = xroot
        rank[xroot] += 1





def kruskal(graph):
    """
    Implémente l'algorithme de Kruskal pour trouver l'arbre couvrant minimal.
    """
    # Création de la liste des arêtes
    edges = []
    for i in range(graph.nb_sommets):
        for j in range(i + 1, graph.nb_sommets):
            weight = graph.adjacency_matrix[i][j]
            edges.append((i, j, weight))
    edges.sort(key=lambda x: x[2])

    # Initialisation des structures de données
    parent = [i for i in range(graph.nb_sommets)]
    rank = [0] * graph.nb_sommets
    mst = []

    # Parcours des arêtes dans l'ordre croissant des poids
    for edge in edges:
        x, y, weight = edge
        xroot = find(parent, x)
        yroot = find(parent, y)

        # Si les sommets ne sont pas dans la même composante connexe, on les relie et on ajoute l'arête à l'arbre couvrant minimal
        if xroot != yroot:
            mst.append((x, y, weight))
            union(parent, rank, xroot, yroot)

    return mst


def calculer_distance(x1, y1, x2, y2):
    """
    Calcul de la distance entre deux villes
    """
    
    # Rayon de la Terre en kilomètres
    R = 6371.0
    
    # Conversion des coordonnées en radians
    lat1, lon1, lat2, lon2 = map(math.radians, [x1, y1, x2, y2])
    
    # Calcul des différences de latitude et de longitude
    dlat = lat2 - lat1
    dlon = lon2 - lon1
    
    # Calcul de la formule de la distance entre deux points sur une sphère
    a = math.sin(dlat / 2) ** 2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon / 2) ** 2
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    distance = R * c
    
    return distance





def extract_from_txt(file_path):
    """
    Extrait les coordonnées à partir d'un fichier texte
    """
    coordinates = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for i in range(0, len(lines), 2):
            x = float(lines[i].strip()) # on enlève les espaces inutiles
            y = float(lines[i + 1].strip())
            coordinates.append((x, y))
    return coordinates




def to_adjacency_matrix(coordinates):
    """
    Convertit les coordonnées en une matrice d'adjacence
    """
    nb_sommets = len(coordinates)
    graph = Graph(nb_sommets)
    for i in range(nb_sommets):
        for j in range(i + 1, nb_sommets):
            weight = calculer_distance(coordinates[i][0], coordinates[i][1], coordinates[j][0], coordinates[j][1])
            add_edge(graph, i, j, weight)
    return graph

def run_kruskal(coordinates):
    """
    Exécute l'algorithme de Kruskal sur les coordonnées données
    """
    graph = to_adjacency_matrix(coordinates)
    minimum_spanning_tree = kruskal(graph)
    return minimum_spanning_tree


def arbre_vers_matrix(minimum_spanning_tree, nb_sommets):
    """
    Convertit l'arbre couvrant minimal en une matrice d'adjacence
    """
    matrix = [[0] * nb_sommets for _ in range(nb_sommets)]
    for edge in minimum_spanning_tree:
        x, y, weight = edge
        matrix[x][y] = weight
        matrix[y][x] = weight
    return matrix


def write_to_csv(matrix, file_path):
    """
    Écrit la matrice dans un fichier CSV
    """
    with open(file_path, 'w', newline='') as file:
        writer = csv.writer(file)
        for row in matrix:
            writer.writerow([' '.join(str(cell) for cell in row) + ' ']) # On ajoute un espace à la fin car le algo_prim.c fait pareil et on évite les problèmes de compatibilité
            



txt_path = chemin+"/lib/villes_select.txt"

coordinates = extract_from_txt(txt_path)
minimum_spanning_tree = run_kruskal(coordinates)
matrix = arbre_vers_matrix(minimum_spanning_tree, len(coordinates))

csv_path = chemin+"/lib/mat.csv"

write_to_csv(matrix, csv_path)
