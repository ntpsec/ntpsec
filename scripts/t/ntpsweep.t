use strict;
use warnings;
use Test::More;
use Test::Util qw(check_help override mock_ntp_util run_mocked);
use Test::Output;
use Sub::Override;

my $script_name = "ntpsweep/ntpsweep";

my @options = (
    [qw(host-list=str l)],
    [qw(peers p)],
    [qw(maxlevel=num m)],
    [qw(strip=str s)],
);

require "./$script_name";

check_help("./$script_name", @options);

mock_ntp_util('ntpsweep', read_vars => 1, peers => 1, sntp_line => 1, dns => 1);

my $header = <<'HEADER';
Host                             st offset(s) version     system       processor
--------------------------------+--+---------+-----------+------------+---------
HEADER

stdout_is(
    sub { run_mocked(run => sub { ntpsweep::run(qw(-l localhost)) }) },
    $header."localhost                         ?\n",
    'no information about host'
);

# Test deprecated -h switch
stdout_is(
    sub { run_mocked(run => sub { ntpsweep::run(qw(-h localhost)) }) },
    $header."localhost                         ?\n",
    '-h switch works'
);

my @vars = ({
    processor      => 'i686',
    system         => 'Linux/2.6.37.6-smp',
    daemon_version => 'ntpd 4.2.6p3@1.2290-o Wed Jan 26 04:19:40 UTC 2011 (1)',
});

my @offset_stratum = (['0.00', 3]);

stdout_is(
    sub { run_mocked(run => sub { ntpsweep::run(qw(-l localhost)) }, offset_stratums => \@offset_stratum ) },
    $header.'localhost                         3     0.000'.(' 'x26)."\n",
    'one host ok but no variables'
);

stdout_is(
    sub { run_mocked(run => sub { ntpsweep::run(qw(-l localhost)) }, offset_stratums => \@offset_stratum, vars => \@vars ) },
    $header."localhost                         3     0.000 4.2.6p3\@1.2 Linux/2.6.37 i686\n",
    'one host with ok variables'
);

## Add one peer
my @peers = ([{ remote => 'pieskovisko.sk' }]);
push @offset_stratum, ['0.01', 2];
push @vars, {
    processor      => 'i686',
    system         => 'Bungalanga OS',
    daemon_version => 'ntpd 4.1.5p3@1.2-o Wed Jan 26 04:19:40 UTC 2011 (1)',
};

stdout_is(
    sub { run_mocked(run => sub { ntpsweep::run(qw(-l localhost -p)) }, offset_stratums => \@offset_stratum, vars => \@vars, peers => \@peers) },
    $header."localhost (1)                     3     0.000 4.2.6p3\@1.2 Linux/2.6.37 i686\n"
           ." pieskovisko.sk (0)               2     0.010 4.1.5p3\@1.2 Bungalanga O i686\n",
    'one peer'
);

# Add another peer
push @peers, [{ remote => 'nic.nz' }];
push @offset_stratum, ['0.04', 1];
push @vars, {
    processor      => 'i686',
    system         => 'NIC OS',
    daemon_version => 'ntpd 4.1.5p3@1.2-o Wed Jan 26 04:19:40 UTC 2011 (1)',
};

stdout_is(
    sub { run_mocked(run => sub { ntpsweep::run(qw(-l localhost -p -m 1)) }, offset_stratums => \@offset_stratum, vars => \@vars, peers => \@peers) },
    $header."localhost (1)                     3     0.000 4.2.6p3\@1.2 Linux/2.6.37 i686\n"
           ." pieskovisko.sk (1)               2     0.010 4.1.5p3\@1.2 Bungalanga O i686\n",
    'one peer twice but -m 1'
);

stdout_is(
    sub { run_mocked(run => sub { ntpsweep::run(qw(-l localhost -p)) }, offset_stratums => \@offset_stratum, vars => \@vars, peers => \@peers) },
    $header."localhost (1)                     3     0.000 4.2.6p3\@1.2 Linux/2.6.37 i686\n"
           ." pieskovisko.sk (1)               2     0.010 4.1.5p3\@1.2 Bungalanga O i686\n"
           ."  nic.nz (0)                      1     0.040 4.1.5p3\@1.2 NIC OS       i686\n",
    'one peer twice'
);

push @peers, [{ remote => 'localhost' }];
push @offset_stratum, ['0.04', 1];
push @vars, {
    processor      => 'i686',
    system         => 'NIC OS',
    daemon_version => 'ntpd 4.1.5p3@1.2-o Wed Jan 26 04:19:40 UTC 2011 (1)',
};

stdout_is(
    sub { run_mocked(run => sub { ntpsweep::run(qw(-l localhost -p)) }, offset_stratums => \@offset_stratum, vars => \@vars, peers => \@peers) },
    $header."localhost (1)                     3     0.000 4.2.6p3\@1.2 Linux/2.6.37 i686\n"
           ." pieskovisko.sk (1)               2     0.010 4.1.5p3\@1.2 Bungalanga O i686\n"
           ."  nic.nz (1)                      1     0.040 4.1.5p3\@1.2 NIC OS       i686\n"
           ."   = localhost".(' 'x18)."\n",
    'loop detection'
);
done_testing;
