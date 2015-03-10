/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

int num_tests = 0;
int num_tests_passed = 0;
int num_tests_failed = 0;

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[3].mpf_start_pc = &proc4;
	g_test_procs[4].mpf_start_pc = &proc5;
	g_test_procs[5].mpf_start_pc = &proc6;
	g_test_procs[6].mpf_start_pc = &proc_A;
	g_test_procs[7].mpf_start_pc = &proc_B;
	g_test_procs[8].mpf_start_pc = &proc_C;
	g_test_procs[9].mpf_start_pc = &set_process_priority_process;
	g_test_procs[10].mpf_start_pc = &wall_clock_display;
	g_test_procs[11].mpf_start_pc = &proc_KCD;
	g_test_procs[12].mpf_start_pc = &proc_CRT;
}


/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */
void proc1(void)
{
	int i;
	
	set_process_priority(1, HIGH);
	for ( i = 1; i < 6; i++) {
		set_process_priority(i + 1, LOW);
	}
#ifdef DEBUG_0
	printf("G019_test: START\r\n");
#endif
	set_process_priority(1, LOWEST);
	
#ifdef DEBUG_0
	printf("G019_test: %d/%d tests OK\r\n", num_tests_passed, num_tests);
	printf("G019_test: %d/%d tests FAIL\r\n", num_tests_failed, num_tests);
	printf("G019_test: END\r\n");
#endif
	
	while (1) {};
}
	
void proc2(void)
{
	int prio_6;
	
	set_process_priority(6, MEDIUM);
	prio_6 = get_process_priority(6);
	num_tests++;
	if (prio_6 == MEDIUM) {
#ifdef DEBUG_0
		printf("G019_test: test %d OK\r\n", 1);
#endif
		num_tests_passed++;
	} else {
#ifdef DEBUG_0
		printf("G019_test: test %d FAIL\r\n", 1);
#endif
		num_tests_failed++;
	}
	
	set_process_priority(2, LOWEST);
	while (1) {};
}

void proc6(void)
{
	int prio = get_process_priority(6);
	
	set_process_priority(2, HIGH);
	
	num_tests++;
	
	if (prio != MEDIUM) {
#ifdef DEBUG_0
		printf("G019_test: test %d FAIL\r\n", 3);
#endif
		num_tests_failed++;
		set_process_priority(6, LOWEST);
	}
	
	set_process_priority(6, LOW);
	
	prio = get_process_priority(6);
	if (prio != LOW) {
#ifdef DEBUG_0
		printf("G019_test: test %d FAIL\r\n", 3);
#endif
		num_tests_failed++;
		set_process_priority(6, LOWEST);
	}
	
#ifdef DEBUG_0
	printf("G019_test: test %d OK\r\n", 3);
#endif
	num_tests_passed++;

	set_process_priority(6, LOWEST);
	while (1) {};
}

void proc3(void) {
	int i;
	int *first = request_memory_block();
	int *elem = first;
	
	for (i = 0; i < 10; i++){
		*elem = i;
		elem++;
	}
	
	num_tests++;
	
	if (0 == release_memory_block(first)){
#ifdef DEBUG_0
		printf("G019_test: test %d OK\r\n", 2);
#endif
		num_tests_passed++;
	}
	else {
#ifdef DEBUG_0
		printf("G019_test: test %d FAIL\r\n", 2);
#endif
		num_tests_failed++;
	}
	
	set_process_priority(3, LOWEST);
	while (1) {}
}

/*
 * test 4, requires 2 processes
 */

int maxed_out_mem = 0;
int has_preempted = 1;

struct llnode {
  struct llnode *next;
  int data;
};

int free_ll(struct llnode *l) {
	struct llnode *tmp;
	while (l) {
		tmp = l->next;
		k_release_memory_block(l);
		l = tmp;
	}
	return 1;
}

void proc4(void) {
  int cache_size = 8;
  void *cache[8];
  int i;

  for ( i = 0; i < cache_size; i++ ) {
    cache[i] = request_memory_block();
  }

  set_process_priority(5, HIGH);

  maxed_out_mem = 1;

  for ( i = 0; i < cache_size; i++ ) {
    release_memory_block(cache[i]);
		has_preempted = 0;
  }

  set_process_priority(4, LOWEST);
	while (1) {};
}

void proc5(void) {
  struct llnode *first = request_memory_block();
  struct llnode *tmp;

  do {
    tmp = request_memory_block();
    tmp->next = first;
    first = tmp;
  } while (!maxed_out_mem);

#ifdef DEBUG_0
  num_tests++;
	free_ll(first);
  if (has_preempted != 1) {
    printf("G019_test: test %d FAIL\r\n", 4);
    num_tests_failed++;
  } else {
    printf("G019_test: test %d OK\r\n", 4);
    num_tests_passed++;
  }
#endif

  set_process_priority(5, LOWEST);
	while (1) {};
}


void proc_A(void) { while (1) {} }
void proc_B(void) { while (1) {} }
void proc_C(void) { while (1) {} }
void set_process_priority_process(void) { while (1) {} }
void wall_clock_display(void) { while (1) {} }
void proc_KCD(void) { while (1) {} }
void proc_CRT(void) { while (1) {} }
