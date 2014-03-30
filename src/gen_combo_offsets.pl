use strict;

my $animation_tics = 80;
my ($min_y, $max_y) = (250, -50);

print "static const int offset[ANIMATION_TICS] = { ";

for my $tic (0 .. $animation_tics - 1) {
	my $t = $tic/$animation_tics;

	my $f;

	if ($t < .5) {
		$f = .5 - .5*(1. - 2.*$t)*(1. - 2.*$t);
	} else {
		$f = .5 + .5*(2.*($t - .5))*(2.*($t - .5));
	}

	my $y = int($min_y + $f*($max_y - $min_y));

	print "$y, ";
}

print "};\n";
