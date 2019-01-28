/*
 * nts.h - NTS (Network Time Security) declarations 
 */
#ifndef NTS_H
#define NTS_H

struct nts_client_t;

int nts_client_ke_request(void);
int nts_server_ke_verify(void);
int nts_client_ke_verify(struct nts_client_t *);
int nts_daily(void);
int nts_validate(struct parsed_pkt *, struct nts_client_t *);
int nts_decorate(struct parsed_pkt *, struct nts_client_t *);

#endif	/* NTS_H */
