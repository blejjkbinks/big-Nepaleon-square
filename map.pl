#!/usr/bin/perl
use warnings;
use strict;

my $def_x = 80;
my $def_y = 40;
my $scale = 20;
my $def_dens = int(($def_x + $def_y) / $scale);
my $def_emp = '.';
my $def_obs = 'o';
my $def_sqr = 'x';

my @dim = ();
my @chr = ();
my $dim_c = 0;
my $chr_c = 0;

my ($x, $y, $dens, $emp, $obs, $sqr);

foreach my $arg (@ARGV)
{
	if (length($arg) == 1) 
	{
		push @chr, $arg;
		$chr_c++;
	}
	elsif ($arg =~ /^\d+$/ || $arg =~ /^-\d+$/)
	{
		push @dim, abs($arg);
		$dim_c++;
	}
}

if ($dim_c == 0 || $dim_c == 1 || $dim_c > 3)
{
	$x = $def_x;
	$y = $def_y;
	$dens = ($dim_c == 1) ? $dim[0] : $def_dens;
}
elsif ($dim_c == 2)
{
	($x, $y) = @dim;
	$dens = (($x + $y) < $scale) ? 1 : int(($x + $y) / $scale);
} 
elsif ($dim_c == 3)
{
	($x, $y, $dens) = @dim;
}

if ($chr_c == 0 || $chr_c == 1 || $chr_c > 3)
{
	$emp = $def_emp;
	$obs = $def_obs;
	$sqr = ($chr_c == 1) ? $chr[0] : $def_sqr;
}
elsif ($chr_c == 2)
{
	($emp, $obs) = @chr;
	$sqr = $def_sqr;
}
elsif ($chr_c == 3)
{
	($emp, $obs, $sqr) = @chr;
}

print "$y$emp$obs$sqr\n";
for (my $i = 0; $i < $y; $i++)
{
	for (my $j = 0; $j < $x; $j++)
	{
		if (int(rand($y) * 2) < $dens)
		{
			print "$obs";
		}
		else
		{
			print "$emp";
		}
	}
	print "\n";
}
