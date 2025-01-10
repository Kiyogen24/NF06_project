import networkx as nx
import matplotlib.pyplot as plt
import csv

# Chemin d'accès à modifier 
chemin = "C:/Users/Romain Goldenchtein/OneDrive/UTT/TC3/NF06/Projet"

def draw_graph(matrix):
    # Crée un objet graphique
    G = nx.Graph()

    # Ajoute les arêtes au graphe
    for i in range(len(matrix)):
        for j in range(i + 1, len(matrix[i])):
            if matrix[i][j] != 0:
                G.add_edge(i, j, weight=round(matrix[i][j])) # on arrondi pour éviter le superflux

    # Positionne les nœuds en utilisant le layout de ressort (spring layout)
    pos = nx.spring_layout(G)

    # Trace le graphe
    nx.draw(G, pos, with_labels=True, font_weight='bold', node_size=700, node_color='skyblue', font_color='black')

    # Ajoute les poids aux arêtes
    labels = nx.get_edge_attributes(G, 'weight')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=labels)

    plt.show()



def lire_matrice_csv(nom_fichier):
    """
    Lis la matrice enregistrée dans un CSV
    """
    matrice = []
    sub_mat = []
    with open(nom_fichier, 'r') as fichier:
        lecteur = csv.reader(fichier)
        
        for ligne in lecteur:
            elt = ligne[0] # Change chaque ligne du CSV en une liste
            elt = elt.split(" ")
            elt.pop()
            for element in elt:
                sub_mat.append(float(element))
                
            matrice.append(sub_mat) # On a donc une liste de listes
            sub_mat = []
    return matrice


graph = lire_matrice_csv(chemin + '/lib/mat.csv')


draw_graph(graph)