
#include <am.h>
#include <klib.h>
#include <rtthread.h>

static rt_thread_t current =NULL;

#define STACK_SIZE (4096 * 8)
#define ROUNDUP(a, sz)      ((((uintptr_t)a) + (sz) - 1) & ~((sz) - 1))
#define ROUNDDOWN(a, sz)    ((((uintptr_t)a)) & ~((sz) - 1))

static Context* ev_handler(Event e, Context *c) {
  switch (e.event) {
    case EVENT_YIELD: 
        rt_ubase_t *para = (rt_ubase_t *)current->user_data;
        rt_ubase_t to = para[0];
        rt_ubase_t from = para[1];
        if (from != 0) {
            *((Context **)from) = c;
        }
        c = *((Context **)to);
      break;
    case EVENT_IRQ_TIMER:
      //printf("EVENT_IRQ_TIMER......\n");
      break;
    default: printf("Unhandled event ID = %d\n", e.event); assert(0);
  }
  return c;
}

void __am_cte_init() {
  cte_init(ev_handler);
}

void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to) {
    rt_ubase_t tmp_user_data = 0;
    rt_ubase_t to_from[2] = {0, 0};

    current = rt_thread_self();

    tmp_user_data = current->user_data;
    to_from[0] = to;
    to_from[1] = from;
    current->user_data = (rt_ubase_t)to_from;

    yield();
    current->user_data = tmp_user_data;
}

void rt_hw_context_switch_to(rt_ubase_t to) {
    rt_hw_context_switch(0, to);
}

void rt_hw_context_switch_interrupt(void *context, rt_ubase_t from, rt_ubase_t to, struct rt_thread *to_thread) {
  assert(0);
}

static void wrap(void *arg) {
    rt_ubase_t *stack_top= (rt_ubase_t *)arg;
    rt_ubase_t entry = *stack_top;
    stack_top--;
    rt_ubase_t parameter = *stack_top;
    stack_top--;
    rt_ubase_t exit = *stack_top;

    // call
    ((void (*)(void *)) entry)((void *)parameter);
    ((void (*)()) exit) ();
}

// rt-thread_hardware
rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit) {
    uintptr_t aligned_stack_addr = ROUNDDOWN((uintptr_t)stack_addr, sizeof(uintptr_t));
    
    Area kstack = {
        .end = (void *)aligned_stack_addr,
        .start = (void *)((uint8_t *)aligned_stack_addr - STACK_SIZE)};

    // Three parameters passed with the stack
    rt_ubase_t *wrap_parameter = (rt_ubase_t*)((uint8_t *)kstack.end - sizeof(Context));

    // pass threee parameter
    rt_ubase_t *stack_top = (rt_ubase_t *)wrap_parameter;
    *stack_top = (rt_ubase_t)tentry;
    stack_top--;
    *stack_top = (rt_ubase_t)parameter;
    stack_top--;
    *stack_top = (rt_ubase_t)texit;

    Context *cp = (Context *)kcontext(kstack, wrap, (void *)wrap_parameter);
    return (rt_uint8_t *)cp;
}
