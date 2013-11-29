package Test::Command;
use strict;
use warnings;
use Test::Builder;
use IPC::Open3;
use IO::Handle;
use Carp;
use base qw(Exporter);
our @EXPORT = qw(cmd_is start_command com_stdout_is com_stderr_is com_ret_is
        com_slurp_stderr com_slurp_stdout wait_for_command);
our $VERSION = 0.1;

my $Test = Test::Builder->new();

my ($pid, $writer_h, $stdout_h, $stderr_h);
my $in_command = 0;

$SIG{CHLD} = sub { wait_for_command() if $in_command; };

sub start_command {
    my (@cmd) = @_;

    croak "Specify a command to start_command" unless @cmd;

    ($pid, $writer_h, $stdout_h, $stderr_h) = 
        (0, IO::Handle->new(), IO::Handle->new(), IO::Handle->new());

    # Older IPC:Open3 just warns is there was an error
    local $SIG{__WARN__} = sub { die @_ };

    $in_command = 1;
    $pid = open3($writer_h, $stdout_h, $stderr_h, @cmd); 
}

sub wait_for_command {
    waitpid $pid, 0;
    my $ret = $? >> 8;
    if (com_slurp_stderr() =~ /^open3: (.+)$/) { 
        croak $1 
    }
    return $ret;
}

sub com_ret_is {
    my ($ex_ret, $descr) = @_;

    local $SIG{CHLD} = 'IGNORE';
    my $ret = wait_for_command;
    $Test->ok(_test_ret($ret, $ex_ret), $descr);
}

sub com_stdout_is {
    my ($ex_stdout, $descr) = @_;

    my $stdout = com_slurp_stdout();
    $Test->ok(_test_string($stdout, $ex_stdout, 'STDOUT'), $descr);
}

sub com_stderr_is {
    my ($ex_stderr, $descr) = @_;

    my $stderr = com_slurp_stderr();
    $Test->ok(_test_string($stderr, $ex_stderr, 'STDERR'), $descr);
}

{
    my $stderr;
    sub com_slurp_stderr { 
        $stderr or $stderr = _slurp_fh($stderr_h);
    }
}
{
    my $stdout;
    sub com_slurp_stdout { 
        $stdout or $stdout = _slurp_fh($stdout_h);
    }
}

sub _test_one {
    my ($s, $ex, $name) = @_;
    my $diag = defined $ex && _test_string($s, $ex);
    if ($diag) {
        $Test->diag("$name doesn't match\n$diag\n");
        return 0;
    }
    return 1;
}

sub cmd_is {
    my ($cmd, $ex_stdout, $ex_stderr, $ex_ret, $descr);

    if (ref $_[0] eq 'HASH') {
        my %args = %{ $_[0] };
        ($cmd, $ex_stdout, $ex_stderr, $ex_ret, $descr) = 
            @args{qw(run stdout stderr ret descr)};
    }
    else {
        ($cmd, $ex_stdout, $ex_stderr, $ex_ret, $descr) = @_;
    }


    local $SIG{CHLD} = 'DEFAULT';
    start_command($cmd);

    my $stdout = com_slurp_stdout;
    my $stderr = $stderr_h ? com_slurp_stderr : '';
    my $ret = wait_for_command;
    my $ok = 1;

    _test_string($stdout, $ex_stdout, 'STDOUT') or $ok = 0;
    _test_string($stderr, $ex_stderr, 'STDERR') or $ok = 0;
    _test_ret($ret, $ex_ret) or $ok = 0;

    $Test->ok($ok, $descr);
}

sub _slurp_fh { 
    my $fh = shift;
    do { local $/; <$fh> } 
}

sub _test_ret {
    my ($got, $exp) = @_;

    return 1 if not defined $exp;

    if ($got != $exp) {
        return $Test->diag(<<DIAG);
Return value doesn't match
Expected: $exp
Got: $got
DIAG
    }
    return 1;
}

sub _test_string {
    my ($got, $exp, $name) = @_;

    return 1 if not defined $exp;

    if (ref $exp eq 'Regexp') {
        if ($got !~ $exp) {
            return $Test->diag(<<DIAG);
$name doesn't match.
Expected: '$exp'
Got: '$got'
DIAG
        }
    }
    elsif (ref $exp eq 'CODE') {
        if (!(my $ret = $exp->($got))) {
            return $Test->diag(<<DIAG);
$name doesn't match.
Expected CODE to return true value
Got: '$ret'
DIAG
        }
    }
    else {
        if ($got ne $exp) {
            return $Test->diag(<<DIAG);
$name doesn't match.
Expected: '$exp'
Got: '$got'
DIAG
        }
    }
    return 1;
}

1;
__END__
=head1 NAME

Test::Command - Test output and ret value of an external command.

=head1 VERSION

Version 0.1

=head1 SYNOPSIS

    use Test::More tests => 1;
    use Test::Command;

    cmd_is('ls', '.\n..', '', 0, "ls on empty directory");
    cmd_is({run => 'ls', stdout => '.\n..', ret => 0});

    start_command('ls');
    ...
    wait_for_command;
    com_stdout_is('.\n..');

=head1 DESCRIPTION

Test::Command provides a simple interface for testing stdout, stderr and return
value of an external command.

=cut

=head1 TESTS

=over 4 

=item B<is_cmd( cmd, exp_stdout, exp_stderr, exp_ret, diag )>

    Runs command and tests for expected results. Pass undef in the parameter you
    don't want to test for.

=over 4 

=item B<cmd> - string of command tu run

=item B<exp_stdout> - expected stdout

=item B<exp_stderr> - expected stderr

=item B<exp_ret> - expected return value of a program

=item B<diag> - diagnostic message printed if test failed

=back

=item B<start_command( cmd )>

Start a command. Returns immediately and doesn't wait for command to finish.

=item B<wait_for_command>

Wait for command started with C<start_command> to finish. Returns exit status
(return value of waipid) of the command.

=item B<com_slurp_stdout>

=item B<com_slurp_stderr>

Slurp stdout/stderr of a process started with start_command and return it.

=item B<com_stdout_is( ex_stdout, diag )>

=item B<com_stderr_is( ex_stdout, diag )>

Test stdout/stderr of the command started with start_command. This will block
until the process has closed its stdout or stderr

=item B<com_ret_is( ex_ret, diag )>

Test for expected return value of a command started with start_command.

=back

All parameters that represent expected value for stdout or stderr can be a
string, a compiled regexp or a code reference, which is passed the contents as
a first parameter and is expected to return true or false value.

=head1 AUTHOR

Oliver Kindernay <oliver.kindernay@gmail.com>

=cut
