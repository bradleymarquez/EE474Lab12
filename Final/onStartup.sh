#!/bin/sh


# Carry out different functions when asked to by the system
case "$1" in 
start)
echo "Starting hBridge Program"
# run application you want to start
sudo /home/Documents/EE474Lab12/Final/hBridge &
;;
stop)
echo "Killing hBridge Program"
# kills the application you want to stop
sudo killall hBridge
;;
*)
echo "Usage: /etc/init.d/my_settings {start | stop}"
exit 1  
;;
esac

exit 0



