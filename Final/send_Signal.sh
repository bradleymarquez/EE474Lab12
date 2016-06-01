#!/bin/bash
gcc -std=c11 -o sig2 signal_send_test.c
target_PID="$(pgrep -f signal_receive_test)"
./sig2 $target_PID
