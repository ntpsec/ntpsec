use strict;
use warnings;
use Test::More tests => 22;
use Test::Command;
use Test::Output;
use Test::Util qw(check_help mock_ntp_util run_mocked override);

my $script_name = 'ntp-wait/ntp-wait';

require_ok($script_name);

my @options = (
    [qw(tries=num n)], 
    [qw(sleep=num s)],
    [qw(verbose v)],
);
check_help("./$script_name", @options);

mock_ntp_util('ntp_wait', read_vars => 1);

{
    my $sleep_for;

    BEGIN {
        override('ntp_wait', sleep => sub {
                is(shift, $sleep_for, 'sleep with the right value');
            });
    }

    sub run_ntp_wait {
        my (%opts) = @_;
        $sleep_for = delete $opts{sleep_for} || 0;
        run_mocked(run => sub { ntp_wait::run(@{ delete $opts{opts} || [] }) }, %opts);
    }
}

my $ret;

my %vars = (
    status_line => {
        leap => 'leap_alarm',
    }
);

stdout_like(
    sub { $ret = run_ntp_wait(vars => [\%vars], opts => [qw(-n 1 -s 0 -v)]) },
    qr/ntpd did not synchronize/,
    'failed to synchronize in time with leap_alarm'
);
is($ret, 1, 'fails when failed to synchronize with leap_alarm');

%vars = (
    status_line => {
        leap => 'sync_alarm',
    }
);

stdout_like(
    sub { $ret = run_ntp_wait(vars => [\%vars], opts => [qw(-n 1 -s 0 -v)]) },
    qr/ntpd did not synchronize/,
    'failed to synchronize in time with sync_alarm'
);
is($ret, 1, 'fails when failed to synchronize with sync_alarm');

stdout_like(
    sub { $ret = run_ntp_wait(vars => [\%vars], opts => [qw(-n 1 -s 0 -v)]) },
    qr/ntpd did not synchronize/,
    'failed to synchronize in time with -v'
);
is($ret, 1, 'fails when failed to synchronize with -v');


%vars = (
    status_line => {
        leap => 'leap_bogus',
    }
);

stdout_like( 
    sub { $ret = run_ntp_wait(vars => [\%vars], opts => [qw(-n 2 -s 0 -v)]) },
    qr/Unexpected 'leap' status <leap_bogus>/,
    'prints debug information on invalid leap status'
);
is($ret, 1, 'fails on invalid leap status');


stdout_like(
    sub { $ret = run_ntp_wait(vars => [{}], opts => [qw(-n 2 -s 0 -v)]) },
    qr/Leap status not avalaible/,
    'print debug information when no leap returned'
);
is($ret, 1, 'fails if not leap returned');

%vars = (
    status_line => {
        leap => 'leap_none',
    }
);

stdout_is( 
    sub { $ret = run_ntp_wait(vars => [\%vars], opts => [qw(-n 1 -s 0)]) },
    '',
    'prints nothing if OK'
);
is($ret, 0, 'succeeds if OK');

stdout_like( 
    sub { $ret = run_ntp_wait(vars => [\%vars], opts => [qw(-n 1 -s 0 -v)]) },
    qr/OK/,
    'prints OK if OK and -v'
);
is($ret, 0, 'succeeds if OK');

#Test the -n switch 

my @vars = (
    { 
        status_line => {
            leap => 'leap_alarm',
        }
    },
    { 
        status_line => {
            leap => 'leap_none',
        }
    },
);
stdout_like( 
    sub { $ret = run_ntp_wait(vars => \@vars, opts => [qw(-n 2 -s 0 -v)]) },
    qr/OK/,
    '-n switch works'
);
is($ret, 0, 'succeeds if OK the second time');

# Test the -s switch

stdout_like( 
    sub { $ret = run_ntp_wait(vars => \@vars, opts => [qw(-n 2 -s 10 -v)], sleep_for => 10) },
    qr/OK/,
    '-s switch works'
);
is($ret, 0, 'suceeds if OK and -s');
