obj-m := assignment5.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules

apps: 
	gcc -o userapp1 userapp_1.c -pthread
	gcc -o userapp2 userapp_2_deadlock_1.c
	gcc -o userapp3 userapp_3_deadlock_2.c -pthread
	gcc -o userapp4 userapp_4_deadlock_3.c -pthread
	gcc -o userapp5 userapp_5.c -pthread

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~ 
	rm -rf userapp1 userapp2 userapp3 userapp4 userapp5
