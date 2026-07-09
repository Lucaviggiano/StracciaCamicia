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

### Fine della partita e decretamento vincitore
la partita finisce quando l'intero mazzo di carte è in possesso di uno dei due giocatori, ergo un giocatore non possiede più carte nel proprio mazzetto. Esso corrisponde ad una configurazione 40 - 0 o 0 - 40 dei due mazzetti. Si decreta allora vincitore ovviamente il giocatore in possesso del mazzo completo (il "40" nella configurazione precedente).

## Presentazione Progetto
ora che si ha un quadro generale completo rispetto al dominio di interesse, possiamo porre le domande a cui cercheremo di dar risposta. In primis come accennavo all'inizio il gioco è determinato, non c'è scelta umana, le regole dettano in modo completo la risoluzione del gioco. Ne segue banalmente che il gioco è perfettamente simulabile con una macchina a stati finiti e facilmente implementabile. Partendo dai due mazzetti iniziali e dall'ordine di gioco stabilito se ne potrebbe facilmente decretare il vincitore in pochi millisecondi.

La principale domanda in questa prima sezione è la seguente : è possibile che una partita non termini mai? \
essa corrisponde, una volta definita l'architettura a stati precisa alla domanda : esiste un loop tra stati? 

### Definizione FSM
Il gioco rappresenta un sistema discreto autonomo, non ci sono input di conseguenza ciò che di norma sarebbe cosi : $$\delta: Q \times \Sigma \to Q$$
collassa al solo $$\delta: Q \to Q$$ poichè l'alfabeto di input è identicamente nullo  ($\Sigma = \emptyset$)

La definizione di stato dovrà allora essere estremamente rigorosa in modo da non creare assenza di determinismo. Per rispettare la richiesta ho codificato uno stato come informazione unicamente caratterizzata dai due mazzetti e giocatore di partenza del ciclo. Gli stati saranno enumerati per numero di carte nei mazzetti e corrispondente ordine di carte e di turno. Una transizione non è un turno di sfilamenti di carte alternato, bensi è tutto un ciclo, da quando si parte alla prima raccolta post-attacco. E' proprio successivamente a tale evento che il gioco si troverà in un'ulteriore stato valido con diverse configurazioni dei mazzetti e di turno.

Alla luce di ciò illustrerò alcuni numeri per farsi un'idea della dimensione del nostro insieme di stati. \
Per iniziare ci terrei a specificare che è palese una presenza possibile di *pruning* in tale insieme e ne farò riferimento più avanti. \
Per i calcoli faremo riferimento alla divisione per categorie del mazzo di carte apprezzata in precedenza. sono 12 carte attacco divise in gruppi da 4, 4 carte blocco e 24 carte lisce.\
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

con *H(k)* la probabilità che nel taglio k la partizione sia in uno stato di conclusione affrettata (3) (essa vale 0 se *k* > 28 ). \
Precisiamo un aspetto sul "Taglio sul dominio logico". Si potrebbe pensare di sfruttarlo definendo uno stato a Regime da cui passano tutte le partite, tale stato indicherebbe una vittoria di ciclo (e conseguente raccolta carte) di entrambi i giocatori. Successivamente ad esso la nostra riduzione (stimata al 98.5%)  entrerebbe in rigore trasformando la formula degli stati esplorabili in modo "dinamico". Tuttavia, questa riduzione a priori è matematicamente scorretta. Esistono infatti rami del grafo perfettamente validi (i cosiddetti "cappotti") in cui un giocatore subisce attacchi continui senza mai vincere una presa. In questi scenari, il giocatore passivo non altererà mai il fondo del proprio mazzo, conservando la "memoria" dello shuffle iniziale (che potrebbe benissimo essere una carta attacco) fino al suo esaurimento.

Sebbene non sia un vincolo assoluto per tutte le foglie del grafo, la regola del suffisso regolare agisce come un potente attrattore. Non appena la partita supera le fasi iniziali e si verifica uno scambio di prese tra i due giocatori, il sistema collassa in un sottospazio in cui entrambi i mazzetti seguono necessariamente la regola sui suffissi. Calcolando la probabilità combinata, sappiamo che solo circa il 1.46% degli stati teorici possiede una struttura di questo tipo. Questo significa che, una volta esaurito il transitorio iniziale, la nostra macchina a stati finiti (FSM) abbandonerà per sempre quasi il 99% dello spazio $N_{explore}$, confinando i loop infiniti e le partite più lunghe all'interno di questo nucleo grammaticale ristretto.


