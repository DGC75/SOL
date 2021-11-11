#!/bin/bash             

if [ $# -ne 1 ]; then            # se il numero di argomenti non e' 1
    echo use: $(basename $0) nomedirectory    # stampo il comando d'uso (nomescript nomedirectory)
    exit -1
fi

dir=$1               
if [ ! -d $dir ]; then       # se $dir non e' una directory, stampo un errore
    echo "L'argomento $dir non e' una directory"
    exit 1;   
fi

bdir=$(basename $dir)        
if [ -w $bdir.tar.gz ]; then    # se il file esiste ed e' scrivibile
    echo -n "il file $bdir.tar.gz esiste gia' sovrascriverlo (S/N)? "
    read yn                      # leggo un carattere dallo standard input
    if [ x$yn != x"S" ]; then    # se il carattere letto non e' "S" esco
	exit 0;
    fi
    rm -f $bdir.tar.gz          # cancello il vecchio file esistente
fi
echo "creo l'archivio $bdir.tar.gz"

tar cf $bdir.tar $dir 2>> ./errori.txt     # appende l'output dello std-error in error.txt
if [ $? -ne 0 ]; then            # controllo che il comando sia andato a buon fine
    echo 
    echo "Errore nella creazione dell'archivio"
    exit 1
fi
gzip $bdir.tar 2>> ./errori.txt     # appende l’output dello std-error nel file error.txt
if [ $? -ne 0 ]; then               # controllo che il comando sia andato a buon fine
    echo
    echo "Errore nella compressione dell'archivio"
    exit 1
fi

echo "archivio creato con successo, il contenuto e':"
tar tzvf $bdir.tar.gz 2>&1             # redirige lo std error sullo std output

exit 0
