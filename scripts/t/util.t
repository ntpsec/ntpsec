use strict;
use warnings;
use Test::More tests => 12;
use Test::Util qw(override);

BEGIN {
    my %test;

    override('NTP::Util', open => sub { 
            is($_[1], $test{open_str}, 'open with right string')
                if defined $test{open_str};
            return if $test{fail_open};
            open $_[0], '<', \$test{output};
        });

    sub test_read_vars {
        my (%opts) = @_;
        _parse_opts(%opts);
        ntp_read_vars(@{ delete $opts{opts} || []});
    }

    sub test_peers {
        my (%opts) = @_;
        _parse_opts(%opts);
        ntp_peers(@{ delete $opts{opts} || []});
    }

    sub test_sntp_line {
        my (%opts) = @_;
        _parse_opts(%opts);
        ntp_sntp_line(@{ delete $opts{opts} || []});
    }

    sub _parse_opts {
        my (%opts) = @_;
        $test{fail_open} = delete $opts{fail_open} || 0;
        $test{output}    = delete $opts{output} || '' ;
        $test{open_str}  = delete $opts{open_str};
    }
}

require_ok 'NTP::Util';
NTP::Util->import(qw(ntp_read_vars ntp_peers ntp_sntp_line));

eval {
    test_read_vars(opts => [0, []], fail_open => 1,
        open_str => "ntpq -n -c 'rv 0 ' |");
} or do {
    like($@, qr/Could not start ntpq: /, 'ntp_read_vars dies on failed execution');
};

is(
    test_read_vars(opts => [0, []], output => 'Connection refused'),
    undef,
    'returns undef when connection refused'
);

is_deeply(
    test_read_vars(opts => [0, [qw(var1 var2 var_3 rootdisp offset)]], 
        output => "var1=foo, var_3=\"bar\"\nphase=1.00, rootdispersion=4.00\n"),
        {   
            var1     => 'foo',
            var_3    => 'bar',
            var2     => undef,
            offset   => '1.00',
            rootdisp => '4.00',
        },
    'variables correctly parsed'
);

is_deeply(
    test_read_vars(opts => [0, []], 
        output => <<'VAR_END'),
associd=0 status=c012 leap_alarm, sync_unspec, 1 event, freq_set,
var1=foo, var_3="bar"
phase=1.00, rootdispersion=4.00
VAR_END
        {   
            var1     => 'foo',
            var_3    => 'bar',
            offset   => '1.00',
            rootdisp => '4.00',

            status_line => { 
                leap   => 'leap_alarm',
                sync   => 'sync_unspec',
                status => 'c012'
            } 
        },
    'variables correctly parsed'
);

eval {
    test_peers(opts => ['localhost'], fail_open => 1,
        open_str => "ntpq -np localhost |");
} or do {
    like($@, qr/Could not start ntpq: /, 'ntp_peers dies on failed execution');
};

is_deeply(
    test_peers(opts => ['localhost'], 
        output => <<'PEER_END'),
             remote           refid      st t when poll reach   delay   offset  jitter
==============================================================================
+194.160.23.2    .GPS.            1 u  188 1024  377    0.290    0.051   0.058
-217.31.205.226  195.113.144.201  2 u  227 1024  377    6.092   -0.372   0.064
 147.231.19.43   .INIT.          16 u    - 1024    0    0.000    0.000 4000.00
*195.113.144.201 .GPS.            1 u  211 1024  377    5.817    0.023   0.054
PEER_END
    [
          {
            'when' => '188',
            'reach' => '377',
            'delay' => '0.290',
            'st' => '1',
            'remote' => '+194.160.23.2',
            'poll' => '1024',
            'jitter' => '0.058',
            'refid' => '.GPS.',
            't' => 'u',
            'offset' => '0.051'
          },
          {
            'when' => '227',
            'reach' => '377',
            'delay' => '6.092',
            'st' => '2',
            'remote' => '-217.31.205.226',
            'poll' => '1024',
            'jitter' => '0.064',
            'refid' => '195.113.144.201',
            't' => 'u',
            'offset' => '-0.372'
          },
          {
            'when' => '-',
            'reach' => '0',
            'delay' => '0.000',
            'st' => '16',
            'remote' => ' 147.231.19.43',
            'poll' => '1024',
            'jitter' => '4000.00',
            'refid' => '.INIT.',
            't' => 'u',
            'offset' => '0.000'
          },
          {
            'when' => '211',
            'reach' => '377',
            'delay' => '5.817',
            'st' => '1',
            'remote' => '*195.113.144.201',
            'poll' => '1024',
            'jitter' => '0.054',
            'refid' => '.GPS.',
            't' => 'u',
            'offset' => '0.023'
          }
        ],
    'peers correctly parsed'
);

eval {
    test_sntp_line(opts => ['localhost'], fail_open => 1,
        open_str => "sntp localhost |");
} or do {
    like($@, qr/Could not start sntp: /, 'ntp_sntp_line dies on failed execution');
};

is_deeply(
    [test_sntp_line(opts => ['localhost'], 
        output => <<'SNTP_END')],
sntp 4.2.7p379@1.2946-o Tue Aug 27 18:55:18 UTC 2013 (4)
Can't open KOD db file /var/db/ntp-kod for writing: Permission denied
2013-09-19 22:49:29.381062 (-0100) +0.000007 +/- 0.077138 localhost 127.0.0.1 s4
SNTP_END
    ['+0.000007', '4'],
    'stratum and offset parsed'
);
