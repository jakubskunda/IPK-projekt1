#**IPK - Projekt 1**

Autor : Jakub Škunda, FIT VUT, 2022
Server vytvorený v jazyku C++ komunikujúci prostredníctvom protokolu HTTP.<br/>
Server naslúcha na zadanom porte a vracia požadované informácie.<br/>
Server po spustení beží na lokálnom serveri v nekonečnom cykle a príjma požiadavky.<br/>
Ak je požiadavka neplatná server vracia chybu, ak platná, server vráti vhodnú odpoveď.<br/>
Server taktiež podporuje komunikáciu pomocou nástrojov wget a curl. <br/>

## Použitie
Makefile po zavolaní príkazu make vytvorí súbor hinfosvc<br/>
Následne môžme spustiť program pomocou príkazu  : ./hinfosvc PORT_NR, kde PORT_NR je číslo portu.<br/>
Pre ukončenie servera je nutné použiť komináciu Ctrl + c. <br/>

# Príklady

Príklady boli testované na serveri merlin<br/>

```
GET http://servername:port/hostname
```
Vracia sieťové meno počítača včetne domény :<br/>
merlin.fit.vutbr.cz <br/>

```
GET http://servername:port/cpu-name
```
Vracia informácie o procesore :<br/>
model name	: Intel(R) Xeon(R) Silver 4214R CPU @ 2.40GHz<br/>

```
GET http://servername:12345/load
```
Vracia aktuálnu záťaž procesora :<br/>
4.237288%