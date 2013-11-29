use strict;
use warnings;
use Test::More tests => 1;
use Test::Util qw(check_help);

my $script_name = "plot_summary";

my @options = (
    [qw(directory=str)],
    [qw(identifier=str)],
    [qw(output-file=str)],
    [qw(plot-term=str)],
    [qw(dont-wait)],
    [qw(offset-limit=float)],
);

check_help("./$script_name", @options);
