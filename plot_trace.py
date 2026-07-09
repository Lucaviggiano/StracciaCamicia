import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Uso: python plot_trace.py <nome_file.csv>")
    sys.exit(1)

csv_filename = sys.argv[1]

if not os.path.exists(csv_filename):
    print(f"Errore: il file '{csv_filename}' non esiste.")
    sys.exit(1)

# Carica i dati estratti dal kernel C++
try:
    df = pd.read_csv(csv_filename, encoding="utf-8")
except UnicodeDecodeError:
    df = pd.read_csv(csv_filename, encoding="utf-16")

plt.figure(figsize=(15, 6))
plt.title("Evoluzione delle Carte: La rottura della Risonanza tramite i '4'")

# 1. Disegna le carte in mano ai giocatori (Linee standard)
plt.plot(df['Turno'], df['Carte_A'], label='Giocatore A', color='blue', alpha=0.7)
plt.plot(df['Turno'], df['Carte_B'], label='Giocatore B', color='red', alpha=0.7)

# 2. Riempi l'area del tavolo (Entropia di base)
plt.fill_between(df['Turno'], 0, df['Dim_Tavolo'], label='Massa sul Tavolo', color='gray', alpha=0.3)

# 3. IL RADAR DEI BLOCCHI: Filtra il DataFrame trovando solo le righe in cui c'è stato un blocco
eventi_quattro = df[df['Blocco'] == 1]

# 4. Plotta i segnalini ('X' nere) facendoli coincidere con l'altezza del tavolo in quel preciso turno
plt.scatter(eventi_quattro['Turno'], eventi_quattro['Dim_Tavolo'], 
            color='black', marker='x', s=60, zorder=5, label='Carta 4 (Scossa Entropica)')

plt.xlabel("Numero di Turni")
plt.ylabel("Numero di Carte")
plt.legend(loc='upper left') # Spostiamo la legenda per non coprire i picchi finali
plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()
plt.show()
