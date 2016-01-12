#! /usr/bin/perl -w

open(FOO, "ospfsmod.c") || die "Did you delete ospfsmod.c?";
$lines = 0;
$lines++ while defined($_ = <FOO>);
close FOO;

@tests = (
    # 1 test reading
    [ 'diff base/hello.txt test/hello.txt >/dev/null 2>&1 && echo $?',
      "0"
    ],
    # 2
    [ 'cmp base/pokercats.gif test/pokercats.gif >/dev/null 2>&1 && echo $?',
      "0"
    ],
    # 3    
    [ 'ls -l test/pokercats.gif | awk "{ print \$5 }"',
      "91308"
    ],

    # 4 test writing
    # We use dd to write because it doesn't initially truncate, and it can
    # be told to seek forward to a particular point in the disk.
    [ "echo Bybye | dd bs=1 count=5 of=test/hello.txt conv=notrunc >/dev/null 2>&1 ; cat test/hello.txt",
      "Bybye, world!"
    ],
    # 5
    [ "echo Hello | dd bs=1 count=5 of=test/hello.txt conv=notrunc >/dev/null 2>&1 ; cat test/hello.txt",
      "Hello, world!"
    ],
    # 6
    [ "echo gi | dd bs=1 count=2 seek=7 of=test/hello.txt conv=notrunc >/dev/null 2>&1 ; cat test/hello.txt",
      "Hello, girld!"
    ],
    # 7
    [ "echo worlds galore | dd bs=1 count=13 seek=7 of=test/hello.txt conv=notrunc >/dev/null 2>&1 ; cat test/hello.txt",
      "Hello, worlds galore"
    ],
    # 8
    [ "echo 'Hello, world!' > test/hello.txt ; cat test/hello.txt",
      "Hello, world!"
    ],
    
    # 9 create a file
    [ 'touch test/file1 && echo $?',
      "0"
    ],

    # 10 read directory
    [ 'touch test/dir-contents.txt ; ls test | tee test/dir-contents.txt | grep file1',
      'file1'
    ],

    # 11 write files, remove them, then read dir again
    [ 'ls test | dd bs=1 of=test/dir-contents.txt >/dev/null 2>&1; ' .
      ' touch test/foo test/bar test/baz && '.
      ' rm    test/foo test/bar test/baz && '.
      'diff <( ls test ) test/dir-contents.txt',
      ''
    ],

    # 12 remove the last file
    [ 'rm -f test/dir-contents.txt && ls test | grep dir-contents.txt',
      ''
    ],


    # 13 write to a file
    [ 'echo hello > test/file1 && cat test/file1',
      'hello'
    ],
    
    # 14  append to a file
    [ 'echo hello > test/file1 ; echo goodbye >> test/file1 && cat test/file1',
      'hello goodbye'
    ],

    # 15 delete a file
    [ 'rm -f test/file1 && ls test | grep file1',
      ''
    ],

    # 16 make a larger file for indirect blocks
    [ 'yes | head -n 5632 > test/yes.txt && ls -l test/yes.txt | awk \'{ print $5 }\'',
      '11264'
    ],
   
    # 17 truncate the large file
    [ 'echo truncernated11 > test/yes.txt | ls -l test/yes.txt | awk \'{ print $5 }\' ; rm test/yes.txt',
      '15'
    ],
    # 18: overwriting files should affect all hardlinks to that file
  [ 'echo hello > test/hardlinktest ; ln test/hardlinktest test/hardlinktest2 ; echo goodbye > test/hardlinktest2 ; cat test/hardlinktest ; rm test/hardlinktest2',
    'goodbye'
  ],
  
  # 19: removing one hardlink should not remove the file itself
  [ 'echo hello > test/filetoremove ; ln test/filetoremove test/filetokeep ; echo goodbye > test/filetoremove ; rm test/filetoremove ; cat test/filetokeep ; rm test/filetokeep',
    'goodbye'
  ],

  # 20: symlinks
  [ 'ln -s test/hello.txt thelink ; echo "World" >> test/hello.txt ; diff test/hello.txt thelink && echo Same contents ; rm thelink',
    'Same contents'
  ],

  # 21: conditional symlinks as root
  [ 'echo "Not root" > test/notroot.txt ; echo "Root" > test/root.txt ; ln -s root?test/root.txt:test/notroot.txt amiroot ; diff test/root.txt amiroot && echo Same contents; rm amiroot',
    'Same contents'
  ],

  # 22: conditional symlinks as non-root
  [ 'su user -c "cat amiroot ; rm amiroot root notroot"',
    'Not root'
  ],

);

my($ntest) = 0;
my(@wanttests);

foreach $i (@ARGV) {
    $wanttests[$i] = 1 if (int($i) == $i && $i > 0 && $i <= @tests);
}

my($sh) = "bash";
my($tempfile) = "lab3test.txt";
my($ntestfailed) = 0;
my($ntestdone) = 0;

foreach $test (@tests) {
    $ntest++;
    next if (@wanttests && !$wanttests[$ntest]);
    $ntestdone++;
    print STDOUT "Running test $ntest\n";
    my($in, $want) = @$test;
    open(F, ">$tempfile") || die;
    print F $in, "\n";
    print STDERR "  ", $in, "\n";
    close(F);
    $result = `$sh < $tempfile 2>&1`;
    $result =~ s|\[\d+\]||g;
    $result =~ s|^\s+||g;
    $result =~ s|\s+| |g;
    $result =~ s|\s+$||;

    next if $result eq $want;
    next if $want eq 'Syntax error [NULL]' && $result eq '[NULL]';
    next if $result eq $want;
    print STDERR "Test $ntest FAILED!\n  input was \"$in\"\n  expected output like \"$want\"\n  got \"$result\"\n";
    $ntestfailed += 1;
}

unlink($tempfile);
my($ntestpassed) = $ntestdone - $ntestfailed;
print "$ntestpassed of $ntestdone tests passed\n";
exit(0);
