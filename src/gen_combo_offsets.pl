use strict;

my $animation_tics = 80;
my ($min_y, $max_y) = (250, -50);

print "static const int offset[ANIMATION_TICS] = { ";

for my $tic (0 .. $animation_tics - 1) {
	print "\n" if $tic%8 == 0;
	my $t = $tic/$animation_tics;
	my $f = .5 + .5*(2.*$t - 1.)**3;
	my $y = int($min_y + $f*($max_y - $min_y));

	print "$y, ";
}

print "};\n";
