#! /bin/bash

while [ ! -e /dev/ttyACM0 ]
do
	echo "no ttyACM0"
	sleep 1
done
sudo chmod 666 /dev/ttyACM0
sleep 1
date +"%s" >/dev/ttyACM0
