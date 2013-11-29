package NTP::Mode6::Packet;
use strict;
use warnings;
use Carp;
use Exporter qw(import);
our @EXPORT_OK = qw(OP_UNSPEC OP_READSTAT OP_READVAR OP_WRITEVAR OP_READCLOCK
            OP_WRITECLOCK OP_SETTRAP OP_ASYNCMSG OP_CONFIGURE OP_SAVECONFIG
            OP_READ_MRU OP_READ_ORDLIST_A OP_REQ_NONCE OP_UNSETTRAP);
our %EXPORT_TAGS = (const => \@EXPORT_OK);

use constant { 
    OP_UNSPEC         => 0,  # unspeciffied
    OP_READSTAT       => 1,  # read status
    OP_READVAR        => 2,  # read variables
    OP_WRITEVAR       => 3,  # write variables
    OP_READCLOCK      => 4,  # read clock variables
    OP_WRITECLOCK     => 5,  # write clock variables
    OP_SETTRAP        => 6,  # set trap address
    OP_ASYNCMSG       => 7,  # asynchronous message
    OP_CONFIGURE      => 8,  # runtime configuration
    OP_SAVECONFIG     => 9,  # save config to file
    OP_READ_MRU       => 10, # retrieve MRU (mrulist)
    OP_READ_ORDLIST_A => 11, # ordered list req. auth.
    OP_REQ_NONCE      => 12, # request a client nonce
    OP_UNSETTRAP      => 31, # unset trap
};

use constant {
    SST_TS_UNSPEC    => 0, # unspec
    SST_TS_ATOM      => 1, # pps
    SST_TS_LF        => 2, # lf radio
    SST_TS_HF        => 3, # hf radio
    SST_TS_UHF       => 4, # uhf radio
    SST_TS_LOCAL     => 5, # local
    SST_TS_NTP       => 6, # ntp
    SST_TS_UDPTIME   => 7, # other
    SST_TS_WRSTWTCH  => 8, # wristwatch
    SST_TS_TELEPHONE => 9, # telephone
};

use constant CTL_HEADER_LENGTH => 12;

sub new {
    my ($class, %opts) = @_;
    my $self = {
        version      => defined $opts{version}     ? $opts{version}     : 2,
        leap         => defined $opts{leap}        ? $opts{leap}        : 0,
        sequence     => defined $opts{sequence}    ? $opts{sequence}    : 0,
        status       => defined $opts{status}      ? $opts{status}      : 0,
        assoc_id     => defined $opts{assoc_id}    ? $opts{assoc_id}    : 0,
        offset       => defined $opts{offset}      ? $opts{offset}      : 0,
        error_bit    => defined $opts{is_error}    ? $opts{is_error}    : 0,
        response_bit => defined $opts{is_response} ? $opts{is_response} : 0,
        more_bit     => defined $opts{more}        ? $opts{more}        : 0,
        opcode       => defined $opts{opcode}      ? $opts{opcode}      : 0,
    };
    my $ret = bless $self, $class;
    if (defined $opts{data}) {
        $self->data($opts{data});
    }
    else {
        $self->data('');
    }
    return $ret;
}

sub version {
    my ($self, $version) = @_;
    $self->{version} = $version if defined $version;
    return $self->{version};
}

sub leap {
    my ($self, $leap) = @_;
    $self->{leap} = 1 if $leap;
    return $self->{leap};
}

sub assoc_id {
    my ($self, $assoc_id) = @_;
    $self->{assoc_id} = $assoc_id if defined $assoc_id;
    return $self->{assoc_id};
}

sub is_response {
    my ($self, $is_response) = @_;
    $self->{response_bit} = 1 if defined $is_response;
    return $self->{response_bit};
}

sub is_command {
    my ($self, $is_command) = @_;
    $self->{response_bit} = 0 if $is_command;
    return !$self->{response_bit};
}

sub is_error {
    my ($self, $is_error) = @_;
    $self->{error_bit} = $is_error if defined $is_error;
    return $self->{error_bit};
}

sub more {
    my ($self, $is_more) = @_;
    $self->{more_bit} = $is_more if defined $is_more;
    return $self->{more_bit};
}

sub opcode {
    my ($self, $opcode) = @_;
    $self->{opcode} = $opcode if defined $opcode;
    return $self->{opcode};
}

sub sequence {
    my ($self, $sequence) = @_;
    $self->{sequence} = $sequence if defined $sequence;
    return $self->{sequence};
}

sub status {
    my ($self, $status) = @_;
    $self->{status} = $status if defined $status;
    return $self->{status};
}

sub offset {
    my ($self, $offset) = @_;
    $self->{offset} = $offset if defined $offset;
    return $self->{offset};
}

sub data_length {
    my $self = shift;
    return $self->{count};
}

sub data {
    my ($self, $data) = @_;
    if (defined $data) {
        # TODO: prevent passing unicode?
        $self->{count} = length $data;
        $self->{data} = $data;
    }
    return $self->{data};
}

sub encode {
    my $self = shift;

    my $li_vn_mode = 0;
    $li_vn_mode = ($self->leap() & 7) << 3;
    $li_vn_mode |= ($self->version() & 7) << 3;
    $li_vn_mode |= 6;

    my $r_m_e_op = 0;
    $r_m_e_op |= 0x80 if $self->is_response;
    $r_m_e_op |= 0x40 if $self->is_error;
    $r_m_e_op |= 0x20 if $self->more;
    $r_m_e_op |= $self->opcode;

    # Align to 32-bit boundary
    my $padding = 0;
    while (($self->data_length()+CTL_HEADER_LENGTH+$padding) & 3) {
        $padding++;
    }

    my $msg = pack "CCnnnnnA*C$padding", $li_vn_mode, $r_m_e_op,
        $self->sequence, $self->status, $self->assoc_id, $self->offset,
        $self->data_length, $self->data, 0 x $padding;
    return $msg;
}

sub decode {
    my ($self, $msg) = @_;

    my @res = unpack 'CCnnnnnA*', $msg;

    my $li_vn_mode = shift @res;
    my $mode = 3;
    $mode = $li_vn_mode & 0x7;
    croak 'Not an NTP Mode 6 packet' if $mode != 6;
    $self->version(($li_vn_mode >> 3) & 0x7);
    $self->leap(($li_vn_mode >> 6) & 0x3);

    my $r_m_e_op = shift @res;
    $r_m_e_op & 0x80 ? 
        $self->is_response(1) :
        $self->is_command(1);
    $self->is_error(1) if $r_m_e_op & 0x40;
    $self->more(1)     if $r_m_e_op & 0x20;
    $self->opcode($r_m_e_op & 0x1f);

    $self->sequence(shift @res);
    $self->status(shift @res);
    $self->assoc_id(shift @res);
    $self->offset(shift @res);

    my $count = shift @res;
    $self->data(shift @res);
    croak "count($count) != recieved data length(".$self->data_length.")"
        if $self->data_length != $count;
}

sub eq {
    my ($self, $ex) = @_;

    croak 'Not a NTP::Mode6::Packet object' 
        if ref $ex ne 'NTP::Mode6::Packet';

    #TODO: move diag out of this method
    my @diag;
    for (grep { $_ !~ /^(data|count)$/ } keys %$self) {
        push @diag, "$_: $self->{$_} | $ex->{$_}" 
            if $self->{ $_ } != $ex->{ $_ };
    }
    push @diag, "data: $self->{data} | $ex->{data}" 
        if $self->{data} ne $ex->{data};
    return join "\n", @diag;
}

1;
