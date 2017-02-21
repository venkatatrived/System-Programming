#include <linux/version.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#include <linux/list.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

/* =============================Start_ Definitions/Declarations=================================== */
#define MYDEV_NAME "mycdrv"

#define ramdisk_size (size_t) (16 * PAGE_SIZE) // ramdisk size 

//data access directions
#define REGULAR 0
#define REVERSE 1

#define CDRV_IOC_MAGIC 'Z'
#define ASP_CHGACCDIR _IOW(CDRV_IOC_MAGIC, 1, int)

#ifndef MYCDRV_MAJOR
#define MYCDRV_MAJOR 0   /* dynamic major by default */
#endif

#define CDRV_IOC_MAGIC 'Z'
#define ASP_CHGACCDIR _IOW(CDRV_IOC_MAGIC, 1, int)

#define container_of(ptr, type, member) ({ const typeof( ((type *)0)->member ) *__mptr = (ptr); (type *)( (char *)__mptr - offsetof(type,member) );})

struct ASP_mycdrv 
{
	struct list_head list;
	struct cdev dev;
	char *ramdisk;
	struct semaphore sem;
	int devNo;
	int direction;
};


static unsigned int mycdrv_major = MYCDRV_MAJOR;
static struct ASP_mycdrv *mycdrv_devices = NULL;
static struct class *mycdrv_class = NULL;
static int NUM_DEVICES = 3; /* mycdrv0 through mycdrv3 */
module_param(NUM_DEVICES, int, S_IRUGO);
/* =============================END_Definitions/Declarations============================== */

/* ===========================start_mycdrv_read=============================== */
static ssize_t mycdrv_read(struct file *filp, char __user *buf, size_t lbuf,
                loff_t *ppos)
{
	int maxbytes = 0;
	int bytes_to_do = 0;
	int i = 0;
	struct ASP_mycdrv *dev = filp->private_data; 
	ssize_t retval = -ENOMEM;

	printk(KERN_EMERG "Entered mycdrv_read\n");

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if(dev->direction == REGULAR)
	{
		maxbytes = ramdisk_size - *ppos;
		bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
		if (bytes_to_do == 0)
		{
			printk(KERN_EMERG "Reached end of the device on a read \n");
			retval = -EINVAL;
			goto out;
		}
		if (copy_to_user(buf, dev->ramdisk+ *ppos, bytes_to_do))
		{
			retval = -EFAULT;
			goto out;
		}
		*ppos += bytes_to_do;
	}
	else if(dev->direction == REVERSE)
	{
		maxbytes = *ppos;
		bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;

		if (bytes_to_do == 0)
		{
			printk(KERN_EMERG "Reached end of device on a read in reverse direction \n");
			retval = -EINVAL;
			goto out;
		}

		for(i=0; i<bytes_to_do; i++)
		{	
			if (copy_to_user(buf+i, dev->ramdisk+ *ppos, 1))
			{
				*ppos +=1;	
				retval = -EFAULT;
				goto out;
			}
			*ppos -=1;
		}
	}
	printk(KERN_EMERG "Exiting the mycdrv_read function, nbytes= %d pos=%d successfully\n", bytes_to_do, (int)*ppos);
	up(&dev->sem);
	return bytes_to_do;
out:
	up(&dev->sem);
	printk(KERN_EMERG "Exiting the mycdrv_read function not successfully\n");
	return retval;
}
/* ===========================end_mycdrv_read=============================== */

