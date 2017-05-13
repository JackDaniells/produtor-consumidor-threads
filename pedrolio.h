#ifndef PEDROLIO_H_
#define PEDROLIO_H_

typedef int rock_t;
typedef int oil_t;

#define PROCESSORS_COUNT 8

extern rock_t pd_read();
extern oil_t pd_process(rock_t rock);
extern double pd_price(oil_t oil);
extern void pd_deliver(oil_t oil);

#endif /*PEDROLIO_H_*/
