/*
 * Our methods, one per linked module
 */
typedef enum {plain_text, raw, json} iomode;

extern void ppscheck(char *device);
extern void tickadj(const bool mode, const int tick);
extern void bumpclock(int);
extern void jitter(const iomode mode);
extern void stepback(void);
extern void precision(const iomode mode);

/*end */
