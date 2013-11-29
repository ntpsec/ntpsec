use strict;
use warnings;
use Test::More tests => 21;
use Test::Util qw(check_help override);
use Test::Command;  
use Test::Output;

my $script_name = "calc_tickadj/calc_tickadj";

my @options = (
    [qw(drift-file=str d)],
    [qw(tick=num t)]
);

check_help($script_name, @options);

require_ok("./$script_name");

BEGIN {
    my ($tick, $drift, $drift_file, $open_fail);

    override('calc_tickadj', open => sub (*;@) {
            return if $open_fail;
            is($_[1], $drift_file) if defined $drift_file;
            CORE::open $_[0], '<', \$drift;
        });

    override('calc_tickadj', readpipe => sub {
            return $tick;
        });

    sub run_calc_tickadj {
        my %opts = @_;
        $open_fail  = delete $opts{fail_open} || 0;
        $drift      = delete $opts{drift} || '0.00';
        $tick       = delete $opts{tick} || '';
        $drift_file = delete $opts{drift_file};
        calc_tickadj::run(@{ delete $opts{opts} || [] })
    }
}

my $ret;

# Test tick detection
for (
    'tick = 10000',
    'PRESET tick = 10000',
    'KERNEL tick = 10000',
    '  tick  =  10000',
) {
    stdout_like( sub { $ret = run_calc_tickadj(tick => $_) }, qr/10000 usec; 10000000 nsec/ );
    ok(!$ret);
}

# Test how the script copes if tickadj is not found/outputs invalid data
for (undef, "Invalid stuff") {
    eval {
        run_calc_tickadj(tick => $_);
    } or do {
        chomp $@; 
        is($@, 'Could not get tick value, try manually with -t/--tick') 
    };

    # Still invalid data but pass tick as argument
    stdout_like( sub { $ret = run_calc_tickadj(tick => $_, opts => [qw(-t 9999)]) }, qr/9999 usec; 9999000 nsec/);
    ok(!$ret);
}

# Test -d switch
stdout_like(
    sub { $ret = run_calc_tickadj(drift_file => 'foobar', drift => '14.00', tick => 'tick = 10000', opts => [qw(-d foobar)]) },
    qr/14.000 \(drift\)/
);
ok(!$ret);

# Open on drift file fails
eval {
    run_calc_tickadj(fail_open => 1, opts => [qw(-t 10000)]);
} or do {
    chomp $@; 
    like($@, qr(Could not open))
};

# Open succeeds but drift file is invalid
eval {
    run_calc_tickadj(drift => 'NOTVALIDDRIFTVALUE', opts => [qw(-t 10000)])
} or do {
    chomp $@; 
    like($@, qr(Invalid drift file value <NOTVALIDDRIFTVALUE>));
};
