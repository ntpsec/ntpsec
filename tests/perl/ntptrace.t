use strict;
use warnings;
use Test::More tests => 6;
use Test::Command;
use Test::Output;
use Test::Util qw(check_help mock_ntp_util run_mocked);
use Sub::Override;

my $script_name = 'ntptrace/ntptrace';

my @options = (
    [qw(numeric n)], 
    [qw(max-hosts=num m)],
    [qw(host=str r)],
);
check_help($script_name, @options);

require "./$script_name";

mock_ntp_util('ntptrace', read_vars => 1, dns => 1);

my @vars = ({
    version    => 'ntpd 4.2.6p3@1.2290-o Wed Jan 26 04:19:40 UTC 2011 (1)',
    processor  => 'i686',
    system     => 'Linux/2.6.37.6-smp',
    leap       => '00',
    stratum    => '3',
    precision  => '-21',
    rootdelay  => '38.998',
    rootdisp   => '94.413',
    refid      => '92.240.244.202',
    reftime    => 'd5842335.00c8980f  Sun Jul  7 2013 19:26:13.003',
    clock      => 'd5842501.ee752d06  Sun Jul  7 2013 19:33:53.931',
    peer       => '848',
    tc         => '9',
    mintc      => '3',
    offset     => '-23.168',
    frequency  => '-10.213',
    sys_jitter => '14.724',
    clk_jitter => '14.754',
    clk_wander => '3.847',
});

stdout_is(
    sub { run_mocked(run => sub { ntptrace::run() }, vars => [@vars,{}], hosts => [qw(localhost)]) },
    "localhost: stratum 3, offset -0.023168, synch distance 0.113912\n",
    'traces single host'
);

stdout_is(
    sub { run_mocked(run => sub { ntptrace::run(qw(-n)) }, vars => [@vars,{}]) },
    "127.0.0.1: stratum 3, offset -0.023168, synch distance 0.113912\n",
    '-n switch works, localhost is default'
);

#Add response to a rv peer
push @vars, {
    srcadr     => '1.2.3.4',
    srcport    => '123',
    dstadr     => '192.168.1.11',
    dstport    => '123',
    leap       => '00',
    stratum    => '2',
    precision  => '-22',
    rootdelay  => '5.142',
    rootdisp   => '30.884',
    refid      => '195.146.149.222',
    reftime    => 'd584299f.8904b565  Sun Jul  7 2013 19:53:35.535',
    rec        => 'd5842d6b.02efea46  Sun Jul  7 2013 20:09:47.011',
    reach      => '377',
    unreach    => '0',
    hmode      => '3',
    pmode      => '4',
    hpoll      => '9',
    ppoll      => '9',
    headway    => '0',
    flash      => '00 ok',
    keyid      => '0',
    offset     => '-24.318',
    delay      => '33.499',
    dispersion => '20.588',
    jitter     => '39.916',
    xleave     => '0.031',
    filtdelay  => '   42.35   93.96  235.47   33.50   34.31   33.90   37.25   34.97',
    filtoffset => '  -21.46    4.17   77.27  -24.32  -23.48  -23.17  -21.27  -25.42',
    filtdisp   => '    0.00    8.01   15.78   23.54   31.25   39.21   47.24   55.23',
};

# Add response from the peer
push @vars, {
    version        => 'ntpd 4.2.4p8@1.1612-o Tue Apr 19 07:08:29 UTC 2011 (1)',
    processor      => "i686",
    system         => "Linux/2.6.38-10-generic-pae",
    leap           => '00',
    stratum        => '1',
    precision      => '-20',
    rootdelay      => '0.000',
    rootdisp       => '0.348',
    peer           => '41683',
    refid          => 'CDMA',
    reftime        => 'd58443c5.cc475b06  Sun Jul  7 2013 19:45:09.797',
    poll           => '5',
    clock          => 'd58443cd.079e0276  Sun Jul  7 2013 19:45:17.029',
    state          => '4',
    offset         => '0.000',
    frequency      => '-21.795',
    jitter         => '0.001',
    noise          => '0.005',
    stability      => '0.000',
    tai            => '0',
};

stdout_is(
    sub { run_mocked(run => sub { ntptrace::run() }, vars => \@vars, hosts => [qw(localhost kenny.oneemedia.com)]) },
    <<'END',
localhost: stratum 3, offset -0.023168, synch distance 0.113912
kenny.oneemedia.com: stratum 1, offset 0.000000, synch distance 0.000348, refid 'CDMA'
END
    'traced a peer up to stratum 1'
);

stdout_is(
    sub { run_mocked(run => sub { ntptrace::run(qw(-m 1)) }, vars => \@vars, hosts => [qw(localhost)]) },
    "localhost: stratum 3, offset -0.023168, synch distance 0.113912\n",
    '-m switch works'
);

splice @vars, 0, 1, {
    version    => 'ntpd 4.2.6p3@1.2290-o Wed Jan 26 04:19:40 UTC 2011 (1)',
    processor  => "i686",
    system     => "Linux/2.6.37.6-smp",
    leap       => '00',
    stratum    => '3',
    precision  => '-21',
    rootdelay  => '38.998',
    rootdisp   => '94.413',
    refid      => '127.127.0.1',
    reftime    => 'd5842335.00c8980f  Sun Jul  7 2013 19:26:13.003',
    clock      => 'd5842501.ee752d06  Sun Jul  7 2013 19:33:53.931',
    peer       => '848',
    tc         => '9',
    mintc      => '3',
    offset     => '-23.168',
    frequency  => '-10.213',
    sys_jitter => '14.724',
    clk_jitter => '14.754',
    clk_wander => '3.847',
};

stdout_is(
    sub { run_mocked(run => sub { ntptrace::run() }, vars => \@vars) },
    "127.0.0.1: stratum 3, offset -0.023168, synch distance 0.113912\n",
    'stopped on 127.127 refid'
);
