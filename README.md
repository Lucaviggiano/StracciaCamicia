# StracciaCamicia

**TL;DR**: Questo progetto utilizza un motore C++ parallelo ad alte prestazioni (HPC) per simulare oltre 100 miliardi di partite di StracciaCamicia, risolvendo il dubbio sull'esistenza di loop infiniti nella variante con carta bloccante.
Analisi su simulazione parallela di diverse partite del gioco noto come StracciaCamicia per distribuzione numero di turni e ricerca possibili loop.

Mi è capitato spesso di giocare con amici e colleghi ad un gioco noto come StracciaCamicia (o come lo chiamiamo dalle mie parti "Batticuore"). Le regole del gioco sono semplici e intuitive, si gioca in 2 persone e ha una peculiarità rara nei giochi di carte : è completamente determinato.
## Regole Gioco
Le regole di gioco sulle quali si sviluppa il progetto potrebbero essere sconosciute al più dei lettori o diverse dalle note in precedenza. Per chiarezza spenderò questo capitolo in una spiegazione più veloce e diretta possibile delle suddette prima di spostare il focus sull'obiettivo della ricerca. 
### Requisiti
- mazzo di carte da gioco italiane (napoletane, bergamasche, etc)
- 2 giocatori
### Set-up
La partita inizia definendo un ordine di gioco, per semplicità definirò due giocatori denominati A e B che ci seguiranno nel corso della spiegazione. Uno tra A e B mischia il mazzo di carte e lo divide in due mazzetti da 20 carte ciascuno. Un mazzetto sarà di possesso del giocatore A e l'altro del giocatore B. I mazzetti andranno sempre tenuti tassativamente coperti, le carte saranno allora rivolte verso il tavolo da gioco e nascoste ad entrambi i giocatori.
Il primo a svolgere una mossa sarà il giocatore che non si è occupato di mischiare, se A mischia e divide i mazzetti sarà il giocatore B a fare la prima mossa.
### Mosse Concesse
L'unica mossa concessa in questo gioco è sfilare dalla testa del proprio mazzetto le carte, posizionandole scoperte sul tavolo per mostrare il valore riportato su ognuna di esse. Tale Mossa è strettamente sequenziale e nel flusso standard del gioco impone di sfilare una sola carta, alternandosi di continuo tra i giocatori.
Ecco un esempio di flusso standard :

A mischia e divide i mazzetti --> B sfila e mostra una carta --> A sfila e mostra una carta --> B ...
#### Carte Attacco, Bloccanti e Lisce
Alla luce della dinamica apprezzata non tutte le carte presenti nel mazzo hanno la stessa funzione. Possono essere divise in 5 categorie
- ASSI : i 4 assi presenti nel mazzo da gioco
- DUE : i 4 due presenti nel mazzo da gioco
- TRE : i 4 tre presenti nel mazzo da gioco
- QUATTRO : i 4 quattro presenti nel mazzo da gioco
- LISCE : tutte le rimanenti carte, ottenute escludendo dal mazzo di gioco le categorie citate sopra (ex. 6 di denara, Re di spade etc) 

le prime tre categorie (assi, due e tre) sono ciò che chiameremo "carte attacco", la categoria del quattro racchiude le "carte bloccanti", le restanti saranno solamente definite come "carte lisce".
### Dinamica di Gioco
Seguendo il flusso standard sopra citato un giocatore, nel suo turno, sfilerà dal mazzetto necessariamente una carta appartenente ad una e una sola delle suddette categorie. In caso venga mostrata una carta liscia o una bloccante il flusso standard rimarrà intatto : la carta verra posizionata sul tavolo scoperta e il turno passerà al giocatore successivo. Se ad esser sfilata è una delle carte attacco invece il flusso di gioco subisce una modifica. Il giocatore che ha sfilato la suddetta diventerà attaccante e l'avversario il difensore. 

L'attaccante impone al difensore di sfilare dal proprio mazzetto tante carte quante dettate dalla carta attacco girata. Se la carta attacco è un'asso il difensore dovrà obbligatoriamente sfilare una carta dalla cima del proprio mazzetto, se la carta è un due allora ne dovrà sfilare due in modo necessariamente sequenziale, e cosi via fino al tre. Il difensore al contempo, obbediendo alla logica difensiva imposta dal gioco, deve sperare che, tra le carte sfilate per difendersi, ci sia una carta bloccante o un ulteriore carta attacco.

