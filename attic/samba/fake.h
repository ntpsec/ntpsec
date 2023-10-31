#include <stdint.h>

#define LEN_PKT_NOMAC 48
#define LEN_MAC 16

#define FAKE_SAMBA_SOCKET "/home/murray/fake-samba-socket/socket"

#define HACK_PACKET_ID 123
#define HACK_KEY_ID 3211
#define HACK_KEY "ABCDEFGHIJKLMNOP"

/* Microsoft says the key_id is little-endian
 * We don't care.  We just pass it around.
 */

/* each of these is preceeded by a length word, network byte order */
struct to_samba {
  uint32_t version;
  uint32_t op;
  uint32_t packet_id;
  uint32_t key_id;
  char pkt[LEN_PKT_NOMAC];
};

struct from_samba {
  uint32_t version;
  uint32_t op; 
  uint32_t packet_id;
  char pkt[LEN_PKT_NOMAC];
  uint32_t key_id;
  char mac[LEN_MAC];
};



struct ntp_time {
  uint32_t seconds;
  uint32_t fraction;
};

struct ntp_packet {
  uint32_t header;
  uint32_t root_delay;
  uint32_t root_dispersion;
  uint32_t ref_id;
  struct ntp_time t0, t1, t2, t3;
  uint32_t keyid;
  char auth[16];
};

