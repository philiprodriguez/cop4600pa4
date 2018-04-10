# Programming Assignment 4
Programming assignment 4 for COP4600. (Steven, Ryan, Philip)

# Usage
The usage is practically identical to that of assignment 3. There are two files, write_device.c and read_device.c, which you can compile into kernel modules using the provided Makefile file. This will yield two files, write_device.ko and read_device.ko. You can then use insmod to install these and rmmod to remove them. When using insmod to install the kernel modules, you MUST install the write_device.ko file first, and then the read_device.ko file. This is because the read_device.ko module depends on shared memory offered by the write device. After that, you should be able to send the device bytes, and if "UCF" is ever sent into the device without intermediate flushing of the deivce buffer, then it will be converted to "Undefeated 2018 National Champions UCF". Note that you must remove the read device before the write device for similar reasons to those mentioned above. Below is an example usage of the kernel modules:

```

root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# make
make -C /lib/modules/4.4.0-116-generic/build M=/home/cop4600/Documents/pa4 modules
make[1]: Entering directory `/usr/src/linux-headers-4.4.0-116-generic'
  CC [M]  /home/cop4600/Documents/pa4/write_device.o
  Building modules, stage 2.
  MODPOST 2 modules
  LD [M]  /home/cop4600/Documents/pa4/write_device.ko
make[1]: Leaving directory `/usr/src/linux-headers-4.4.0-116-generic'
root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# insmod write_device.ko
root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# insmod read_device.ko
root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# dmesg | tail -8
[ 5371.457511] Initializing the FIFO write device...
[ 5371.457515] Registered FIFO write device with major number 246.
[ 5371.457527] Created FIFO write device class.
[ 5371.458774] Successfully created FIFO write device!
[ 5378.800729] Initializing the FIFO read device...
[ 5378.800733] Registered FIFO read device with major number 245.
[ 5378.800740] Created FIFO read device class.
[ 5378.803510] Successfully created FIFO read device!
root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# echo "Go UCF." > /dev/fifowritedev
root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# echo "I think UCF has a good CS program." > /dev/fifowritedev
root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# cat /dev/fiforeaddev
Go Undefeated 2018 National Champions UCF.
I think Undefeated 2018 National Champions UCF has a good CS program.
root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# dmesg | tail -10
[ 5434.105067] FIFO write device opened.
[ 5434.105098] 8 bytes sent to FIFO write device, 8 bytes written.
[ 5434.105101] FIFO write device closed.
[ 5497.652234] FIFO write device opened.
[ 5497.652245] 35 bytes sent to FIFO write device, 35 bytes written.
[ 5497.652248] FIFO write device closed.
[ 5511.718574] FIFO read device opened.
[ 5511.718584] 113 bytes read from FIFO read device.
[ 5511.718588] 0 bytes read from FIFO read device.
[ 5511.718591] FIFO read device closed.
root@cop4600-VirtualBox:/home/cop4600/Documents/pa4# 

```

# Authors

This module was written by Philip Rodriguez, Steven Chen, and Ryan Beck.
