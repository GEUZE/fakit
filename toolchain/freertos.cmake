set(FREERTOS_SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/../library/freertos/source/tasks.c"
	"${CMAKE_CURRENT_LIST_DIR}/../library/freertos/source/queue.c"
	"${CMAKE_CURRENT_LIST_DIR}/../library/freertos/source/timers.c"
	"${CMAKE_CURRENT_LIST_DIR}/../library/freertos/source/list.c"
	"${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/freertos_port_CM4F/heap_2.c"
	"${CMAKE_CURRENT_LIST_DIR}/../library/rtos++/source/process.cpp")

set(FREERTOS_INCLUDE_DIRS
	"${CMAKE_CURRENT_LIST_DIR}/../library/freertos/include"
	"${CMAKE_CURRENT_LIST_DIR}/../library/rtos++/")
