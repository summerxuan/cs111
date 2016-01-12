/usr/local/cs/bin/osptracker -p12997&
/usr/local/cs/bin/osptracker -p12998&
/usr/local/cs/bin/osptracker -p12999&
/usr/local/cs/bin/osptracker -p13000 -f&

./osppeer -dbase -t164.67.100.231:12997&
./osppeer -dbase -t164.67.100.231:12998 -s &
./osppeerbad -dbase -t164.67.100.231:12999 -b &
./osppeer -dbase -t164.67.100.231:13000 &
