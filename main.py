import tkinter as tk
from tkinter import ttk
import subprocess
import csv
import folium # la bibliotèque Folium permet de créer des cartes Html depuis Python
import time
import webbrowser

# Liste des villes et leurs coordonnées
import lib.villes as v
#import lib.villes_americaines as v     # Alternative avec des villes américaines (+ les villes françaises connues)


# Chemin d'accès à modifier 
chemin = "."

# Créer une carte centrée sur la France
carte = folium.Map(location=[46.603354, 1.888334], zoom_start=6)


# Liste de villes connues et leurs coordonnées       
villes_connues = {
    'Paris': (48.8566, 2.3522),
    'Marseille': (43.296482, 5.36978),
    'Lyon': (45.75, 4.85),
    'Toulouse': (43.6045, 1.4440),
    'Nice': (43.7031, 7.2661),
    'Nantes': (47.2184, -1.5536),
    'Strasbourg': (48.5734, 7.7521),
    'Montpellier': (43.6110, 3.8767),
    'Bordeaux': (44.8378, -0.5792),
    'Lille': (50.6292, 3.0573),
    'Troyes': (48.2978, 4.0742),
    # Il est possible d'ajouter plus de villes à afficher par défaut
}

villes_connues_liste = list(villes_connues.keys())


def run_c_program(prgm):
    """
    Fonction qui lance le fichier C avec l'algorithme de Prim
    """
    result = subprocess.run([chemin+'/src/'+prgm], stdout=subprocess.PIPE)
    return result.stdout.decode('utf-8') # remettre au bon format


def run_python():
    """
    Fonction qui lance un fichier python avec l'algorithme de Kruskal
    """
    result = subprocess.run(["python", chemin+'/src/kruskal.py'], stdout=subprocess.PIPE)
    return result.stdout.decode('utf-8') # remettre au bon format
    


