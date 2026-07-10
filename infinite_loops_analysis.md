# Analisi Loop Infiniti (con Regola del 4 Bloccante)

Confermo in pieno! Grazie al nuovo `CycleInspector` integrato nel branch `main`, abbiamo ispezionato i 4 seed sospetti identificati dalla simulazione massiva e **tutti e 4 si sono rivelati dei veri loop infiniti**. 

Nel gioco completo (che include la **regola del 4 bloccante**), la dinamica è tale che il sistema riesce a trovare delle particolari disposizioni delle carte che, dopo un breve transitorio iniziale, portano a un ciclo chiuso in cui i due giocatori continuano a scambiarsi prese all'infinito senza che nessuno riesca a svuotare il mazzetto dell'avversario.

Di seguito i dettagli strutturali e, soprattutto, i **mazzi iniziali** per ognuna di queste straordinarie configurazioni. (Ricorda che le prime 20 carte appartengono al Giocatore A e le restanti 20 al Giocatore B).

---

## Seed: 3549138389513009901
Questo loop entra a regime quasi subito, ma il ciclo in sé è gigantesco, durando decine di migliaia di turni prima di ripetersi.
- **Transitorio iniziale (Inizio ciclo)**: 1 macro-stato
- **Lunghezza del Loop (Macro-stati)**: 3.220
- **Durata del Loop (Turni)**: 31.680 turni per singola ripetizione
- **Mazzo Iniziale**: 
  `00302020301002040010 01003040010040030204`

---

## Seed: 3549138391254191215
Un ciclo leggermente più corto, ma altrettanto affascinante. Raggiunge il regime dopo soli 2 macro-stati (ovvero dopo le primissime 2 prese).
- **Transitorio iniziale (Inizio ciclo)**: 2 macro-stati
- **Lunghezza del Loop (Macro-stati)**: 1.876
- **Durata del Loop (Turni)**: 18.999 turni per singola ripetizione
- **Mazzo Iniziale**: 
  `00400100304002030100 14003030201004002020`

---

## Seed: 3549138409557862522
Questo seed parte *esattamente* da una configurazione che è già parte del loop perfetto, senza alcun transitorio di macro-stati da smaltire.
- **Transitorio iniziale (Inizio ciclo)**: 0 macro-stati
- **Lunghezza del Loop (Macro-stati)**: 3.220
- **Durata del Loop (Turni)**: 31.720 turni per singola ripetizione
- **Mazzo Iniziale**: 
  `40030400201004003010 00203030100202040010`

> [!NOTE] 
> Nota curiosa: La durata del loop in macro-stati di questo seed (3220) è identica a quella del primo seed. Pur partendo da disposizioni iniziali apparentemente diverse, è possibile che entrambi convergano nello stesso enorme attrattore (lo stesso ciclo), sebbene entrino nel ciclo in punti leggermente diversi!

---

## Seed: 3549138451982464284
Questo ciclo è di un ordine di grandezza più piccolo rispetto agli altri, il che lo rende teoricamente più "facile" da analizzare se si volesse tracciare manualmente la sequenza di attacchi e blocchi.
- **Transitorio iniziale (Inizio ciclo)**: 3 macro-stati
- **Lunghezza del Loop (Macro-stati)**: 420
- **Durata del Loop (Turni)**: 3.920 turni per singola ripetizione
- **Mazzo Iniziale**: 
  `10014004003010020203 00200301004004003020`

---

> [!TIP] 
> Ora che hai questi mazzi `(0 = Liscia, 1 = Asso, 2 = Due, 3 = Tre, 4 = Blocco)`, puoi caricarli nei tuoi script Python (`plot_trace.py`) o usare la modalità `trace` per tracciare graficamente il numero di carte e visualizzare letteralmente l'onda stazionaria del loop infinito formarsi davanti ai tuoi occhi!
