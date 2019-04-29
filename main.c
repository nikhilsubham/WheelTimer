#include <stdio.h>
#include "WheelTimer2.h"
#include "LinkedListApi.h"
#include <string.h>

#define WHEEL_SIZE 10
#define WHEEL_TIMER_CLOCK_TIC_INTERVAL 1

/* Application routine to be (indirectly) invoked by Wheel timer.
 * It could be of any prototype*/
void
print_hello(char *S){ 
    printf("%s\n", S);
}



void wrapper_print_hello(void *arg, int arg_size){
    char *S = (char *)arg;
    print_hello(S);
}

int
main(int argc, char **argv){

    /*create a wheel timer object*/
    wheel_timer_t *wt = init_wheel_timer(WHEEL_SIZE, WHEEL_TIMER_CLOCK_TIC_INTERVAL);
    /*start the wheel timer thread*/
    start_wheel_timer(wt);
     
    

    slot_event_elem * wt_elem = 
        register_app_event(wt, wrapper_print_hello, "MyString", 
                           strlen("MyString"), 
                           15,  /*wrapper_print_hello fn will be called after every 5 seconds*/
                           1); /*1 indefinitely, 0 only once : call for wrapper_print_hello*/

    wt_elem = 
        register_app_event(wt, wrapper_print_hello, "Nikhil", 
                           strlen("Nikhil"), 
                           12,  /*wrapper_print_hello fn will be called after every 5 seconds*/
                           1); /*1 indefinitely, 0 only once : call for wrapper_print_hello*/

    sleep(20);
    de_register_app_event(wt, wt_elem);
   
    scanf("\n");
    return 0;
}
