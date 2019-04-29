#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WheelTimer.h"
#include "LinkedListApi.h"
#include <memory.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>


wheel_timer_t* init_wheel_timer(int wheel_size, int wheel_needle_interval){

  wheel_timer_t* wt = calloc(1, sizeof(wheel_timer_t) +
                             wheel_size* sizeof(ll_t*));
  
  wt->wheel_needle_interval = wheel_needle_interval;
  wt->wheel_size = wheel_size;
  //wt->current_cycle_no = 0;
  //wt->current_wheel_needle =0;

  memset(&(wt->wheel_thread), 0, sizeof(wheel_timer_t));
  
  int i;
  for(i=0; i < wheel_size; i++)
       wt->slots[i] = init_singly_ll();
       
  return wt;
}


static void* wheel_fn(void* arg){

    wheel_timer_t *wt = (wheel_timer_t *)arg;
    int absolute_slot_no = 0, i =0;
    slot_event_elem* elem;
    ll_t* slot_list= NULL;
    singly_ll_node_t* head1 = NULL, *prev_node = NULL;

    while(1) 
    {  
       wt->current_wheel_needle++;
       if(wt->current_wheel_needle == wt->wheel_size)
              wt->current_wheel_needle = 0;
            
           if(wt->current_wheel_needle == 0)
                wt->current_cycle_no++;          
             
       sleep(wt->wheel_needle_interval);

       slot_list = wt->slots[wt->current_wheel_needle];
       absolute_slot_no = GET_WT_CURRENT_ABS_SLOT_NO(wt);
       printf("Wheel Timer Time = %d:\n", absolute_slot_no * wt->wheel_needle_interval);
       if(is_singly_ll_empty(slot_list))
	   printf("\n");
           
       ITERATE_LIST_BEGIN2(slot_list, head1, prev_node){
       
          
            elem = (slot_event_elem *)head1->data;
            if(elem -> execution_cycle_no ==  wt->current_cycle_no)
               {
                   elem-> app_callback(elem->args, elem->size_of_args);
                   if(elem->is_recurrence)
                      {
                         int slot_no_for_elem  =  absolute_slot_no  + (elem->time_interval/wt->wheel_needle_interval);
                         int next_slot_no      =  slot_no_for_elem % wt->wheel_size;
                         int cycle_no          =  slot_no_for_elem/ wt->wheel_size;
                         elem -> execution_cycle_no = cycle_no;
                         elem -> slot_number = next_slot_no;

                         if(next_slot_no == wt->current_wheel_needle){
                                ITERATE_LIST_CONTINUE2(slot_list, head1, prev_node);
			    } 
                         singly_ll_remove_node(slot_list, head1);
                         singly_ll_add_node(wt->slots[next_slot_no], head1);

                      }
                    else
                      {
      			 free_wheel_timer_element((slot_event_elem*) head1->data);
			 singly_ll_delete_node(slot_list, head1);
                      }
                       
               } 
            
              //head1 = head1->next;
             
         } ITERATE_LIST_END2(slot_list, head1, prev_node);   //ITERATE_LIST_END
       
     }
     return NULL;
}




slot_event_elem* register_app_event(wheel_timer_t* wt, app_call_back app_callback, 
                        void* args, int arg_size, int time_interval,
                        int is_recursive)
{
  if(!wt || !app_callback) return NULL;
  slot_event_elem *elem = calloc(1, sizeof(slot_event_elem));

  elem -> time_interval =  time_interval;
  elem -> app_callback  =  app_callback;
  elem -> args 	        =  calloc(1, arg_size);
  memcpy(elem->args, args, arg_size);
  elem -> size_of_args  =  arg_size;
  elem -> is_recurrence =  is_recursive;

  int wt_absolute_slot  =  GET_WT_CURRENT_ABS_SLOT_NO(wt);
  int slot_no_for_elem  =  wt_absolute_slot + (elem->time_interval/wt->wheel_needle_interval);
  int slot_no           =  slot_no_for_elem % wt->wheel_size;
  int cycle_no          =  slot_no_for_elem/ wt->wheel_size;
  elem -> execution_cycle_no = cycle_no;
  elem -> slot_number = slot_no;
  singly_ll_add_node_by_val(wt->slots[slot_no], elem);
  return elem;
}


void
de_register_app_event(wheel_timer_t *wt, struct _wheeel_timer_slot_event *wt_elem){

   int slot_num = wt_elem -> slot_number;
   ll_t* slot_list = wt->slots[slot_num];
   singly_ll_delete_node_by_data_ptr(slot_list,wt_elem);
   
}



void free_wheel_timer_element(slot_event_elem* elem){
	free(elem->args);
	free(elem);
}



void start_wheel_timer(wheel_timer_t *wt){

	if (pthread_create(&wt->wheel_thread, NULL, wheel_fn, (void*)wt))
	{
		printf("Wheel Timer Thread initialization failed, exiting ... \n");
		exit(0);
	}
}


void reset_wheel_timer(wheel_timer_t *wt){
	wt->current_wheel_needle = 0;
	wt->current_cycle_no  = 0;
}