def lire_matrice_csv(nom_fichier):
    """
    Lis la matrice enregistrée dans un CSV par le fichier C
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


distances = {} # les distances entre chaque liaison directe entre 2 villes


fenetre_resultat = None    # fenêtre pour la recherche de trajet
affichage_trajet = None   #variable globale pour ne pas avoir de copies
nbr_villes= None
dist_totale = None

correspondance_ville = {}  # Dictionnaire pour mapper les nombres à des villes
    
def print_graph(noeudDepart, noeudArrive):
    """
    Fonction pour imprimer le graphe dans un fichier texte
    """
    global correspondance_ville 
    correspondance_cle = {}  # Dictionnaire pour mapper les clés de type chaîne de caractères à des nombres
    prochaine_cle = 0  # Prochain nombre disponible pour le mappage

    with open(chemin + '/lib/graphe.txt', 'w') as f:
        for cle, valeur in distances.items():
            if cle not in correspondance_cle:
                correspondance_cle[cle] = prochaine_cle
                correspondance_ville[prochaine_cle] = cle # les deux graphes sont complémentaires
                prochaine_cle += 1 # Nombre associé à la prochaine ville
            cle_mappee = correspondance_cle[cle]
            for voisin, distance in valeur:
                if voisin not in correspondance_cle:
                    correspondance_cle[voisin] = prochaine_cle
                    correspondance_ville[prochaine_cle] = voisin
                    prochaine_cle += 1
                voisin_mappe = correspondance_cle[voisin]
                f.write(f'{cle_mappee} {voisin_mappe} {distance}\n')
        
        f.write(f'{correspondance_cle[noeudDepart.strip()]} {correspondance_cle[noeudArrive.strip()]}\n')  # Ajout du numéro associé de noeudDepart et noeudArrive
    
                
                
def fermeture():
    """
    Fonction qui remet la fenêtre Tkinter et ses variables pour le trajet entre 2 villes à None si l'utilisateur la ferme manuellement
    """
    global fenetre_resultat
    global affichage_trajet
    global dist_totale
    global nbr_villes
    fenetre_resultat.destroy()  # on supprime cette fenêtre
    fenetre_resultat = None
    affichage_trajet = None
    dist_totale = None
    nbr_villes = None
    
    
def update_map(algo):
    """
    Met à jour la carte en fonction des villes sélectionnées.
    """
        
    global fenetre_resultat
    if fenetre_resultat is not None:
        fermeture() # on ferme la fenêtre si la map est mise à jour
     
    global distances
    distances = {} # on rénitialise le dictionnaire (au caus où cetaine villes auraient été déséléctionnées)
    
    # Créer une nouvelle carte
    carte = folium.Map(location=[46.603354, 1.888334], zoom_start=6)

    # Liste de chaque ville sélectionnée
    selected_cities = [city for city, var in vars.items() if var.get()]
    
    # Mettre Paris en première position car ville centrale
    if 'Paris' in selected_cities:
        if selected_cities[0] != 'Paris':
            selected_cities.remove('Paris')
            selected_cities.insert(0, 'Paris')
    else:
        selected_cities.insert(0, 'Paris')
    
    print("Villes séléctionnées : ")
    for city in selected_cities:
        print(city, end=" ") # Pas de retour à la ligne
    print("\n")
    selected_cities_coordinates = []
     
    # Ajout des coordonées pour chaque ville séléctionnée   
    for city in selected_cities:
        coordinates = v.villes[city]
        selected_cities_coordinates.append(f"{coordinates[0]}")
        selected_cities_coordinates.append(f"{coordinates[1]}")
    
    # On met chaque coordonnée dans un fichier txt où chaque ligne est la coordonnée X puis Y sur la ligne d'après  
    with open(chemin+'/lib/villes_select.txt', 'w') as file:
        file.write('\n'.join(selected_cities_coordinates))
    
    
    start_time = time.time() # on lance un chrono pour avoir le temps d'execution
    
    
    if algo=="2":
        run_python() # On fait tourner l'algorithme de Krusal en Python qui viendra piocher dans notre fichier txt
    else:
        # On fait tourner le C qui viendra piocher dans notre fichier txt        
        run_c_program("algo_prim")
    
    
    # Lis la matrice que le C a mit dans un CSV
    mat_C = lire_matrice_csv(chemin+'/lib/mat.csv')

    print(f"\n Temps d'execution : {time.time()-start_time} \n") # print le temps mis pour faire les calculs

    
    # Ajoute des pins pour les villes sélectionnées
    for i in range(len(selected_cities)):
        city = selected_cities[i]
        
        # Créer le popup avec le tableau des villes reliées et leur distance (la bibliothèque folium permet de "coder" en HTML directement dans Python)
        popup_content = f"<h5><strong>{city}</strong></h5><table>"
        
        for j in range(len(selected_cities)):
            if mat_C[i][j] != 0:
                connected_city = selected_cities[j]
                
                distance = mat_C[i][j] # les distances ont été transmises dans la matrice par le C
                if selected_cities[i].lower() not in distances:
                    distances[selected_cities[i].lower()] = []
                (distances[selected_cities[i].lower()]).append((connected_city.lower(),distance))
                
                coord1, coord2 = v.villes[city], v.villes[connected_city] 
                
                popup_content += f"<tr><td>{connected_city}</td><td>{distance:.2f} km</td></tr>" # ajout du contenu du popup
                
                folium.PolyLine([coord1, coord2], color="black", 
                                popup=f"Distance: {distance:.2f} km", # la distance s'affiche quand on clique sur la ligne
                                weight=3.5, opacity=1).add_to(carte) 

        

            
        popup_content += "</table>" # on ferme la table

        style = {"prefix": "fa", "color": "darkred", "icon": "subway"} # style pour les icônes ("fa" permet l'accès à plus d'icônes)
        
        # Création des Pins
        folium.Marker(
            location= v.villes[city],
            tooltip=city, # donne le nom de la ville quand on passe la souris dessus
            popup=popup_content, # donne le tableau des villes reliées quand on clique dessus
            icon=folium.Icon(**style) # on applique le style
        ).add_to(carte) 


    carte.add_child(folium.LatLngPopup()) # donne les coordonnés de la carte à l'endroit où on clique
    
    # Enregistrer la carte dans un fichier HTML
    carte.save(chemin+'/carte.html')
    

 
# Choix de l'algorithme pour calculer l'arbre couvrant de poids minimal
print("Quel algorithme voulez-vous utiliser pour tracer le réseau ferrovière ?")
print("1: Algorithme de Prim        2: Algorithme de Kruskal")
algorithme = input("Choix : ")
while not (algorithme=="1" or algorithme=="2"):
    print("Veuillez taper le numéro associé à l'algorithme désiré")
    algorithme = input("Choix : ")   

# Créer une fenêtre Tkinter
root = tk.Tk() 
root.minsize(200, 1) # largeur minimale à 200 pixels
root.title('Séléction des villes à relier') # titre de la fenêtre
root.iconbitmap(chemin+"/lib/trainn.ico") # icon de la fenêtre
ttk.Frame(root, height=5).pack() # Esapce blanc

# Variable IntVar pour chaque ville (directement à 1 pour les villes connues car on les affiche d'office)
vars = {ville: tk.IntVar(value=1 if ville in villes_connues_liste else 0) for ville in v.villes}


# Créer une Entry pour la recherche et une Listbox pour les résultats
search_var = tk.StringVar(value="")
search_entry = ttk.Entry(root, textvariable=search_var)

# Créer un Frame pour contenir les résultats de la recherche
search_results = ttk.Frame(root)
search_results.pack()



def update_search(*args): # args permet d'avoir une actualisation en directe 
    """
    Met à jour les résultats de la recherche chaque fois que l'Entry change
    """
        
    # Supprimer les anciens résultats de la recherche
    for widget in search_results.winfo_children():
        widget.destroy()

    # Ajouter les nouveaux résultats de la recherche
    search_text = search_var.get().lower()
    if search_text != "":
        count = 0 # Nombre de villes max à afficher
        for ville in v.villes:
            if search_text in ville.lower() and count < 20: # Nombre de villes max=20
                result_frame = tk.Frame(search_results) # créé un Frame cochable pour chaque villes de la recherche
                result_frame.pack(fill='x')
                ttk.Label(result_frame, text=ville).pack(side='left')
                
                if ville != "Paris": # Paris ne peut pas être déséléctionné
                    tk.Checkbutton(result_frame, variable=vars[ville]).pack(side='right')
                count += 1
            
    else : # Si on cherche rien, affiche les villes connues
        for ville in villes_connues:
            if search_text in ville.lower():
                result_frame = tk.Frame(search_results) # créé un Frame cochable pour chaque villes de la recherche
                result_frame.pack(fill='x')
                ttk.Label(result_frame, text=ville).pack(side='left')
                
                if ville != "Paris": # Paris ne peut pas être déséléctionné
                    tk.Checkbutton(result_frame, variable=vars[ville]).pack(side='right')
                
   
search_var.trace_add('write', update_search) # la fonction se met en marche à chaque fois que search_var change


def select_part():
    """
    Séléctionne toutes les villes de la recherche
    """
    for widget in search_results.winfo_children():
        if isinstance(widget, tk.Frame):
            try: # il arrive que les villes soient déjà séléctionnées ou non séléctionnable (comme Paris)
                checkbutton = widget.winfo_children()[1]
                checkbutton.select()
            except:
                continue
            
            
def select_all():
    """
    Séléctionne toutes les villes 
    """
    for var in vars.values():
        if var!=vars['Paris']: # Paris déjà séléctionné
            var.set(1)
    

def deselect_all():
    """
    Déséléctionne toutes les villes
    """
    for var in vars.values():
        if var!=vars['Paris']: # sauf Paris
            var.set(0)


def Dijkstra(depart, arrive):
    """
    Fonction qui renvoie le chemin le plus court entre 2 villes et la distance associée
    """
    print_graph(depart.lower(), arrive.lower()) # Écriture du graphe dans le fichier txt
    chemin_distance = run_c_program("dijkstra")
    if chemin_distance=="RIEN": # si pas de chemin trouvé (cas anormal)
        print("Aucun chemin trouvé reliant ces 2 noeuds")
        return
    lignes = chemin_distance.split("\n")
    
    # Transforme la première ligne en une liste d'entiers
    chemin_court = [int(x) for x in lignes[0].split()]
    for i in range(len(chemin_court)):
        chemin_court[i] = correspondance_ville[chemin_court[i]].capitalize() # on récupère la ville associée aux nombres 

    chemin_court.reverse() # dijkstra.c renvoie le chemin à l'envers
    nb_villes = len(chemin_court)
 
    # Transforme la dernière ligne en un nombre à virgule flottante
    cout = float(lignes[-1])


    if chemin_court is not None:
        for i in range(1, (len(chemin_court)//10)+1):
            chemin_court.insert(i*10,'\n') # on rajoute des insertions de lignes toutes les 10 villes pour éviter d'avoir une fenêtre trop grande

    
    # On transforme la liste en chaîne de caractères relié par des flèches
    trajet_str = " ➜ ".join(chemin_court)
    trajet_str_bis = " -> ".join(chemin_court)

 
    try: # il y a parfois des problèmes de caractères spéciaux
        print(f"Le chemin le plus court pour aller de {depart.strip().capitalize()} à {arrive.strip().capitalize()} est : {trajet_str} \n")
    except:
        print(f"Le chemin le plus court pour aller de {depart.strip().capitalize()} a {arrive.strip().capitalize()} est : {trajet_str_bis} \n")
    print("Nombre de villes : ", nb_villes)
    print("Distance totale : ", cout, "\n")
 
 
     # Créer un cadre pour afficher le chemin et la distance totale
    global affichage_trajet
    if affichage_trajet is not None:
        affichage_trajet.destroy()
    affichage_trajet = ttk.Frame(fenetre_resultat) 
    affichage_trajet.pack(fill='x')
    
    # Afficher le chemin  
    ttk.Label(affichage_trajet, text="Chemin : ", font=("TkDefaultFont", 10, "bold")).pack(side='left')  # étiquette pour le chemin
    ttk.Label(affichage_trajet, text=trajet_str).pack(side='left')
    
        # Créer un cadre pour afficher le chemin et la distance totale
    global nbr_villes
    if nbr_villes is not None:
        nbr_villes.destroy()
    nbr_villes = ttk.Frame(fenetre_resultat) 
    nbr_villes.pack(fill='x')
    
    # Afficher le nombre de villes  
    ttk.Label(nbr_villes, text="Nombre de villes : ", font=("TkDefaultFont", 10, "bold")).pack(side='left')  # étiquette pour le chemin
    ttk.Label(nbr_villes, text=str(nb_villes)).pack(side='left')

    # Créer un autre cadre pour afficher la distance totale
    global dist_totale
    if dist_totale is not None:
        dist_totale.destroy()
    dist_totale = ttk.Frame(fenetre_resultat)
    dist_totale.pack(fill='x')
    
    # Afficher la distance totale
    ttk.Label(dist_totale, text="Distance totale:", font=("TkDefaultFont", 10, "bold")).pack(side='left')  # étiquette pour la distance
    ttk.Label(dist_totale, text=round(cout, 3)).pack(side='left') # on arrondi pour éviter les ditances à rallonge


def villes_valides():
    """
    Vérifie si les villes de départ et d'arrivée sont valides.
    """
    depart = ville_depart.get().lower().strip()   
    arrivee = ville_arrivee.get().lower().strip()  
    if depart in distances and arrivee in distances:
        bouton_valider.config(state='normal')
    else:
        bouton_valider.config(state='disabled')


def afficher_trajet():
    """
    Fonction pour afficher le trajet entre 2 villes et la distance associée
    """        
    # Créer une nouvelle fenêtre Tkinter pour afficher le résultat
    global fenetre_resultat
    if fenetre_resultat is not None:
        return  # on évite que l'utilisateur puisse ouvrir plusieurs fenêtres
   
    fenetre_resultat = tk.Tk() 
    fenetre_resultat.minsize(200, 1) # Définir la largeur minimale à 200 pixels
    fenetre_resultat.title('Trajet') # Définir le titre de la fenêtre
    fenetre_resultat.iconbitmap(chemin+"/lib/trainn.ico") # Définir l'icône de la fenêtre

    fenetre_resultat.protocol("WM_DELETE_WINDOW",fermeture)    # si la fen6etre est fermée on lance la fonction fermeture


    # Créer deux widgets Entry pour entrer les villes
    global ville_depart
    global ville_arrivee

    ttk.Frame(fenetre_resultat, height=5).pack()  # Espace
    
    ville_depart = ttk.Entry(fenetre_resultat)
    ville_arrivee = ttk.Entry(fenetre_resultat)

    ville_depart.pack()
    ville_arrivee.pack()
    

    ttk.Frame(fenetre_resultat, height=10).pack()  # Espace

    # Créer un bouton "Valider"
    global bouton_valider
    bouton_valider = ttk.Button(fenetre_resultat, text="Valider", command=lambda: Dijkstra(ville_depart.get(), ville_arrivee.get()))
    bouton_valider.pack()
    
    # Vérfier si les villes sont valides lorsque leurs valeurs changent
    ville_depart.bind('<KeyRelease>', lambda event: villes_valides())
    ville_arrivee.bind('<KeyRelease>', lambda event: villes_valides())
    
    # Désactiver le bouton initialement
    bouton_valider.config(state='disabled')
    ttk.Frame(fenetre_resultat, height=10).pack()  # Espace


ttk.Frame(root, height=5).pack() # Esapce

# Ajouter l'Entry et la Listbox à la fenêtre
search_entry.pack()
ttk.Frame(root, height=5).pack() # Esapce
search_results.pack()

ttk.Frame(root, height=5).pack() # Esapce

style_bouton = ttk.Style()
style_bouton.configure("GreenButton.TButton", background="green") # contours bleus pour le bouton
style_bouton.map('GreenButton.TButton', foreground = [('active', '!disabled', 'green')], background = [('active', '!disabled', 'white')]) # quand la souris passe sur le bouton, on change les couleurs des bordures et du texte

# Créer les boutons pour la séléction partielle et la mise à jour de la carte
ttk.Button(root, text="Sélectionner les villes",command=select_part).pack()
ttk.Frame(root, height=5).pack() # Esapce
ttk.Button(root, text="Mettre à jour la carte", style="GreenButton.TButton", command=lambda: update_map(algorithme)).pack()


ttk.Frame(root, height=10).pack() # Esapce

# Ajout d'un séparateur entre les boutons
ttk.Separator(root, orient="horizontal").pack(fill="x")

ttk.Frame(root, height=10).pack() # Esapce
style_bouton.configure("BlackButton.TButton", background="black") # contours noirs pour le bouton
style_bouton.configure("RedButton.TButton", background="red") # contours verts pour le bouton
style_bouton.configure("BlueButton.TButton", background="blue") # contours bleus pour le bouton
style_bouton.map('BlueButton.TButton', foreground = [('active', '!disabled', 'blue'),], background = [('active', '!disabled', 'white')])
style_bouton.map('RedButton.TButton', foreground = [('active', '!disabled', 'red')], background = [('active', '!disabled', 'white')])
style_bouton.map('BlackButton.TButton', background = [('active', '!disabled', 'white')]) 


# Ajout des 2 boutons avec leur fonction associé
ttk.Button(root, text="Tout sélectionner",cursor="plus", style="BlueButton.TButton", command=select_all).pack()
ttk.Frame(root, height=5).pack()
ttk.Button(root, text="Tout désélectionner",cursor="plus", style="RedButton.TButton", command=deselect_all).pack()
ttk.Frame(root, height=10).pack()

ttk.Separator(root, orient="horizontal").pack(fill="x")
ttk.Frame(root, height=10).pack() # Esapce

# Bouton pour le trajet entre 2 villes
ttk.Button(root, text="Chemin entre 2 villes", style="BlackButton.TButton", command=afficher_trajet).pack()
ttk.Frame(fenetre_resultat, height=10).pack()  # Espace


update_search()
update_map(algorithme) # Première mise à jour pour avoir les villes connues de base

try:
    webbrowser.open("carte.html") # on affiche la carte dans une page web
except:
    print("Impossible d'ouvrir la carte (faites le manuellement)")

# Démarrer la boucle principale Tkinter
root.mainloop()


