**Attuale funzionamento del sistema:**

1. il client computa un vettore audio
2. il vettore viene inviato al server
3. il server scrive il contenuto del vettore in un buffer di memoria
4. il server legge tutti gli altri buffer e computa un vettore somma
5. il vettore somma viene inviato al client
6. il thread in ascolto a lato client riceve il vettore e lo appende in una queue
7. il client somma il vettore in queue al vettore audio che sta attualmente computando (vai al punto 1)

**Problemi:**

1. il server può ricevere un vettore audio prima di un altro, stessa cosa il client (dimostrato con assert(currTID == lastTID + 1))

**TODO** (entro Lunedì, Mercoledì al massimo)

Implementare vettore di vettori a lato server. Il TID ricevuto dal client indica quale vettore aggiornare (operazione %).
Ogni stream client ha una storia dell'ultimo TID di risposta; verrà usato per scegliere quale vettore degli altri client leggere.
Sarebbe il top scrivere zero (resettare) un vettore quando è stato letto una volta da tutti i client.
Implementare la stessa cosa a lato client. Resettare il vettore non appena viene letto.

*Nota del 07/05 1:00~*
Implementato ring buffer (vettore di vettori). Sembrano esserci meno artefatti. Il delay tra client e server è decisamente notabile (0.5 sec. se non di peggio); probabilmente corrisponde alla dimensione del buffer?

*Nota del 24/05 17:25~*
Il sistema produce artefatti solo se sono connessi più di due client (il problema non si presenta su test in locale).
Gli artefatti sono probabilmente dovuti alla successione di vettori contenenti audio e vettori contenenti zeri (ritornati) dal server in caso di mancanza di vettori nel buffer).

*Tempo di esecuzione della object function UDPResponse*
- 1 client connesso: ~0.050 ms