Nel caso il difensore sfilasse unicamente carte lisce l'attacco terminerebbe. Il difensore non sarebbe stato allora in grado di difendersi e l'attaccante riceverebbe come premio tutte le carte presenti per terra fino a quel momento. Le carte vinte vanno inserite in coda al proprio mazzetto, senza alterare minimamente l'ordine con cui sono state sfilate nel gioco. Una volta terminato l'attacco il gioco ripartirà, questa volta sarà il vincitore del precedente scontro a partire, sfilando la prima carta in coda al proprio mazzetto, a cui l'avversario risponderà seguendo il flusso standard. 

Se invece venisse sfilata una carta bloccante o una carta attacco in difesa, l'attacco terminerebbe in modo fallimentare. La carta bloccante semplicemente blocca l'attacco riportando il flusso di gioco standard in rigore. Di conseguenza dopo il blocco, il turno passa al precedente attaccante e si riprende a sfilare normalmente.

Le carte attacco invece invertono i ruoli, ora colui che prima difendeva diventa l'attaccante con la stessa logica applicata in precedenza. Ne nasce un attacco a catena da cui il precedente attaccante, ora difensore, dovrà difendersi seguendo le regole del gioco.

### Esempio flusso completo di gioco : 

1. **A** mischia le carte e divide i mazzetti
2. **B** sfila e mostra una carta *liscia* (6 di denara)
3. **A** sfila e mostra una carta *bloccante* (4 di spade)
4. **B** sfila e mostra una carta *attacco* (tre di bastoni)
5. **A** deve difendersi (ha 3 carte difesa a disposizione):
   * *1ª carta*: sfila una carta *liscia* (7 di spade)
   * *2ª carta*: sfila una carta *bloccante* (4 di bastoni)
6. **B** sfila e mostra una carta *liscia* (donna di coppe)
7. **A** sfila e mostra una carta *attacco* (asso di denara)
8. **B** deve difendersi (ha 1 carta difesa a disposizione):
   * *1ª carta*: sfila una carta *attacco* (due di spade)
9. **A** deve difendersi (ha 2 carte difesa a disposizione):
   * *1ª carta*: sfila una carta *liscia* (5 di coppe)
   * *2ª carta*: sfila una carta *liscia* (re di spade)

* **B vince l'attacco**: prende tutte le carte presenti sul tavolo e le infila in coda al proprio mazzetto.
* **Conteggio carte**: **B** ha 26 carte | **A** ha 14 carte.

10. **B** riparte, sfila e mostra una carta *liscia* (re di bastoni)
11. **A** sfila e mostra ...

### Fine della partita e Proclamazione del vincitore
La partita finisce quando l'intero mazzo di carte è in possesso di uno dei due giocatori, ergo un giocatore non possiede più carte nel proprio mazzetto. Esso corrisponde ad una configurazione 40 - 0 o 0 - 40 dei due mazzetti. Si decreta allora vincitore ovviamente il giocatore in possesso del mazzo completo (il "40" nella configurazione precedente).

## Presentazione Progetto
Ora che si ha un quadro generale completo rispetto al dominio di interesse, possiamo porre le domande a cui cercheremo di dar risposta. In primis come accennavo all'inizio il gioco è determinato, non c'è scelta umana, le regole dettano in modo completo la risoluzione del gioco. Ne segue banalmente che il gioco è perfettamente simulabile con una macchina a stati finiti e facilmente implementabile. Partendo dai due mazzetti iniziali e dall'ordine di gioco stabilito se ne potrebbe facilmente decretare il vincitore in pochi millisecondi.

La principale domanda in questa prima sezione è la seguente : è possibile che una partita non termini mai?

Essa corrisponde, una volta definita l'architettura a stati precisa alla domanda : esiste un loop tra stati? 

### Definizione FSM
Il gioco rappresenta un sistema discreto autonomo, non ci sono input di conseguenza ciò che di norma sarebbe cosi : $$\delta: Q \times \Sigma \to Q$$
collassa al solo $$\delta: Q \to Q$$ poichè l'alfabeto di input è identicamente nullo  ($\Sigma = \emptyset$)

