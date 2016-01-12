#! /bin/sh

# UCLA CS 111 Lab 1b - Test that valid syntax is processed correctly.

tmp=$0-$$.tmp
mkdir "$tmp" || exit

(
cd "$tmp" || exit

cat >test.sh <<'EOF'
for a in 1 2 3 
do 
echo $a
done

if echo a
then echo pass condition
else echo notpass condition
fi

if (!(echo a))
then echo pass condition
else echo notpass condition
fi
EOF

cat >test.exp <<'EOF'
1
2
3
a
pass condition
a
notpass condition
EOF

../timetrash test.sh >test.out 2>test.err || exit

diff -u test.exp test.out || exit
test ! -s test.err || {
  cat test.err
  exit 1
}

) || exit

rm -fr "$tmp"
