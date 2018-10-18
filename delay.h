/**
 * sleep usleep nanosleep clock_nanosleep select
 * https://cloud.tencent.com/developer/article/1008495
 */
static inline void delay_us(unsigned us){
   //struct timespec res;
   //res.tv_sec = us/1000000;
   //res.tv_nsec = (us * 1000) % 1000000;
   //clock_nanosleep(CLOCK_MONOTONIC,0, &res, NULL);
    usleep(us);
}
static inline void delay_ms(unsigned ms){
    delay_us(ms * 1000);
}