La definizione di stato dovrà allora essere estremamente rigorosa in modo da non creare assenza di determinismo. Per rispettare la richiesta ho codificato uno stato come informazione unicamente caratterizzata dai due mazzetti e giocatore di partenza del ciclo. Gli stati saranno enumerati per numero di carte nei mazzetti e corrispondente ordine di carte e di turno. Una transizione non è un turno di sfilamenti di carte alternato, bensi è tutto un ciclo, da quando si parte alla prima raccolta post-attacco. E' proprio successivamente a tale evento che il gioco si troverà in un'ulteriore stato valido con diverse configurazioni dei mazzetti e di turno.

Alla luce di ciò illustrerò alcuni numeri per farsi un'idea della dimensione del nostro insieme di stati.

Per iniziare ci terrei a specificare che è palese una presenza possibile di *pruning* in tale insieme e ne farò riferimento più avanti.

Per i calcoli faremo riferimento alla divisione per categorie del mazzo di carte apprezzata in precedenza. Sono 12 carte attacco divise in gruppi da 4, 4 carte blocco e 24 carte lisce.\
Tenendo conto di ciò se ne deriva che le permutazioni all'interno degli stessi gruppi sono con ripetizione. In particolare allora la formula per ottenere il numero di stati completo è un semplice coefficiente multinomiale dove va tenuto conto di due aspetti : L'ordine ne raddoppia la dimensione (una stessa identica configurazione dei mazzetti porta a stati diversi a seconda del giocatore che parte a sfilare); le 40 carte devono formare due mazzetti e di conseguenza per ogni combinazione con ripetizione ci saranno 41 tagli disponibili. La formula completa sarà allora : 

$$N_{stati} = \left( \frac{40!}{4! \cdot 4! \cdot 4! \cdot 4! \cdot 24!} \right) \cdot 41 \cdot 2 \approx 3.25016 \times 10^{20}$$

Un numero non esagerato ma comunque troppo grande per approcci di analisi grafo come un Brute-Force BFS.

Ragionando maggiormente sulle logiche di gioco possiamo però tagliare, ridurre, limitare ulteriormente il numero di stati, in particolare, partendo unicamente dalle permutazioni con ripetizione sul mazzo completo ovvero:

$$P = \frac{40!}{4! \cdot 4! \cdot 4! \cdot 4! \cdot 24!} \approx 3.9636 \times 10^{18}$$

sfruttiamo la presenza di tali limitazioni :
 1. Taglio sulla simmetria : l'informazione del turno si può codificare interamente sull'indice del primo mazzo, uno stato del tipo [Mazzo A, Mazzo B, tocca ad A] è equivalente ad uno stato del tipo [Mazzo B, Mazzo A, tocca a B] collassando entrambi su [Mazzo giocatore del turno, Mazzo giocatore non del turno]. Ciò permette di risparmiarsi completamente il fattore 2 nel calcolo totale.
 2. Taglio sui pozzi : sui 41 tagli possibili del mazzo permutato due sono evidentemente degli stati finali, indifferentemente dalla configurazione interna gli stati 40 - 0 e 0 - 40 possono collassare in due unici stati finali *Vittoria passivo* e *Vittoria attivo* riducendo ulteriormente i possibili stati a
 
 $$N = (P \cdot 39) + 2 \approx 1.54582 \times 10^{20}$$
    
 3. Taglio per Conclusione affrettata : Se in una partizione tutte le carte attacco sono nel mazzetto di un unico giocatore la vittoria ne segue immediatamente, matematicamente avendo 12 carte attacco e 28 tra lisce e bloccanti qual è la probabilità che un mazzo di massimo 28 carte (se ne avesse di più ci sarebbe necessariamente una carta attacco) non ci siano carte attacco? La distribuzione è ipergeometrica con formula
    
