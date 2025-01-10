import pandas as pd

villes = []

# Lire le fichier Excel
df = pd.read_excel('ensemble.xlsx', sheet_name='Communes')

# Convertir la colonne de population en numérique (à cause des lignes de textes inutiles)
df.iloc[:, 9] = pd.to_numeric(df.iloc[:, 9], errors='coerce')

# Filtrer les villes dont la population dépasse 50 000
villes = df[df.iloc[:, 9] > 50000].iloc[:, 6].tolist()

print(villes)