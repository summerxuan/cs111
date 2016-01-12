#! /bin/sh

# UCLA CS 111 Lab 1 - Test that valid syntax is processed correctly.

tmp=$0-$$.tmp
mkdir "$tmp" || exit

(
cd "$tmp" || exit

cat >test.sh <<'EOF'
cat < f1
echo a > f1
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
cat < f1
echo a
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo a
echo a
echo b
echo b
echo b
echo b
echo c
echo b
echo j
echo k
echo k
echo k
echo l
echo i
echo k
echo 2
echo a && echo b
echo k || echo k
ls | cat
echo a; echo b; echo c
(echo a) || (echo b)
false || echo b
false && echo a
echo aa> a
echo bb >b
echo dd>d
 sort < a | cat b - | tr A-Z a-z > c
  sort -k2 d - < a | uniq -c > e
  diff a c > f

EOF

cat >test.exp <<'EOF'
a
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
a
a
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
a
a
b
b
b
b
c
b
j
k
k
k
l
i
k
2
a
b
k
a
alloc.c
alloc.h
alloc.o
b
c
check-dist
command.h
command-internals_1221.h
command-internals_1440.h
command-internals_1719.h
command-internals.h
command-internals_with_two.h
d
diff
e
execute-command_1251.c
execute-command_1434.c
execute-command_1440.c
execute-command_1502.c
execute-command_1517.c
execute-command_1547.c
execute-command_1621.c
execute-command_1736.c
execute-command.c
execute-command.o
f
f1
lab1-xiaoxuan
lab1-xiaoxuan.tar.gz
main1447.c
main1719.c
main2041.c
#main.c#
main.c
main.o
Makefile
out
print-command.c
print-command.o
read-command_1221.c
read-command_1440.c
read-command_1719.c
read-command_1808.c
read-command_1908.c
read-command-1c.c
read-command_2039.c
read-command.c
read-command-final.c
read-command.o
read-command_with_two.c
README
#test2.sh#
test-lab1b.sh
test-p-bad.sh
test-p-ok.sh
timetrash
a
b
c
a
b

EOF

../timetrash -t test.sh >test.out 2>test.err || exit

diff -u test.exp test.out || exit
test ! -s test.err || {
  cat test.err
  exit 1
}

) || exit

rm -fr "$tmp"
