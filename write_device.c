/*
Philip Rodriguez
Steven Chen
Ryan Beck

Programming Assignment 4
*/

#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h> // Has copy to user function
#include <linux/delay.h>
#define DEVICE_NAME "fifowritedev"
#define CLASS_NAME "fifowrite"
#define BUFFER_SIZE 1024

static int majorNumber;
struct class * deviceClass;
struct device * deviceDevice;

MODULE_LICENSE("GPL");

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops =
{
	.open = dev_open,
	.write = dev_write,
	.release = dev_release
};

// This is our lock for all queue variables!
struct mutex queueMutex;
EXPORT_SYMBOL(queueMutex);

// The buffer in which we will store characters!
char queue[BUFFER_SIZE];
EXPORT_SYMBOL(queue);

// Represents the next open position in our message buffer.
short queueFirstByte;
EXPORT_SYMBOL(queueFirstByte);

// Represents the number of bytes currently in the queue.
short queueSize;
EXPORT_SYMBOL(queueSize);

int init_module(void)
{
	printk(KERN_INFO "Initializing the FIFO write device...\n");

  	mutex_init(&queueMutex);

	// Assign a major number
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

	// Did we succeed?
	if (majorNumber < 0)
	{
		// We failed.
		printk(KERN_ALERT "Failed to assign FIFO write device a major number!\n");
		return majorNumber;
	}
	// We succeeded!
	printk("Registered FIFO write device with major number %d.\n", majorNumber);

	// Register device class
	deviceClass = class_create(THIS_MODULE, CLASS_NAME);

	// Did we succeed?
	if (IS_ERR(deviceClass))
	{
		// Nope.
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create FIFO write device class!\n");
		return PTR_ERR(deviceClass);
	}
	// We succeeded!
	printk(KERN_INFO "Created FIFO write device class.\n");

	// Register the device driver
	deviceDevice = device_create(deviceClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

	// Did we succeed?
	if (IS_ERR(deviceDevice))
	{
		// Nope.
		class_destroy(deviceClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the FIFO write device!\n");
		return PTR_ERR(deviceDevice);
	}
	// We succeeded!
	printk(KERN_INFO "Successfully created FIFO write device!\n");

	// Initialize queueSize
	queueSize = 0;

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Cleaning up FIFO write device!\n");
 	mutex_destroy(&queueMutex);
	device_destroy(deviceClass, MKDEV(majorNumber, 0));
	class_unregister(deviceClass);
	class_destroy(deviceClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);

	printk(KERN_INFO "FIFO write device cleaned up!\n");
}

static int dev_open(struct inode * inodep, struct file * filep)
{
	printk(KERN_INFO "FIFO write device opened.\n");
	return 0;
}

static int dev_release(struct inode * inodep, struct file * filep)
{
	printk(KERN_INFO "FIFO write device closed.\n");
	return 0;
}

static ssize_t dev_write(struct file * filep, const char * buffer, size_t len, loff_t * offset)
{
	int buffer_iterator = 0;
	int bytes_written = 0;
	char temp[3];
	int templateIterator, templateSize=38;
	char template[] = "Undefeated 2018 National Champions UCF";


	// We need to make sure we can have the shared memory to ourselves!
	mutex_lock(&queueMutex);

	// While we have space in the queue and something left in the buffer to grab
	while (queueSize < BUFFER_SIZE && buffer_iterator < len)
	{
		// Write the character
		queue[(queueFirstByte+queueSize)%BUFFER_SIZE] = buffer[buffer_iterator];
		bytes_written++;
		buffer_iterator++;
		queueSize++;

		// Perform the UCF check if applicable
		if (queueSize >= 3)
		{
			// Load the last 3 inserted characters...
			temp[0] = queue[(queueFirstByte+queueSize-3)%BUFFER_SIZE];
			temp[1] = queue[(queueFirstByte+queueSize-2)%BUFFER_SIZE];
			temp[2] = queue[(queueFirstByte+queueSize-1)%BUFFER_SIZE];
			
			// Check them
			if (temp[0]=='U' && temp[1]=='C' && temp[2]=='F')
			{
				// We need to write over this thing! Back the queue up! //TODO: ask about bytes_written... how would retroactive work?
				queueSize -= 3;

				// While there is something in the template to read AND space in the queue...
				for (templateIterator=0; templateIterator < templateSize && queueSize < BUFFER_SIZE; templateIterator++)
				{
					queue[(queueFirstByte+queueSize)%BUFFER_SIZE] = template[templateIterator];
					queueSize++;
				}
			}			
		}
	}

	// Now that we are done modifying the shared stuff, we can unlock our mutex!
	mutex_unlock(&queueMutex);

	printk(KERN_INFO "%zu bytes sent to FIFO write device, %d bytes written.\n", len, bytes_written);
	return len;
}
