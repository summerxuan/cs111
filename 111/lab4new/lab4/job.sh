#!/bin/bash

while true; do
sh ./kill-osptracker-osppeer.sh
sleep 3
sh ./setup-trackers.sh
sleep 3
sh ./setup-peers.sh
#sleep 2
#sh ./setup-peers.sh
sleep 3
sh ./good-peers.sh
sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
#sh ./good-peers.sh
sleep 2
sh ./bad-peers.sh
sh ./bad-peers.sh
#sh ./bad-peers.sh
#sh ./bad-peers.sh
sleep 60
done 