/* ===========================start_mycdrv_write=============================== */
ssize_t mycdrv_write(struct file *filp, const char __user *buf, size_t lbuf,
                loff_t *ppos)
{

	int maxbytes = 0;
	int bytes_to_do = 0;
	int i = 0;
	struct ASP_mycdrv *dev = filp->private_data; 
	ssize_t retval = -ENOMEM;

	printk(KERN_EMERG "Entered mycdrv_write\n");

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if(dev->direction == REGULAR)
	{
		maxbytes = ramdisk_size - *ppos;
		bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
		//bytes_to_do = maxbytes > lbuf ? lbuf : 0;
		if (bytes_to_do == 0)
		{
			printk(KERN_EMERG "Reached end of the device on a write \n");
			retval = -EINVAL;
			up(&dev->sem);
			return retval;
		}

		if (copy_from_user(dev->ramdisk + *ppos, buf, bytes_to_do))
		{
			retval = -EFAULT;
			goto out;
		}
		*ppos += bytes_to_do;
	}
	else if(dev->direction == REVERSE)
	{
		maxbytes = *ppos;
		bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
		//bytes_to_do = maxbytes > lbuf ? lbuf : 0;
		if (bytes_to_do == 0)
		{
			printk(KERN_EMERG "Reached start of the device on a write in reverse direction\n");
			retval = -EINVAL;
			goto out;
		}

		for(i=0; i<bytes_to_do; i++)
		{	
			if (copy_from_user(dev->ramdisk +*ppos, buf+i, 1))
			{
				*ppos +=1;	
				retval = -EFAULT;
				goto out;
			}
			*ppos -=1;
		}
	}
	
	printk(KERN_EMERG "Exiting the WRITE function, nbytes=%d pos=%d successfully\n", bytes_to_do, (int)*ppos);
	up(&dev->sem);
	return bytes_to_do;

out:
	up(&dev->sem);
	printk(KERN_EMERG "Exiting mycdrv_write not successfully\n");
	return retval;

}
/* ===========================end_mycdrv_write=============================== */


/* ===========================start_mycdrv_ioctl=============================== */
static long mycdrv_ioctl(struct file *filp, unsigned int cmd, unsigned long dir)
{
	struct ASP_mycdrv *dev = filp->private_data;
	int retval = 0;

	printk(KERN_EMERG "Entered mycdrv_ioctl\n");
	if (_IOC_TYPE(cmd) != CDRV_IOC_MAGIC) 
		return -ENOTTY;

	if (_IOC_NR(cmd) > REVERSE) 
		return -ENOTTY;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	switch(dir) 
	{

		case REGULAR:
			dev->direction = REGULAR;
			printk(KERN_EMERG "Setting direction to REGULAR \n");
			break;

		case REVERSE:
			dev->direction = REVERSE;
			printk(KERN_EMERG "Setting direction to REVERSE \n");
			break;

		default:
			up(&dev->sem);
			printk(KERN_EMERG "Invalid Direction\n");
			return -ENOTTY;
	}
	up(&dev->sem);
	printk(KERN_EMERG "Exiting mycdrv_ioctl successfully\n");	
	return retval;
}
/* ===========================end_mycdrv_ioctl=============================== */

/* ===========================start_mycdrv_llseek=============================== */

loff_t mycdrv_llseek(struct file *filp, loff_t offset, int whence)
{
	struct ASP_mycdrv *dev = filp->private_data;
	loff_t newposition;

	printk(KERN_EMERG "Entered mycdrv_llseek\n");
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	switch(whence) 
	{
		case SEEK_SET:
			newposition = offset;
			printk(KERN_EMERG "Setting newpositon to  SEEK_SET %d\n", (int)newposition);
			break;

		case SEEK_CUR:
			newposition = filp->f_pos + offset;
			printk(KERN_EMERG "Inside  for SEEK_CUR %d\n", (int)newposition);
			break;

		case SEEK_END:
			newposition = ramdisk_size + offset;
			printk(KERN_EMERG "Inside SEEK END %d\n", (int)newposition);
			break;

		default: 
			up(&dev->sem);
			printk(KERN_EMERG "Invalid Origin\n");
			return -EINVAL;
		
	}
	if (newposition < 0 ) 
	{
		up(&dev->sem);
		printk(KERN_EMERG "New Position is Out of bound\n");
		return -2;
	}

	filp->f_pos = newposition;
	up(&dev->sem);
	printk(KERN_EMERG "Exiting mycdrv_llseek successfully\n");
	return newposition;
}
/* ===========================end_mycdrv_llseek=============================== */

/* ===========================start_mycdrv_open=============================== */
static int mycdrv_open(struct inode *inode, struct file *filp)
{
	struct ASP_mycdrv *dev;

	dev = container_of(inode->i_cdev, struct ASP_mycdrv, dev);
	filp->private_data = dev;
	printk(KERN_EMERG "Executed mycdrv_open successfully\n");
	return 0;
}
/* ===========================end_mycdrv_open=============================== */

