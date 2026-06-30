Popis projektu
Cílem projektu je implementovat program, který provede shlukovou analýzu nad síťovými datovými toky pomocí metody nejbližšího souseda (single linkage). Program bude pracovat s daty popisujícími komunikaci mezi dvěma počítači a na základě uživatelem zadaných vah rysů toků bude slučovat nejbližší shluky, dokud nedosáhne požadovaného počtu shluků.

Shluková analýza v tomto projektu slouží jako prostředek pro procvičení práce s datovými strukturami, dynamickou pamětí, načítáním dat ze souboru a implementací jednoduchého algoritmu.

Vstupní data
Vstupní soubor obsahuje statistické údaje o záznamu komunikace (tzv. údaje o síťových tocích) mezi dvěma počítači v síti. Každý tok je popsán jedním řádkem se strukturovanými hodnotami:

flow_id – jedinečný celočíselný identifikátor toku
src_ip – IP adresa zdroje (IP adresa je čtyřčíslí v rozsahu 0-255 oddělené tečkami)
dst_ip – IP adresa cíle
total_bytes – celkově přenesená data (bajty)
flow_duration – celková doba trvání toku (sekundy)
packet_count – počet přenesených paketů
avg_interarrival_time – průměrná doba mezi pakety (sekundy, může mít i desetinnou část)
Z těchto údajů budete používat čtyři rysy:

b – celkově přenesená data (total_bytes)
t – doba trvání toku (flow_duration)
d – průměrná doba mezi pakety (avg_interarrival_time)
s – průměrná délka paketu (nutno vypočítat)
Vzdálenost dvou toků musíte vypočítat jako váženou Euklidovskou vzdálenost nad hodnotami (b, t, d, s) a váhami zadanými uživatelem.

Metoda nejbližšího souseda
Na začátku tvoří každý tok samostatný shluk. Algoritmus v každém kroku:
    1. najde dvě skupiny s nejmenší vzájemnou vzdáleností,
    2. sloučí je do jednoho shluku,
    3. pokračuje, dokud nedosáhne cílového počtu shluků.
Vzdáleností dvou shluků se rozumí minimální vzdálenost libovolné dvojice toků, každý z jiného shluku.

Formát vstupního souboru
První řádek:

count=X
kde X je počet toků v souboru.
Každý následující řádek má formát:

FLOWID SRC_IP DST_IP TOTAL_BYTES FLOW_DURATION PACKET_COUNT AVG_INTERARRIVAL
Odevzdání
Odevzdejte jediný zdrojový soubor v jazyce C s názvem flows.c . Soubor odevzdejte prostřednictvím informačního systému.

Překlad a spuštění
Program bude překládán příkazem:

cc -std=c11 -Wall -Wextra -Werror -pedantic flows.c -o flows -lm
Syntax spuštění programu:

./flows SOUBOR [N WB WT WD WS]
    SOUBOR – vstupní soubor s definicí toků
    N – požadovaný počet výsledných shluků (N > 0)
    WB WT WD WS – váhy jednotlivých rysů (nezáporná reálná čísla)
    WB – váha pro total_bytes
    WT – váha pro flow_duration
    WD – váha pro avg_interarrival_time
    WS – váha pro průměrnou délku paketu
Všechny váhy jsou povinné, pokud je zadáno N. Pokud N zadané není, program shlukovací analýzu provádět nebude a vypíše shluky o jednom toku.

Program by měl být přenositelný na různé platformy. Doporučujeme ho vyzkoušet nejen na vlastním počítači, ale také na školních serverech merlin a eva.

Výstup programu
Program vypíše výsledné shluky ve formátu:

Clusters:
cluster 0: FLOWID1 FLOWID2 ...
cluster 1: FLOWID3 FLOWID4 ...
...
Identifikátory toků ve shluku budou seřazeny od nejmenšího čísla po největší. Shluky budou seřazeny dle nejmenšího identifikátoru toku ve shluku.

Využití AI při práci na projektu
Studentům je zakázáno generovat zdrojový kód pomocí AI nebo jej z AI opisovat. AI však můžete použít jako asistenta při pochopení zadání a návrhu řešení. Doporučené oblasti, na které se můžete AI zeptat:

Vysvětlit váženou Euklidovskou vzdálenost.
Jak postupně rozdělit projekt na menší dílčí úlohy.
Jaké základní kroky musí provádět algoritmus single linkage.
Jakým způsobem organizovat práci s dynamickou pamětí.
Váš prompt by mohl začínat: "Nesmíš generovat nebo doplnit kód. Můžeš však pomoci porozumět problému, navrhnout postup nebo vysvětlit algoritmy. Vystupuješ v roli pomocníka / učitele."

Naučte se využívat AI/LLM jako konzultanta, nikoli jako generátor hotových řešení.

Ukázkový vstup a výstup
Soubor data.txt:

count=4
10 192.168.1.1 192.168.1.2 2000 10 20 0.05
11 10.0.0.5    10.0.0.6    4000 20 30 0.07
12 192.168.2.1 192.168.2.3 1500 8  10 0.03
13 172.16.0.10 172.16.0.11 6000 25 45 0.06
Příklad spuštění:

$ ./flows data.txt 2 1.0 1.0 1.0 1.0
Výstup:

Clusters:
cluster 0: 10 12
cluster 1: 11 13
Pomocný testovací skript
Základní funkcionalitu programu můžete ověřit pomocí testovacího skriptu test.sh. Skript má sloužit jako reference použití programu a jeho výsledky ještě nezaručují dostatečné bodové hodnocení. Použití skriptu:


$ ls
flows.c
test.sh

$ sh test.sh
test 1: ✔ cteni a tisk clusteru
test 2: ✔ cteni a serazeny tisk clusteru
test 3: ✔ cteni a serazeny tisk celeho shluku
test 4: ✔ cteni a serazeny tisk 2 shluku, priklad ze zadani
test 5: ✔ cteni a serazeny tisk 2 shluku, float vahy
test 6: ✔ cteni a serazeny tisk 3 shluku, vaha na WS