$$Prob(0 \text{ attacchi}) = \frac{\binom{28}{k}}{\binom{40}{k}}$$

 4. Taglio sul dominio logico : Dopo che entrambi i giocatori hanno vinto almeno un attacco sarà impossibile trovare configurazioni con carte in coda diverse da :
    * 1. Suffix Asso: [..., Asso, Liscia]
    * 2. Suffix Due: [..., Due, Liscia, Liscia]
    * 3. Suffix Tre: [..., Tre, Liscia, Liscia, Liscia]

    Se ne deriva dalle regole, se un giocatore ha raccolto carte il difensore ha "giocato carta liscia tot volte" dove *tot* è minimo 1. Per quanto questa euristica propone un potenziale risparmio enorme sulle permutazioni non potrà essere inserita nella formula finale dato che l'informazione "ho pescato almeno una volta" non è davvero codificabile matematicamente.

<br>
La formula finale sarà allora

$$N_{explore} = 2 + \sum_{k=1}^{39} \left[ P \cdot \left( 1 - H(k) - H(40-k) \right) \right] \approx 1.37507 \times 10^{20}$$

con *H(k)* la probabilità che nel taglio k la partizione sia in uno stato di conclusione affrettata (3) (essa vale 0 se *k* > 28 ).

Precisiamo un aspetto sul "Taglio sul dominio logico". Si potrebbe pensare di sfruttarlo definendo uno stato a Regime da cui passano tutte le partite, tale stato indicherebbe una vittoria di ciclo (e conseguente raccolta carte) di entrambi i giocatori. Successivamente ad esso la nostra riduzione (stimata al 98.5%)  entrerebbe in rigore trasformando la formula degli stati esplorabili in modo "dinamico". Tuttavia, questa riduzione a priori è matematicamente scorretta. Esistono infatti rami del grafo perfettamente validi (i cosiddetti "cappotti") in cui un giocatore subisce attacchi continui senza mai vincere una presa. In questi scenari, il giocatore passivo non altererà mai il fondo del proprio mazzo, conservando la "memoria" dello shuffle iniziale (che potrebbe benissimo essere una carta attacco) fino al suo esaurimento.

Sebbene non sia un vincolo assoluto per tutte le foglie del grafo, la regola del suffisso regolare agisce come un potente attrattore. Non appena la partita supera le fasi iniziali e si verifica uno scambio di prese tra i due giocatori, il sistema collassa in un sottospazio in cui entrambi i mazzetti seguono necessariamente la regola sui suffissi. Calcolando la probabilità combinata, sappiamo che solo circa il 1.46% degli stati teorici possiede una struttura di questo tipo. Questo significa che, una volta esaurito il transitorio iniziale, la nostra macchina a stati finiti (FSM) abbandonerà per sempre quasi il 99% dello spazio $N_{explore}$, confinando i loop infiniti e le partite più lunghe all'interno di questo nucleo grammaticale ristretto.
# Implementazione
Alla luce dei risultati apprezzati nel paragrafo precedente, una ricerca esaustiva di loop nello spazio degli stati esplorabili non è computazionalmente affrontabile. L'unico approccio valido è la simulazione di quante più partite possibili partendo da mazzi mischiati in modo semi-randomico. Il focus della parte implementativa sarà questo : Creare una struttura che permetta di simulare il numero maggiore di partite, alla ricerca di un loop.

**Primo dettaglio fondamentale** : Il problema di simulare partite è estremamente parallelizzabile. Le partite non si influenzano tra loro, quindi, sfruttando al massimo la potenza di calcolo e parallelismo della cpu, se ne possono simulare multiple in parallelo.

**Secondo dettaglio fondamentale** : Trovare una partita che non finisce mai unicamente dalla simulazione è notoriamente impossibile. Avendo però definito la macchina a stati finiti che descrive il gioco, diventa più facile trovare dei cicli guardando la struttura a grafo da essa generata. Le partite simulate avranno quindi un Cutoff limite sulla durata della simulazione, una volta superato tale limite la partita verrà salvata e analizzata successivamente tramite algoritmi di rilevamento cicli.

## Microstati e struttura dei dati
Il passaggio tra uno stato e l'altro, come definiti in precedenza, richiede più turni. Un turno è definito dallo sfilamento di una carta da uno dei due mazzetti. Poiché nella simulazione della partita non possiamo passare direttamente da uno stato all'altro senza simulare tutti i turni interni, definiamo dei Microstati. Essi sono delle specializzazioni degli stati definiti in precedenza ma distano tra loro un singolo turno. Inglobano anche gli stati standard ma hanno più informazioni cosi da permettere una valida simulazione a livello implementativo.
### Informazioni Microstati :
Ecco una lista delle informazioni che caratterizzano univocamente un Microstato :
- Mazzetto del giocatore A
- Mazzetto del giocatore B
- Mazzetto nel tavolo
- giocatore che deve svolgere il turno
- giocatore in attacco
- numero di carte da sfilare

