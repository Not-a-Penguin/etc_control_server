#include "utils.h"

double randn(double mu, double sigma){
    double U1, U2, W, mult;
    static double X1, X2;
    static int call = 0;
  
    if (call == 1){
        call = !call;
        return (mu + sigma * (double) X2);
      }
  
    do{
        U1 = -1 + ((double) rand () / RAND_MAX) * 2;
        U2 = -1 + ((double) rand () / RAND_MAX) * 2;
        W = pow (U1, 2) + pow (U2, 2);
      }
    while (W >= 1 || W == 0);
  
    mult = sqrt ((-2 * log (W)) / W);
    X1 = U1 * mult;
    X2 = U2 * mult;
  
    call = !call;
  
    return (mu + sigma * (double) X1);
  }
  
int ms_sleep(long msec){
  
    struct timespec ts;
    int res;

    if (msec < 0){
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do{
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
  }