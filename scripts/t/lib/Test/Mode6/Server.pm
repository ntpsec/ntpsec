package Test::Mode6::Server;
use strict;
use warnings;
use IO::Socket::INET;
use Test::Builder;
use NTP::Mode6::Packet;
use Carp;
use Exporter qw(import);
use Errno qw(EINTR);
use Devel::Hexdump qw(xd);
our @EXPORT = qw(start_mode6_server expect_mode6_command stop_mode6_server);

my $socket;
my $Test = Test::Builder->new();

sub start_mode6_server {
    $socket = IO::Socket::INET->new(
        Proto     => 'udp',
        LocalPort => 123,
    ) or croak "Could not create UDP socket on port 123: $!";
    $Test->diag("NTP control server started");
}

sub _diag_and_fail {
    $Test->diag(shift);
    $Test->ok(0, shift);
    0;
}

sub expect_mode6_command {
    my ($ex_packet, $respond, $descr) = @_;

    return _diag_and_fail('Not a CODE reference', $descr) 
        if ref $respond ne 'CODE';

    my $remote_addr = $socket->recv(my $in_msg = '', 4096, 0);
    if (!$remote_addr) {
        _diag_and_fail("Error in recv: $!", $descr) if $! != EINTR;
        return;
    }

    my $recv_packet = NTP::Mode6::Packet->new();
    $recv_packet->decode($in_msg);
    my $diag = $recv_packet->eq($ex_packet);
    if ($diag) {
        _diag_and_fail("Packets don't match\n$diag", $descr);
        return;
    }

    my $resp_packet = $respond->($recv_packet);
    return _diag_and_fail('reponse sub didn\'t return NTP::Mode6::Packet object', $descr)
        if ref $resp_packet ne 'NTP::Mode6::Packet';

    my $out_msg = $resp_packet->encode;
    $socket->send($out_msg, 0, $remote_addr) == length $out_msg
        or return _diag_and_fail("Error sending response packet: $!", $descr);

    $Test->ok(1, $descr);
}

sub stop_mode6_server {
    close $socket;
    $Test->diag("NTP control server stopped");
}

1;