Si noti che nel caso il mazzetto nel tavolo fosse vuoto ci troveremmo in uno degli stati standard come definitivi in precedenza.

<img width="1625" height="447" alt="image" src="https://github.com/user-attachments/assets/2c8beeac-f47b-4d0c-97f6-585825608983" />

*Figura 1: Rappresentazione di un Microstato e dei suoi componenti durante la simulazione.*


---

## Kernel
Definiti i Microstati la struttura è chiara. Partendo da un determinato microstato si guardano le informazioni sul giocatore che deve svolgere il turno, il numero di carte da sfilare e il giocatore in attacco e si passa al microstato successivo seguendo le regole del gioco. 

Le carte sono codificate seguendo la categorizzazione apprezzata in precedenza. Le carte lisce sono codificate da uno 0, gli assi da 1, i due da 2 e i tre da 3 e le carte bloccanti da 4. Seguendo questa codifica, con delle corte strutture di blocchi if annidati rispetto al numero di carte da sfilare e alla carta sfilata nel turno, possiamo tranquillamente simulare un turno di gioco. Salviamo inoltre in un contatore il numero di turni simulati dall'inizio della partita. Tale kernel di simulazione è altamente parallelizzato e ottimizzato. 

I risultati di ogni partita simulata sono :
  1. Seed di partenza del mazzo da carte completo (pre-shuffle), per riproducibilità della partita.
  2. Numero di Turni della partita, per Cutoff.


## Simulazione completa
Nella sezione "Guida al programma" si potranno trovare le indicazioni per riprodurre l'esperimento o altre funzionalità avanzate aggiunte. La simulazione completa segue un flusso elementare : si lancia la simulazione di uno specifico numero di partite (argomento del programma), con uno specifico Cutoff (anche esso argomento) - Tutte le partite che avranno una durata in turni superiore a tale Cutoff saranno salvate, in particolare verrà salvato il seed del mazzo di partenza cosi da poterla riprodurre. Si esegue una analisi dei cicli su partite che superano cut-off (algoritmo di rilevamento dei cicli di Floyd).

# Risultati Simulazione
Inanzitutto è da apprezzare la potenza simulativa di un kernel ben ottimizzato. In una cpu da 32 core si è raggiunto un Throughput di 22.8922 Milioni di sim/sec. Simulando 100 miliardi di partite (in 1 ora e 12 minuti) i risultati rispondo a pieno al dubbio cuore del progetto.

Sono state trovate ben 4 partite infinite :  la dinamica è tale che il sistema riesce a trovare delle particolari disposizioni delle carte che, dopo un breve transitorio iniziale, portano a un ciclo chiuso in cui i due giocatori continuano a scambiarsi prese all'infinito senza che nessuno riesca a svuotare il mazzetto dell'avversario. Di seguito i dettagli strutturali e, soprattutto, i mazzi iniziali per ognuna di queste configurazioni. Si Ricorda che le prime 20 carte appartengono al Giocatore A e le restanti 20 al Giocatore B - sarà quest'ultimo ad iniziare.

---

**Seed: 3549138389513009901**

Questo loop entra a regime quasi subito, ma il ciclo in sé è gigantesco, durando decine di migliaia di turni prima di ripetersi.

- **Transitorio iniziale (Inizio ciclo)**: 1 stato
- **Lunghezza del Loop** (stati): 3.220
- **Durata del Loop** (Turni): 31.680 turni per singola ripetizione
- **Mazzo Iniziale** (dalla cima alla coda - diviso in mazzetti): 00302020301002040010 01003040010040030204

---

**Seed: 3549138391254191215**

Un ciclo leggermente più corto, ma altrettanto affascinante. Raggiunge il regime dopo soli 2 stati (ovvero dopo le primissime 2 prese).

