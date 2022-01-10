#include<linux/types.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/module.h>
#include<linux/ioctl.h>
#include"control.h"

#define name cdevice

dev_t device;

int name_open(struct inode *inode,struct file *filp);
int name_release(struct inode *inode,struct file *filp);
ssize_t name_read(struct file*filp,char __user *Ubuff,size_t count,loff_t *offp);
ssize_t name_write(struct file*filp,const char __user *Ubuff,size_t count,loff_t *offp);
long name_ioctl(struct file *filp,unsigned int cmd,unsigned long arg);

struct file_operations fops =
{
    .open = name_open,
    .release = name_release,
    .read = name_read,
    .unlocked_ioctl=name_ioctl,
    .write = name_write
};

struct cdev *my_cdev;

static int chdev_init(void)
{
    int major,minor;
    int result;
    result=alloc_chrdev_region(&device,0,1,"cdevice");
    if(result<0)
    {
        printk(KERN_ALERT "this device region cannot be allocated\n");
        return (-1);
    }
    major=MAJOR(device);
    minor=MINOR(device);
    printk(KERN_ALERT "the major and minor number of device is %d %d",major,minor);
    
    my_cdev = cdev_alloc();
    my_cdev->ops = &fops;
    //cdev_init(my_cdev,&fops);

    result=cdev_add(my_cdev,device,1);
    if(result<0)
    {
        printk(KERN_ALERT "this device is not added to the kernel\n");
        unregister_chrdev_region(device,1);
        return(-1);
    }
    printk(KERN_ALERT "the device has been successfully added to the kernel\n");
    return 0;
}

static void chdev_exit(void)
{
    int major,minor;
    major=MAJOR(device);
    minor=MINOR(device);
    printk(KERN_ALERT "the major and minor number of device is %d %d",major,minor);
    printk(KERN_ALERT "UNREGISTERING THE DEVICE\n");
    unregister_chrdev_region(device,1);
    cdev_del(my_cdev);
    printk(KERN_ALERT "DELETING THE DEVICE\n");
   
}

int name_open(struct inode *inode,struct file *filp)
{
    printk(KERN_ALERT "thnks for opening the program...\n");
    return 0;
}

int name_release(struct inode *inode,struct file *filp)
{
    printk(KERN_ALERT "nice to meet u..bubye\n");
    return 0;
}

ssize_t name_read(struct file *filp,char __user *Ubuff,size_t count,loff_t *offp)
{
    char Kbuff[50]="this is kernel\n";
    ssize_t retn;
    unsigned long result;
    result=copy_to_user((char *)Ubuff,(char *)Kbuff,count);
    if(result==0)
    {
        printk(KERN_ALERT "entire data has been successfully read\n");
        return 0;
    }
    else if(result>0)
    {
        printk(KERN_ALERT "part of the data has been successfully read\n");
        retn=count-result;
        return retn;
    }
    else
    {
        printk(KERN_ALERT "error reading data from kernel\n");
        retn=-EFAULT;
        return retn;
    }
}


ssize_t name_write(struct file *filp,const char __user *Ubuff,size_t count,loff_t *offp)
{
    char Kbuff[50];
    ssize_t retn;
    unsigned long result;
    result=copy_from_user((char *)Kbuff,(char *)Ubuff,count);
    printk(KERN_ALERT "data written is %s\n",Kbuff);
    if(result==0)
    {
        printk(KERN_ALERT "entire data has been successfully written\n");
        return 0;
    }
    else if(result>0)
    {
        printk(KERN_ALERT "part of the data has been successfully written\n");
        retn=count-result;
        return retn;
    }
    else
    {
        printk(KERN_ALERT "error writing data to the kernel\n");
        retn=-EFAULT;
        return retn;
    }
}
long name_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
    unsigned int buff,stp_bits;
    char Ubuff[20];
    struct Defaultdata
    {
    int Baudrate;
    int stopbits;
    int parity;
    };
    struct Defaultdata Tosend;
    switch(cmd)
    {
        case SET_BAUDRATE:
        __get_user(buff,(int __user*)arg);
        printk(KERN_ALERT "BAUDRATE GIVEN BY USER IS:%d\n",buff);
        break;
        case SET_NO_STOPBITS:
        __get_user(stp_bits,(int __user*)arg);
        printk(KERN_ALERT"the stop bits given by user are:%d\n",stp_bits);
        break;
        case SET_DIRECTION_WRITE:
        printk(KERN_ALERT"direction set to write\n");
        break;
        case SET_DEFAULTDATA:
        copy_from_user(&Tosend,(struct Defaultdata *)arg,sizeof(Tosend));
        printk(KERN_ALERT"the baudrate from defaultdata is %d\n",Tosend.Baudrate);
        break;
        default:
        printk(KERN_ALERT"ENTER VALID COMMAND\n");
        return(-EINVAL);
    }
    return 0;
}

module_init(chdev_init);
module_exit(chdev_exit);