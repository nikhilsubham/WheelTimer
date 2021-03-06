#ifndef __WHEEL_TIMER__
#define __WHEEL_TIMER__

#include <pthread.h>
#include "LinkedListApi.h"

typedef struct _wheeel_timer_slot_event slot_event_elem;
typedef void (*app_call_back)(void *arg, int sizeof_arg);

struct _wheeel_timer_slot_event{
     int time_interval;
     int execution_cycle_no;
     int slot_number;
     app_call_back app_callback;
     void* args;
     int size_of_args;
     int is_recurrence;
};

typedef struct _wheel_timer_t{
     int wheel_needle_interval;
     int wheel_size;
     int current_wheel_needle;
     int current_cycle_no;
     pthread_t wheel_thread; 
     ll_t* slots[0];
}wheel_timer_t;

wheel_timer_t* init_wheel_timer(int clock_needle_interval, int wheel_size);

#define GET_WT_CURRENT_ABS_SLOT_NO(wt)	((wt->current_cycle_no * wt->wheel_size) + wt->current_wheel_needle)

slot_event_elem* register_app_event(wheel_timer_t* wt, app_call_back app_callback, 
                        void* args, int arg_size, int time_interval,
                        int is_recursive);

void start_wheel_timer(wheel_timer_t *wt);

//static void* wheel_fn(void* arg);

void free_wheel_timer_element(slot_event_elem *elem);

void reset_wheel_timer(wheel_timer_t *wt);

#endif
