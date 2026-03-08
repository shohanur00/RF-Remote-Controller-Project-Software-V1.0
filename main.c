#include "stm32f401xc.h"
#include "app.h"

int main(void){
	
	App_Setup();
	
	while(1){
		
		App_Main_Loop();
	}
	
}