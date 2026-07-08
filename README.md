# StracciaCamicia
Analisi su simulazione parallela di diverse partire del gioco noto come StracciaCamicia per distribuzione numero di turni e ricerca possibili loop.

Mi è capitato spesso di giocare con amici e colleghi ad un gioco noto come StracciaCamicia (o come lo chiamiamo dalle mie parti "Batticuore"). Le regole del gioco sono semplici e intuitive, si gioca in 2 persone e ha una peculiarità rara nei giochi di carte : è completamente determinato.
## Regole Gioco
le regole di gioco sulle quali si sviluppa il progetto potrebbero essere sconosciute al più dei lettori o diverse dalle note in precedenza. Per chiarezza spenderò questo capitolo in una spiegazione più veloce e diretta possibile delle suddette prima di spostare il focus sull'obiettivo della ricerca. 
### Requisiti
- mazzo di carte da gioco italiane (napoletane, bergamasche, etc)
- 2 giocatori
### Set-up
la partita inizia definendo un ordine di gioco, per semplicità definirò due giocatori denominati A e B che ci seguiranno nel corso della spiegazione. uno tra A e B mischia il mazzo di carte e lo divide in due mazzetti da 20 carte ciascuno. Un mazzetto sarà di possesione del giocatore A e l'altro del giocatore B. I mazzetti andranno sempre tenuti tassativamente coperti, le carte saranno allora rivolte verso il tavolo da gioco e nascoste ad entrambi i giocatori.
Il primo a svolgere una mossa sarà il giocatore che non si è occupato di mischiare, se A mischia e divide i mazzetti sarà il giocatre B a fare la prima mossa.
### Mosse Concesse
L'unica mossa concessa in questo gioco è sfilare dalla testa del proprio mazzetto le carte, posizionandole scoperte sul tavolo per mostrare il valore riportato su ognuna di esse. Tale Mossa è strettamente sequenziale e nel flusso standard del gioco impone di sfilare una sola carta, alternandosi di continuo tra i giocatori.
Ecco un esempio di flusso standard : \
A mischia e divide i mazzetti --> B sfila e mostra una carta --> A sfila e mostra una carta --> B ...
#### Carte Attacco, Bloccanti e Lisce
alla luce della dinamica apprezzata non tutte le carte presenti nel mazzo hanno la stessa funzione. Possono esssere divise in 5 categorie
- ASSI : i 4 assi presenti nel mazzo da gioco
- DUE : i 4 due presenti nel mazzo da gioco
- TRE : i 4 tre presenti nel mazzo da gioco
- QUATTRO : i 4 quattro presenti nel mazzo da gioco
- LISCE : tutte le rimanenti carte, ottenute escludendo dal mazzo di gioco le categorie citate sopra (ex. 6 di denara, Re di spade etc) 

le prime tre categorie (assi, due e tre) sono ciò che chiameremo "carte attacco", la categoria del quattro racchiude le "carte bloccanti", le restanti saranno solamente definite come "carte lisce".
### Dinamica di Gioco
Seguendo il flusso standard sopra citato un giocatore, nel suo turno, sfilerà dal mazzetto necessariamente una carta appartenente ad una e una sola delle suddette categorie. In caso venga mostrata una carta liscia o una bloccante il flusso standard rimarrà intatto : la carta verra posizionata sul tavolo scoperta e il turno passerà al giocatore successivo. Se ad esser sfilata è una delle carte attacco invece il flusso di gioco subisce una modifica. Il giocatore che ha sfilato la suddetta diventerà attaccante e l'avversario il difensore. 

l'attaccante impone al difensore di sfilare dal proprio mazzetto tante carte quante dettate dalla carta attacco girata. Se la carta attacco è un'asso il difensore dovrà obbligatoriamente sfilare una carta dalla cima del proprio mazzetto, se la carta è un due allora ne dovrà sfilare due in modo necessariamente sequenziale, e cosi via fino al tre. il difensore al contempo, obbediendo alla logica difensiva imposta dal gioco, deve sperare che, tra le carte sfilate per difendersi, ci sia una carta bloccante o un ulteriore carta attacco.

Nel caso il difensore sfilasse unicamente carte liscie l'attacco terminerebbe. Il difensore non sarebbe stato allora in grado di difendersi e l'attaccante riceverebbe come premio tutte le carte presenti per terra fino a quel momento. Le carte vinte vanno inserite in coda al proprio mazzetto, senza alterare minimamente l'ordine con cui sono state sfilate nel gioco. Una volta terminato l'attacco il gioco ripartirà, questa volta sarà il vincitore del precedente scontro a partire, sfilando la prima carta in coda al proprio mazzetto, a cui l'avversario risponderà seguendo il flusso standard. 

Se invece venisse sfilata una carta bloccante o una carta attacco in difesa, l'attacco terminerebbe in modo fallimentare. La carta bloccante semplicemente blocca l'attacco riportando il flusso di gioco standard in rigore. Di conseguenza dopo il blocco, il turno passa al precedente attaccante e si riprende a sfilare normalmente. \
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

## Fine della partita e decreto vincitore
la partita finisce quando l'intero mazzo di carte è in possesso di uno dei due giocatori, ergo un giocatore non possiede più carte nel proprio mazzetto. Esso corrisponde ad una configurazione 40 - 0 o 0 - 40 dei due mazzetti. Si decreta allora vincitore ovviamente il giocatore in possesso del mazzo completo (il "40" nella configurazione precedente).



