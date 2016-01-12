#! /bin/sh

# UCLA CS 111 Lab 1 - Test that valid syntax is processed correctly.

tmp=$0-$$.tmp
mkdir "$tmp" || exit

(
cd "$tmp" || exit

cat >test.sh <<'EOF'
#1.while command
while echo condition
do echo loopbody done

#2.until command
until echo condition
do echo loopbody done

#3.for command
for a in 1 2 3
do 
echo $a
done

#4.if command
if echo a
then echo pass condition
else echo notpass condition
fi

#5. if command with not command
if (!(echo a))
then echo pass condition
else echo notpass condition
fi

#6.if nested in while 
while echo condition
do echo b
if echo if_condition
then echo then_condition
else echo else_condition
fi
done

#7.until nested in if 
if echo condition
then
until echo until_condition
do
echo a
done
else
echo b
fi

#8. not command
!(echo a || echo b)
EOF

cat >test.exp <<'EOF'
# 1
  while
    echo condition
  do
    echo loopbody
  done
# 2
  until
    echo condition
  do
    echo loopbody
  done
# 3
  for
    a in 1 2 3
  do
    echo $a
  done
# 4
  if
    echo a
  then
    echo pass condition
  else
    echo notpass condition
  fi
# 5
  if
    (
     !
       (
        echo a
       )
    )
  then
    echo pass condition
  else
    echo notpass condition
  fi
# 6
  while
    echo condition
  do
      echo b \
    ;
      if
        echo if_condition
      then
        echo then_condition
      else
        echo else_condition
      fi
  done
# 7
  if
    echo condition
  then
    until
      echo until_condition
    do
      echo a
    done
  else
    echo b
  fi
# 8
  !
    (
       echo a \
     ||
       echo b
    )
EOF

../timetrash -p test.sh >test.out 2>test.err || exit

diff -u test.exp test.out || exit
test ! -s test.err || {
  cat test.err
  exit 1
}

) || exit

rm -fr "$tmp"
