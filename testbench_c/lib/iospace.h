#ifndef IOSPACE_H_
#define IOSPACE_H_

#define IOSPACE 0x5000

typedef struct IO {
	struct {
		char enDev;
		char enWr;
		char enRd;
		char status;
		char wr;
		char wrRdy;
		char rd;
		char rdRdy;
	} VMConsole;
	struct {
		char enDev;
		char enTimer;
		char status;
		unsigned int period;
	} Timer;
} io_t;

static io_t * io __data = (io_t*)IOSPACE;

#endif