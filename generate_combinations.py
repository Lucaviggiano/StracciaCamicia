import itertools

deck_A = "00200000021000013000"
deck_B = "00020001020310030300"
base_deck = deck_A + deck_B

# Trova tutti gli indici delle lisce (valore '0')
zero_indices = [i for i, card in enumerate(base_deck) if card == '0']

if len(zero_indices) != 28:
    print(f"Errore: trovate {len(zero_indices)} lisce, ma ce ne aspettavamo 28.")
    exit(1)

combinations = list(itertools.combinations(zero_indices, 4))
print(f"Generate {len(combinations)} combinazioni (atteso 20475).")

with open("combinations.txt", "w") as f:
    for combo in combinations:
        deck_list = list(base_deck)
        for idx in combo:
            deck_list[idx] = '4'
        deck_str = "".join(deck_list)
        f.write(deck_str + "\n")

print("Salvataggio completato in combinations.txt")
