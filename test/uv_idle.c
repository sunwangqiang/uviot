#include <stdio.h>
#include <uv.h>
int counter = 0;

void wait_for_a_while(uv_idle_t* handle) {
	counter++;
	printf("%d ", counter);
	if (counter >= 1000){
		uv_idle_stop(handle);
        printf("\n");
	}
}

int main() {
	uv_idle_t idler;
    
	uv_idle_init(uv_default_loop(), &idler);
	uv_idle_start(&idler, wait_for_a_while);
    
	printf("start idling...\n");
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    
	uv_loop_close(uv_default_loop());
	return 0;
}