/* ===========================start_mycdrv_close=============================== */

static int mycdrv_close(struct inode *inode, struct file *filp)
{
	printk(KERN_EMERG "Executed mycdrv_close successfully\n");
	return 0;
}

/* ===========================end_mycdrv_close=============================== */

/* ===========================Start_fileops_structure=============================== */

struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.open = mycdrv_open,
	.release = mycdrv_close,
	.read = mycdrv_read,
	.write = mycdrv_write,
	.llseek = mycdrv_llseek,
	.unlocked_ioctl = mycdrv_ioctl,
};
/* ===========================End_fileops_structure=============================== */


/* ===========================start_mycdrv_cleanup_module=============================== */
static void mycdrv_cleanup_module(int devices_to_destroy)
{
	int i=0;
	printk(KERN_EMERG "Entered mycdrv_cleanup_module\n");
	if (mycdrv_devices) 
	{
		for (i = 0; i < devices_to_destroy; ++i) 
		{
			printk(KERN_EMERG "Cleaning up device %d resources\n", i);
			device_destroy(mycdrv_class, MKDEV(mycdrv_major, i));
			cdev_del(&mycdrv_devices[i].dev);
			if(mycdrv_devices[i].ramdisk)
			{
				printk(KERN_EMERG "freeing ramdisk memory of  device %d \n", i);
				kfree(mycdrv_devices[i].ramdisk);
			}
		}
		
		if (mycdrv_devices) 
		{
			printk(KERN_EMERG "Cleaning up all devices Structures%d \n", i);
			kfree(mycdrv_devices);
		}
	}
	
	if (mycdrv_class)
	{
		printk(KERN_EMERG "Inside cleaning of mycdrv_class in cleanup Module %d \n", i);
		class_destroy(mycdrv_class);
	}

	unregister_chrdev_region(MKDEV(mycdrv_major, 0), NUM_DEVICES);
	printk(KERN_EMERG "Exiting mycdrv_cleanup_module successfully\n");
	return;
}

/* ===========================end_mycdrv_cleanup_module=============================== */

/* ===========================start_mycdrv_setup_dev=============================== */
static int mycdrv_setup_dev(struct ASP_mycdrv *mycdrv, int index, struct class *mycdrvclass)
{
	int result = 0;
	struct device *device = NULL;
	int minor = index;
	dev_t devno = MKDEV(mycdrv_major, minor);

    printk(KERN_EMERG "Entered mycdrv_setup_dev for device %d \n", minor);

	cdev_init(&mycdrv->dev, &mycdrv_fops);
	mycdrv->dev.owner = THIS_MODULE;
	mycdrv->dev.ops = &mycdrv_fops;

	if (cdev_add (&mycdrv->dev, devno, 1) < 0)
	{
		cdev_del(&mycdrv->dev);
		if(mycdrv->ramdisk)
			kfree(mycdrv->ramdisk);
		printk(KERN_EMERG "device %d cdev_add() failed", minor);
		return -1;
	}

	device = device_create(mycdrvclass, NULL,  devno, NULL, MYDEV_NAME "%d", minor);

	if (IS_ERR(device)) 
	{
		result = PTR_ERR(device);
		printk(KERN_EMERG "Error %d while trying to create %s%d", result, MYDEV_NAME, minor);
		cdev_del(&mycdrv->dev);
		if(mycdrv->ramdisk)
			kfree(mycdrv->ramdisk);
		return result;
	}
	printk(KERN_EMERG "Exiting mycdrv_setup_dev successfully for device %d \n", minor);
	return 0;
}
/* ===========================end_mycdrv_setup_dev=============================== */

