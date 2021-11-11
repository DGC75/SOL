#!/bin/bash


if [ $# -lt 1 ]; then
    echo "Usage: ./capitalize.sh text_file" 
    exit 0
fi

SERVER_CMD=./toup_server
CLIENT_CMD=./toup_client
out=$(basename -s .txt $1)-capital.txt
# ALTERNATIVO per qualsiasi estensione: 
# out=${1%.*}-capital.txt

make

if [ -e out ]; then rm out; fi

$SERVER_CMD & # avvio il server in background

for word in $(cat $1); do
	$CLIENT_CMD $word >> $out #invoco il client e salvo il risultato nel file $out
done

kill $(pidof $SERVER_CMD) # termino il server
