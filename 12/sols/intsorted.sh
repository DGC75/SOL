#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: ./insorted.sh cartella_input file_output" 
    exit 0
fi

tmpfile=/tmp/lines_unsorted

if [ -e $tmpfile ]; then rm $tmpfile; fi # rimuovo il file temporaneo se gia' esiste

for f in $(ls $1/*.dat); do  # itera su tutti i file .dat nella cartella $1
        # tr -s " " : comprime gli spazi duplicati in uno solo per far funzionare bene cut
        # cut -d" " -f2 : seleziona la seconda parola
        # cut -d"." -f1 : seleziona la parte prima del . (o tutto se non appare .)
        cat $f | tr -s " " | cut -d" " -f2 | cut -d"." -f1 >> $tmpfile
done

sort -gu $tmpfile > $2 # sort numerico e con rimozione duplicati
