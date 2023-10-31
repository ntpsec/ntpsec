# Hi


PROGS = fake-samba fake-ntp-server fake-ntp-client

# Compiler flags
CFLAGS = -O1 -Wall -Wstrict-prototypes -Wmissing-prototypes

all: \
	$(PROGS)

clean:
	rm $(PROGS)

fake-samba: fake.h fake-samba.c
	cc $(CFLAGS) -g -o fake-samba fake-samba.c

fake-ntp-server: fake.h fake-ntp-server.c
	cc $(CFLAGS) -g -o fake-ntp-server fake-ntp-server.c

fake-ntp-client: fake.h fake-ntp-client.c
	cc $(CFLAGS) -g -o fake-ntp-client fake-ntp-client.c


