#!/bin/sh

export PATH="/usr/local/cs/bin:$PATH"

run_peer () {
    while true
    do
	./osppeer $@
	echo "*** OSPPEER $@ EXIT($?)! Restarting..."
    done
}

for i in {1..2}
do
    run_peer -dbase &
    run_peer -dbase -s &
done

# good
for i in {1..2}
do
    run_peer -dbase -t164.67.100.231:12997 &
done

# slow
for i in {1..2}
do
    run_peer -dbase -t164.67.100.231:12998 -s &
done

# evil
for i in {1..2}
do
    run_peer -dbase -t164.67.100.231:12999 -b &
done

# popular
for i in {1..2}
do
    run_peer -dbase -t164.67.100.231:13000 &
done

wait