- **Transitorio iniziale (Inizio ciclo)**: 2 stati
- **Lunghezza del Loop** (stati): 1.876
- **Durata del Loop** (Turni): 18.999 turni per singola ripetizione
- **Mazzo Iniziale** (dalla cima alla coda - diviso in mazzetti): 00400100304002030100 14003030201004002020

---

**Seed: 3549138409557862522**

Questo seed parte esattamente da una configurazione che è già parte del loop perfetto, senza alcun transitorio di stati da smaltire.

- **Transitorio iniziale (Inizio ciclo)**: 0 stati
- **Lunghezza del Loop** (stati): 3.220
- **Durata del Loop** (Turni): 31.720 turni per singola ripetizione
- **Mazzo Iniziale** (dalla cima alla coda - diviso in mazzetti): 40030400201004003010 00203030100202040010

NOTE
*Verifica Attrattori*: La durata del loop in stati di questo seed (3220) è identica a quella del primo seed. Un'analisi approfondita ha confermato che **entrambi convergono esattamente allo stesso ciclo attrattore**. Pur partendo da disposizioni iniziali diverse, raggiungono la stessa identica sequenza di stati periodici, entrando nel ciclo in punti differenti.

---

**Seed: 3549138451982464284**

Questo ciclo è di un ordine di grandezza più piccolo rispetto agli altri, il che lo rende teoricamente più "facile" da analizzare se si volesse tracciare manualmente la sequenza di attacchi e blocchi.

- **Transitorio iniziale (Inizio ciclo)**: 3 stati
- **Lunghezza del Loop** (stati): 420
- **Durata del Loop** (Turni): 3.920 turni per singola ripetizione
- **Mazzo Iniziale** (dalla cima alla coda - diviso in mazzetti): 10014004003010020203 00200301004004003020



<img width="1500" height="600" alt="Loop_infinito" src="https://github.com/user-attachments/assets/8bbdbdb8-c90d-453d-981c-2033c2ad0efa" />

*Figura 2: Tracciato di un loop (andamento del numero di carte per giocatore nel tempo).*


# Guida al programma
In questa sezione si illustrano le modalità di compilazione e le diverse funzionalità offerte dal programma. Il progetto è scritto interamente in C++17 e utilizza CMake come sistema di build. Non ha dipendenze esterne, si appoggia unicamente alla libreria standard e ai thread POSIX (o equivalente Windows).

## Compilazione
Per compilare il progetto è sufficiente creare una cartella di build e lanciare CMake. Su sistemi Linux o macOS :
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
Su Windows con Visual Studio installato il procedimento è identico, CMake genererà automaticamente il progetto per MSVC. L'eseguibile verrà prodotto direttamente nella cartella `build/` indipendentemente dalla piattaforma, grazie alla configurazione forzata dell'output directory nel CMakeLists.

## Modalità di utilizzo
Il programma espone diverse modalità operative selezionabili tramite argomenti da riga di comando. Se nessun argomento viene fornito il programma parte in modalità simulazione con i valori di default (1 milione di partite, cutoff a 10.000 turni).

### Simulazione massiva
È la modalità principale del programma, quella per cui è stato concepito. Lancia la simulazione parallela di un numero arbitrario di partite, distribuendo il carico su tutti i core disponibili della macchina.
```bash
./StracciaCamicia [num_partite] [cutoff] [num_thread]
```
- **num_partite** : quante partite simulare in totale. Il valore di default è 1.000.000.
- **cutoff** : il numero massimo di turni concessi ad una singola partita prima di essere interrotta e segnalata come sospetta. Il valore di default è 10.000.
- **num_thread** : il numero di thread da utilizzare per la simulazione. Il valore di default è il numero massimo di thread hardware rilevati automaticamente dalla macchina.

Il seed di partenza viene generato automaticamente combinando `std::random_device` e il clock di sistema ad alta risoluzione, garantendo unicità tra esecuzioni successive. Ogni partita simulata utilizza un seed incrementale a partire da quello iniziale. Al termine della simulazione il programma stampa un report completo con vittorie, turni medi, estremi e la lista dei seed sospetti che hanno raggiunto il cutoff.

Esempio di lancio per 100 miliardi di partite con cutoff a 5000 turni su 32 thread :
```bash
./StracciaCamicia 100000000000 5000 32
```

