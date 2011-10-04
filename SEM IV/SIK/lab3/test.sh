for i in `seq $2`
do
	./client $1 "jakies id $i" "jakas wiadomosc $i" &
done
