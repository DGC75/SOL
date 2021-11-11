#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Nessun file .dat passato come argomento" 
    exit 0
fi
for f ; do  # itero su tutti gli argomenti
    sum=0   # conterra' la somma degli elementi 
    std=0   # standard deviation
    cnt=0   # contatore 
    values=() # array dei valori
    exec 3<$f   # apro il file $f in lettura e gli assegno il descrittore 3 
    while IFS=" " read -u 3 line; do #leggo riga per riga
        elem=$(echo $line | tr -s " " | cut -d" " -f 2) #toglo spazi duplicati e prendo il secondo campo
    	sum=$(echo "scale=2; $sum+$elem" | bc -lq)  #sommo i due numeri con bc
    	values[$cnt]=$elem
    	(( cnt += 1))
    done
    exec 3<&-  # chiudo il descrittore 3 
    avg=$(echo "scale=2; $sum/$cnt" | bc -lq)
    sum=0 
    for((i=0;i<$cnt; i++)); do
	   sum=$(echo "scale=2; $sum+(${values[$i]}-$avg)^2" | bc -lq) #scarto quadratico medio con bc
    done
    if [ $cnt -gt 1 ]; then
	   std=$(echo "scale=2; sqrt($sum/($cnt-1))" | bc -lq)
    fi
    f1=${f%.*}   # tolgo l'estensione
    f2=$(basename $f1) 

    echo -e $f2 "\t" $cnt "\t" $avg "\t" $std # l'opzione -e ci consente di usare simboli speciali come \t (tab)

    # In alternativa, si poteva utilizzare awk per avere un output formattato sfruttando il suo printf
    # echo "$f2 $cnt $avg $std"| LC_ALL="C" awk '{printf "%-12s %-3d %8.2f %6.2f\n", $1, $2, $3, $4}'
done