### Tracciamento singola partita
Data una partita di interesse (tipicamente individuata dalla simulazione massiva) è possibile riprodurla turno per turno generando un file CSV con lo stato del gioco ad ogni singolo sfilamento.
```bash
./StracciaCamicia [seed]
```
Il programma ricostruisce il mazzo iniziale a partire dal seed fornito, simula la partita e stampa su stdout un CSV con le colonne : `Turno, Carte_A, Carte_B, Dim_Tavolo, Carta_Pescata, Blocco`. Questo output è pensato per essere rediretto su file e successivamente analizzato o graficato con strumenti esterni.

Esempio di utilizzo :
```bash
./StracciaCamicia 3549138451982464284 > traccia.csv
```
il file risultante può essere caricato nel script `plot_trace.py` incluso nel repository per visualizzare graficamente l'andamento delle carte dei due giocatori nel tempo.

### Partita personalizzata
È possibile simulare una partita a partire da un mazzo definito manualmente, senza passare per il meccanismo dei seed. Il programma accetta in input un file di testo con il mazzo completo e i nomi dei giocatori.
```bash
./StracciaCamicia [nome_file.txt]
```
Il file deve rispettare il seguente formato :
```
Mazzo : 0030202030100204001001003040010040030204
Giocatori Alice Bob
```
dove la stringa del mazzo è composta da 40 cifre (le prime 20 per il primo giocatore, le restanti 20 per il secondo) e la riga Giocatori specifica i nomi dei due sfidanti. Il programma stamperà il vincitore e il numero di turni impiegati.

### Salvataggio mazzo da seed
Per esportare il mazzo generato da un determinato seed in un file di testo riutilizzabile nella modalità personalizzata :
```bash
./StracciaCamicia save [seed] [nome_file.txt]
```
Il programma genererà il file con il mazzo mischiato corrispondente al seed specificato, pronto per essere ispezionato manualmente o caricato nella modalità partita personalizzata.

### Ispezione seed sospetti
Questa è la modalità di analisi post-simulazione. Dato un file contenente i seed sospetti (prodotto automaticamente dalla simulazione massiva), il programma li rifiltra con un cutoff più elevato e, per quelli che lo superano, esegue l'algoritmo di rilevamento cicli di Floyd (lepre e tartaruga) per determinare con certezza se la partita entra in un loop infinito.
```bash
./StracciaCamicia inspect [file_sospetti.txt] [cutoff]
```
- **file_sospetti.txt** : il file prodotto dalla simulazione contenente le righe nel formato `- Seed: 123456789`.
- **cutoff** : il nuovo limite di turni per il filtraggio. Si consiglia un valore sensibilmente superiore a quello usato nella simulazione originale (ad esempio 20.000 se la simulazione usava 5.000).

Per ogni seed che supera il cutoff l'algoritmo di Floyd individua l'esatto punto di ingresso nel ciclo e la sua lunghezza, sia in termini di stati (prese completate con tavolo vuoto) sia in turni effettivi.

Esempio di utilizzo :
```bash
./StracciaCamicia inspect sospetti.txt 20000
```

### Analisi batch da file
La modalità batch consente di testare in serie un elenco di mazzi predefiniti, tipicamente generati da analisi combinatorie esterne. Il programma legge un file di testo dove ogni riga rappresenta un mazzo completo (40 cifre) e simula la partita corrispondente, segnalando quelli che raggiungono il cutoff.
```bash
./StracciaCamicia batch [file.txt]
```
Il file deve contenere una riga per mazzo, ciascuna composta dalle 40 cifre nella codifica standard (0=Liscia, 1=Asso, 2=Due, 3=Tre, 4=Blocco). Il cutoff in questa modalità è fissato a 10.000 turni.

## Conclusioni
L'analisi massiva condotta dimostra che la regola del "4 bloccante" funge da iniettore di entropia nel sistema. Mentre nel gioco tradizionale i loop infiniti sono noti e relativamente meno complessi, l'introduzione di questa singola regola altera significativamente la dinamica di convergenza, rendendo i loop non del tutto impossibili, ma statisticamente quasi inesistenti (con una probabilità stimata intorno allo $0.000000004\%$). Questo risultato conferma che le regole apparentemente semplici di "StracciaCamicia" nascondono un grafo degli stati di straordinaria complessità e fascino computazionale.
