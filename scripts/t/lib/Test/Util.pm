package Test::Util;
use strict;
use warnings;
use Test::Command;
use Test::More;
use File::Basename qw(basename);
use Exporter       qw(import);
use Carp           qw(croak);
use Scalar::Util   qw(set_prototype);
use Sub::Override;

our @EXPORT_OK = qw(check_help override call_orig mock_ntp_util run_mocked);

our $DIAG = 0;
sub cdiag { diag @_ if $DIAG }

sub check_help {
    my ($script_path, @options) = @_;
    my $script_name = basename($script_path);

    cmd_is("$script_path -?", '', 
        sub {
            my $help = shift;
            my $i;

            cdiag "Got help '$help'";

            $help =~ /^$script_name\ [^\n]+\nUSAGE:\ $script_name\ 
                      \[\ -<flag>\ \[<val>\]\ \|\ --<name>\[{=\|\ }<val>\]\ \]
                    /x or return 0;

            cdiag "Usage line OK";

            for (@options, [qw(help ?)], [qw(more-help)]) {
                my ($long, $short) = @$_;
                my $o = $short ? "-$short, --$long" : "  , --$long";

                cdiag "Testing for $o\n";

                return 0 if $help !~ /$o/;
            }

            cdiag "Options OK";

            $help =~ <<'END' or return 0;
Options are specified by doubled hyphens and their name or by a single
hyphen and the flag character.$
END
            cdiag "Bottom line OK";

            return 1;
        }, 0, "help's ok");
}

my $override = Sub::Override->new;

{
    my (@vars, @peers, @hosts, @offset_stratums);

    sub mock_ntp_util {
        my ($package, %mock) = @_;

        $override->replace($package.'::ntp_read_vars' => sub { 
                return undef if !@vars;
                croak 'vars elements should be HASH refs' 
                    if ref $vars[0] ne 'HASH';
                return +{ %{ shift @vars } };
            }) if exists $mock{read_vars};

        $override->replace($package.'::ntp_peers'     => sub { 
                return () if !@peers;
                croak 'peer elements should be ARRAY refs'
                    if ref $peers[0] ne 'ARRAY';
                return @{ shift @peers }
            }) if exists $mock{peers};

        $override->replace($package.'::do_dns'        => sub { 
                return shift @hosts 
            }) if exists $mock{dns};

        $override->replace($package.'::ntp_sntp_line' => sub { 
                return () if !@offset_stratums;
                croak 'offset_stratums elements should be ARRAY refs' 
                    if ref $offset_stratums[0] ne 'ARRAY';
                return @{ shift @offset_stratums };
            }) if exists $mock{sntp_line};
    }

    sub run_mocked {
        my %opts = @_;

        for (qw(vars peers hosts offset_stratums)) {
            croak "$_ is not an ARRAY reference" if exists $opts{$_} && ref $opts{$_} ne 'ARRAY';
        }
        croak "run is not a CODE reference" if ref $opts{run} ne 'CODE';

        @vars  = @{ $opts{vars} } if defined $opts{vars};
        @peers = @{ $opts{peers}} if defined $opts{peers};
        @hosts = @{ $opts{hosts}} if defined $opts{hosts};
        @offset_stratums = @{ $opts{offset_stratums} } if defined $opts{offset_stratums};
        $opts{run}->();
    }
}

sub override {
    my ($package, $subname, $code) = @_;

    croak 'not a CODE reference' if ref $code ne 'CODE';

    my $proto = prototype $code;
    my $override = set_prototype(sub {
            my $caller = caller;
            if ($caller eq $package) {
                return $code->(@_);
            }
            else {
                call_orig($package, $subname);
            }
        }, $proto);

    {
        no strict 'refs';
        *{"CORE::GLOBAL::".${subname}} = $override;
    }
}

sub call_orig {
    my ($pkg, $name) = (shift, shift);
    # TODO: cache this?
    _make_helper($pkg, $name)->(@_);
}


# Doing this right would require some hairy code and a lot more work (just look
# at Fatal.pm) Since we just want to override open sleep and few other builtins
# this is sufficient.
my %orig_helpers = (
    sleep => 'return CORE::sleep $_[0]',
    open  => '
        no strict q(refs);
        if (@_ == 1) {
            return CORE::open($_[0]);
        }
        elsif (@_ == 2) {
            return CORE::open($_[0], $_[1]);
        }
        elsif (@_ >= 3) {
            return CORE::open($_[0], $_[1], @_[2 .. $#_]);
        }
    ',
);

# This a helper for the override sub, it generates sub that provides the
# original package context
sub _make_helper {
    my ($pkg, $name) = @_;
    my $sub = eval "
        sub {
            package $pkg;
            $orig_helpers{$name}
        }
    ";
    die "Eval failed for ${pkg}: $@" unless $sub;
    return $sub;
}

1;
