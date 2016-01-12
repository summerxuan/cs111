#! /bin/sh

# UCLA CS 111 Lab 1b - Test that valid syntax is processed correctly.

tmp=$0-$$.tmp
mkdir "$tmp" || exit

(
cd "$tmp" || exit

cat >test.sh <<'EOF'
echo a
false && echo b
false || echo c
echo a || echo b
echo f | cat
echo writing to file > f1
cat<f1
echo i ;echo j; echo k
(echo a )||(echo b)
EOF

cat >test.exp <<'EOF'
a
c
a
f
writing to file
i
j
k
a
EOF

../timetrash -t test.sh >test.out 2>test.err || exit

diff -u test.exp test.out || exit
test ! -s test.err || {
  cat test.err
  exit 1
}

) || exit

rm -fr "$tmp"
