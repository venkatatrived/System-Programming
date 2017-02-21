echo -n "1-2:1.0" > /sys/bus/usb/drivers/usbhid/unbind
insmod usbkbd.ko

