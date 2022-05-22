#include <bsp/board.h>
#include <tusb.h>
#include "usb_descriptors.h"
#include "core1Tasks.h"

void setup()
{
	tusb_init();
	coreloop();
}

void coreloop()
{
	while (1)
	{
		tud_task();
	}
}