/* ===========================start_mycdrv_init_module=============================== */
static int __init mycdrv_init_module(void)
{

	int result = 0;
	int i=0;
	int noofDevices=0;
	dev_t dev = 0;

	printk(KERN_EMERG "Entered mycdrv_init_module\n");

	if (mycdrv_major) 
	{
		dev = MKDEV(mycdrv_major, 0);
		result = register_chrdev_region(dev, NUM_DEVICES, MYDEV_NAME);
	}
	else 
	{
		result = alloc_chrdev_region(&dev, 0, NUM_DEVICES, MYDEV_NAME);
	}

	if (result < 0) 
	{
		printk(KERN_EMERG "alloc_chrdev_region() failed\n");
		return result;
	}
	
	mycdrv_major = MAJOR(dev);
	printk(KERN_INFO "mycdrv: major number : %d", mycdrv_major);

	mycdrv_class = class_create(THIS_MODULE, MYDEV_NAME);
	if(IS_ERR(mycdrv_class))
		return -1;

	printk(KERN_EMERG "Creating Structure for given no of devices \n");
	mycdrv_devices = kmalloc(NUM_DEVICES * sizeof(struct ASP_mycdrv), GFP_KERNEL);
	if (!mycdrv_devices) 
	{
		printk(KERN_EMERG "Creation of Devices Structure failed \n");
		result = -ENOMEM;
		if(mycdrv_class)
			class_destroy(mycdrv_class);
		unregister_chrdev_region(MKDEV(mycdrv_major, 0), NUM_DEVICES);
		return result;
	}

	memset(mycdrv_devices, 0, NUM_DEVICES * sizeof(struct ASP_mycdrv));

	printk(KERN_EMERG "Initializing device Structure of device %d \n", i);
	mycdrv_devices[0].ramdisk = kmalloc(ramdisk_size, GFP_KERNEL);
	if (!mycdrv_devices[0].ramdisk) 
	{
		printk(KERN_EMERG "Failed Allocating memory ramdisk for device %d\n", i);
		result = -ENOMEM;
		noofDevices = 1;
		goto failure;
	}
	memset(mycdrv_devices[0].ramdisk, 0, ramdisk_size);
	mycdrv_devices[0].devNo = 0;
	mycdrv_devices[0].direction = REGULAR;
    mycdrv_devices[0].list.next = &mycdrv_devices[0].list;
    mycdrv_devices[0].list.prev = &mycdrv_devices[0].list;
    sema_init(&mycdrv_devices[0].sem,1);
	result = mycdrv_setup_dev(&mycdrv_devices[0], 0, mycdrv_class);
	if(result)
	{
		noofDevices = 1;
		printk(KERN_EMERG "Initiaing cleanup module");
		goto failure;
	}

	for (i = 1; i < NUM_DEVICES; i++) 
	{
		printk(KERN_EMERG "Initializing setup of device Structure%d \n", i);
		mycdrv_devices[i].ramdisk = kmalloc(ramdisk_size, GFP_KERNEL);
		if (!mycdrv_devices[i].ramdisk) 
		{
			printk(KERN_EMERG "Failed Allocating memory ramdisk for device %d\n", i);
			result = -ENOMEM;
			noofDevices = i;
			goto failure;
		}
		memset(mycdrv_devices[i].ramdisk, 0, ramdisk_size);
		mycdrv_devices[i].devNo = i;
		mycdrv_devices[i].direction = REGULAR;
    	list_add ( &mycdrv_devices[i].list , &mycdrv_devices[0].list) ;
		sema_init(&mycdrv_devices[i].sem,1);
		result = mycdrv_setup_dev(&mycdrv_devices[i], i, mycdrv_class);
		if(result)
		{
			noofDevices = i;
			if(mycdrv_devices[i].ramdisk)
				kfree(mycdrv_devices[i].ramdisk);
			printk(KERN_EMERG "Initiaing cleanup module");

		}
	}
	printk(KERN_EMERG "Exiting mycdrv_init_module successfully\n");
	return 0; 

failure:
	mycdrv_cleanup_module(noofDevices);
	printk(KERN_EMERG "Exiting mycdrv_init_module not successfully\n");
	return result;

}
/* ===========================end_mycdrv_init_module=============================== */

/* ===========================start_mycdrv_exit_module=============================== */
static void __exit mycdrv_exit_module(void)
{
	printk(KERN_EMERG "Initiating mycdrv_cleanup_module");
	mycdrv_cleanup_module(NUM_DEVICES);
	return;
}

/* ===========================end_mycdrv_exit_module=============================== */

module_init(mycdrv_init_module);
module_exit(mycdrv_exit_module);