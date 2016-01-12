#! /usr/bin/perl -w

open(FOO, "osprd.c") || die "Did you delete osprd.c?";
$lines = 0;
$lines++ while defined($_ = <FOO>);
close FOO;

@tests = (
# single write

    # 1
    [ '(echo test1 | ./osprdaccess -w) && ' .
      '(./osprdaccess -r 16 | hexdump -C)',
      "00000000 74 65 73 74 31 0a 00 00 00 00 00 00 00 00 00 00 |test1...........| " .
      "00000010" ],
     # 2
    [ '(echo aa | ./osprdaccess -w 2 -l -d 1) & ' .
      'sleep 0.5 ; (echo b | ./osprdaccess -w 1 -o 1 -l) ; ' .
      './osprdaccess -r 2',
      "ab"
    ],
    # 3
    [ 'echo a | ./osprdaccess -w 1 ; ' .
      '(./osprdaccess -r -l -d 3 >/dev/null &) ; ' .
      '(echo b | ./osprdaccess -w -l 1 &) ; ' .
      'sleep 1 ; ./osprdaccess -r 1',
      "a"
    ],

    # 4
    [ '(./osprdaccess -r 6 -l 1) &' .
      '(echo foobar | ./osprdaccess -w -l) ',
      "foobar"
    ],
    
    #16 A simple deadlock
    [
      'echo ababab | ./osprdaccess -w -l /dev/osprda /dev/osprda ' .
      './osprdaccess -r /dev/osprda',
      "ioctl OSPRDIOCACQUIRE: Resource deadlock avoided"
    ],
    #6 A slightly more complex deadlock
    [
      'echo test1 | ./osprdaccess -w -l -d 0.1 /dev/osprda /dev/osprdb & ' .
      'echo test2 | ./osprdaccess -w -l -d 0.1 /dev/osprdb /dev/osprda ',
      "ioctl OSPRDIOCACQUIRE: Resource deadlock avoided"
    ],
    #20 A more elaborate deadlock
    [
      '(echo test1 | ./osprdaccess -w -l -d 0.1 /dev/osprda /dev/osprdb ) & ' .
      '(echo test2 | ./osprdaccess -w -l -d 0.1 /dev/osprdb /dev/osprdc ) & ' .
      '(echo test3 | ./osprdaccess -w -l -d 0.1 /dev/osprdc /dev/osprda ) & ' ,
      "ioctl OSPRDIOCACQUIRE: Resource deadlock avoided"
    ]
   
    );

my($ntest) = 0;

my($sh) = "bash";
my($tempfile) = "lab2test.txt";
my($ntestfailed) = 0;
my($ntestdone) = 0;
my($zerodiskcmd) = "./osprdaccess -w -z";
my(@disks) = ("/dev/osprda", "/dev/osprdb", "/dev/osprdc", "/dev/osprdd");

my(@testarr, $anytests);
foreach $arg (@ARGV) {
    if ($arg =~ /^\d+$/) {
	$anytests = 1;
	$testarr[$arg] = 1;
    }
}

foreach $test (@tests) {

    $ntest++;
    next if $anytests && !$testarr[$ntest];

    # clean up the disk for the next test
    foreach $disk (@disks) {
	`$sh <<< "$zerodiskcmd $disk"`
    }

    $ntestdone++;
    print STDOUT "Starting test $ntest\n";
    my($in, $want) = @$test;
    open(F, ">$tempfile") || die;
    print F $in, "\n";
    print STDERR $in, "\n";
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
    $ntestfailed++;
}

unlink($tempfile);
my($ntestpassed) = $ntestdone - $ntestfailed;
print "$ntestpassed of $ntestdone tests passed\n";
exit(0);
