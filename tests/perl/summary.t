use strict;
use warnings;
use Test::More tests => 1;
use Test::Util qw(check_help);

my $script_name = "summary";

my @options = (
    [qw(directory=str)],
    [qw(end-date=num)],
    [qw(output-directory=str)],
    [qw(peer-dist-limit=float)],
    [qw(skip-time-steps=float)],
    [qw(start-date=num)],
);

check_help("./$script_name", @options);
