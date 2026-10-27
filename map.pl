#!/usr/bin/perl
use warnings;
use strict;

my $usage = "usage: $0 [width] [height] [density%] [empty] [obstacle] [full]\n";
my ($x, $y, $density) = (80, 40, 7);
my ($e, $o, $f) = ('.', 'o', 'x');
my (@num, @chr);
foreach my $arg (@ARGV) {
	print $usage and exit 0 if ($arg eq '-h');
	if ($arg =~ /^\d+$/) {
		push @num, $arg;
	}
	elsif ($arg =~ /^[ -~]$/) {
		push @chr, $arg;
	}
	else {
		die "'$arg' is neither a number nor a printable character\n$usage";
	}
}
die "too many numbers or characters\n$usage" if (@num > 3 || @chr > 3);
$density = $num[0] if (@num == 1);
($x, $y, $density) = (@num, $density) if (@num >= 2);
$f = $chr[0] if (@chr == 1);
($e, $o) = @chr if (@chr >= 2);
$f = $chr[2] if (@chr == 3);
die "width and height must be at least 1\n$usage" if ($x < 1 || $y < 1);
die "density must be between 0 and 100\n$usage" if ($density > 100);
die "characters must all be different\n$usage" if ($e eq $o || $e eq $f || $o eq $f);

print "$y$e$o$f\n";
for (my $i = 0; $i < $y; $i++) {
	for (my $j = 0; $j < $x; $j++) {
		if (rand(100) < $density) {
			print $o;
		}
		else {
			print $e;
		}
	}
	print "\n";
}
