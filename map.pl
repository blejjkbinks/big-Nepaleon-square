#!/usr/bin/perl
use warnings;
use strict;

# usage: ./map.pl [width] [height] [density%] [empty] [obstacle] [full]
#   numbers: 1 = density, 2 = width height, 3 = width height density
#   chars:   1 = full,    2 = empty obstacle, 3 = empty obstacle full

my ($x, $y, $pct) = (80, 40, 7);
my ($emp, $obs, $sqr) = ('.', 'o', 'x');

sub usage
{
	my ($err) = @_;
	print STDERR "error: $err\n" if $err;
	print STDERR "usage: $0 [width] [height] [density%] [empty] [obstacle] [full]\n";
	exit($err ? 1 : 0);
}

my (@num, @chr);
foreach my $arg (@ARGV)
{
	usage() if $arg eq '-h' || $arg eq '--help';
	if ($arg =~ /^\d+$/)
	{
		push @num, $arg;
	}
	elsif ($arg =~ /^[ -~]$/)
	{
		push @chr, $arg;
	}
	else
	{
		usage("'$arg' is neither a number nor a printable character");
	}
}
usage("too many numbers") if @num > 3;
usage("too many characters") if @chr > 3;

if (@num == 1)
{
	$pct = $num[0];
}
elsif (@num >= 2)
{
	($x, $y) = @num;
	$pct = $num[2] if @num == 3;
}

if (@chr == 1)
{
	$sqr = $chr[0];
}
elsif (@chr >= 2)
{
	($emp, $obs) = @chr;
	$sqr = $chr[2] if @chr == 3;
}

usage("width and height must be at least 1") if $x < 1 || $y < 1;
usage("density must be between 0 and 100") if $pct > 100;
usage("characters must all be different")
	if $emp eq $obs || $emp eq $sqr || $obs eq $sqr;

print "$y$emp$obs$sqr\n";
for (my $i = 0; $i < $y; $i++)
{
	my $line = '';
	for (my $j = 0; $j < $x; $j++)
	{
		$line .= (rand(100) < $pct) ? $obs : $emp;
	}
	print "$line\n";
}
