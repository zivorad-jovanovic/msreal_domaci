//Rad sa vise node_fajlova (poslednji zadatak)
//Zivorad Jovanovic EE81/2017

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#define BUFF_SIZE 40

MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

int ALU[15];
int pos = 0;
int endRead = 0;
unsigned char RegA,RegB,RegC,RegD;
unsigned char result = 0;
unsigned char carry = 0;
char komanda[3];
char binarno[8];
int i = 0;
int number_of_minors = 6;


int ALU_open(struct inode *pinode, struct file *pfile);
int ALU_close(struct inode *pinode, struct file *pfile);
ssize_t ALU_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t ALU_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = ALU_open,
	.read = ALU_read,
	.write = ALU_write,
	.release = ALU_close,
};


int ALU_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully opened file\n");
		return 0;
}

int ALU_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully closed file\n");
		return 0;
}

void make_bin(int dec)
{
	int i = 0;

	while(dec != 1)
	{

		if(dec % 2 == 0)
		 {
	        	dec /= 2;
	        	binarno[7-i] = '0';
		 }

		else
		 {
		       dec /= 2;
	               binarno[7-i] = '1';
		 }

		i++;

	}

	binarno[7-i] = '1';
	binarno[i] = '\0';
	
}


ssize_t ALU_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret = 0;
	char buff[BUFF_SIZE];
	long int len = 0;
	int minor = MINOR(pfile->f_inode->i_rdev);
	

	if (endRead)
	{
		endRead = 0;
		pos = 0;
		printk(KERN_INFO "Succesfully read from file\n");
		return 0;
	}

	if(minor == 5)
	{	
        
	 if(carry == 0)
         {
		 if(strcmp(komanda,"dec") == 0)
		 {
                       len = scnprintf(buff,BUFF_SIZE, "%d,carry = %d",result,carry);
		 }
		 
	 	 else if(strcmp(komanda,"hex") == 0)
		 {
			len = scnprintf(buff,BUFF_SIZE,"0x%x,carry = %d",result,carry);
		 }

		 else if(strcmp(komanda,"bin") == 0)
		 {
       			 make_bin((int)result);
			 len = scnprintf(buff,BUFF_SIZE,"0b%s,carry = %d",binarno,carry);
		 }

		 else
		 {
			 len = scnprintf(buff,BUFF_SIZE,"0x%x,carry = %d",result,carry);
		 }
		 	ret = copy_to_user(buffer,buff,len);
	 }

	 else
 	  {
        	len = scnprintf(buff,BUFF_SIZE, "Prekoracen opseg,carry = 1");
	        ret = copy_to_user(buffer,buff,len);
	     
	  }
	}

	else if(minor >= 0 && minor < 4)
	{
            switch(minor)
	    {
	    	case 0:
			len = scnprintf(buff,BUFF_SIZE,"0x%x\n",RegA);
			ret = copy_to_user(buffer,buff,len);
			break;
		case 1:
			len=scnprintf(buff,BUFF_SIZE,"0x%x\n",RegB);
			ret=copy_to_user(buffer,buff,len);
			break;
		case 2:
			len = scnprintf(buff,BUFF_SIZE,"0x%x\n",RegC);
			ret = copy_to_user(buffer,buff,len);
		        break;
		case 3:
			len = scnprintf(buff,BUFF_SIZE,"0x%x\n",RegC);
			ret = copy_to_user(buffer,buff,len);
			break;
	    
	    }
	}




	if(ret)
		return -EFAULT;
	
	pos++;

	if (pos == 1) {
		endRead = 1;
	}

	return len;
}

ssize_t ALU_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[BUFF_SIZE];
	char operacija[BUFF_SIZE];
	char format[BUFF_SIZE];
	int ret = 0,ret1 = 0,ret2 = 0;
	unsigned int heks_broj;
	char oznaka_registra;
	unsigned int dec_broj;
	int minor;
	char operator,prvi_operand,drugi_operand;
	unsigned char prvi = 0,drugi = 0;


	ret = copy_from_user(buff,buffer,length);
	oznaka_registra = buff[3];

	 minor = MINOR(pfile->f_inode->i_rdev);
	
   if(buffer[5] != '+' && buffer[5] != '-' && buffer[5] != '*' && buffer[5] != '/' )
   {   

	if(ret)
		return -EFAULT;

	buff[length-1] = 0;

   if(minor >= 0 && minor < 4)
   {	   
	   ret = sscanf(buff,"0x%x",&heks_broj);

	   switch(minor)
	   {
		    case 0:
		    RegA = heks_broj;
	            printk(KERN_INFO "U Reg%c je upisano 0x%x",oznaka_registra,heks_broj);
		    break;
		   
		    case 1:
		    RegB = heks_broj;
		    printk(KERN_INFO "U Reg%c je upisano 0x%x",oznaka_registra,heks_broj);
		    break;

		    case 2:
		    RegC = heks_broj;
		    printk(KERN_INFO "U Reg%c je upisano 0x%x",oznaka_registra,heks_broj);
		    break;

		    case 3:
		    RegD = heks_broj;
		    printk(KERN_INFO "U Reg%c je upisano 0x%x",oznaka_registra,heks_broj);
		    break;
	   
	   
	   }

	dec_broj = (unsigned int)heks_broj;



       if(ret==1)//one parameter parsed in sscanf
       {
		    
	   if(dec_broj <= 255)
	   {
           printk(KERN_INFO "Uspesno uneta heks vrednost 0x%x u reg%c\n",heks_broj,oznaka_registra);
	   }


	   else
	   {	  
	   printk(KERN_WARNING "Neispravno uneta heks vrednost");	
	   }
       }
	  
       else
       {
       printk(KERN_WARNING "Nepravilno uneta heks vrednost!!");		
       }
   
   
   }
   else if (minor == 4)
   {

       ret1 = copy_from_user(operacija,buffer,length);

       if(ret1)
	       return -EFAULT;

	operacija[length-1] = 0;

        operator = operacija[5];
        prvi_operand = operacija[3];
        drugi_operand = operacija[10];
       

	switch(prvi_operand)
	{
		case 'A':
		prvi = RegA;
		break;

		case 'B':
		prvi = RegB;
		break;

		case 'C':
		prvi = RegC;
		break;

		case 'D':
		prvi = RegD;
		break;

		default:
		printk(KERN_ERR "Neispravno uneti operandi!");
	
	}

	switch(drugi_operand)
	{
		case 'A':
		drugi = RegA;
		break;
	
		case 'B':
		drugi = RegB;
		break;

		case 'C':
		drugi = RegC;
		break;

		case 'D':
		drugi = RegD;
		break;

		default:
		printk(KERN_ERR "Neispravno uneti operandi!");
	}


        switch(operator)
          {
           case '+':
 	   result = (int)prvi + (int)drugi ;

	   if((int)prvi + (int)drugi > 255)
	   {
	   carry = 1;
	   printk(KERN_ERR "Prekoracen je opseg - rezultat nije validan!");
	   }

	   else
	   {
		   carry = 0;
		   printk(KERN_INFO "Rezultat je : %d ",result);
	   }
	   break;
	   
	   case '-':
	   result = (int)prvi - (int)drugi ;

	   if((int)prvi - (int)drugi < 0)
	   {
		   carry = 1;
		   printk(KERN_ERR "Prekoracen je opseg - rezultat nije validan!");
	   }

	   else
	   {
		   carry = 0;
		   printk(KERN_INFO "Rezultat je : %d ",result);
	   }

           break;
      
      	   case '*':
	   result = (int)prvi * (int)drugi ;

	   if((int)prvi * (int)drugi > 255)
	   {	   
	   carry = 1;
	   printk(KERN_ERR "Prekoracen je opseg - rezultat nije validan!");
	   }

	   else
	   {
		   carry = 0;
		   printk(KERN_INFO "Rezultat je : %d ",result);
	   }

	   break;

	   case '/':
	   carry = 0;
	   result = (int)prvi / (int)drugi ;
	   printk(KERN_INFO "Rezultat je : %d ",result);
	   break;

	   default:
	   printk(KERN_ERR "Neispravno uneta operacija!!");
      
        }

}
	ret2 = copy_from_user(format,buffer,length);

	if(ret2)
	return -EFAULT;

	format[length - 1] = 0;

	komanda[0] = format[7];
	komanda[1] = format[8];
	komanda[2] = format[9];

	if(strcmp(komanda,"dec") == 0)
	printk(KERN_WARNING "Zahtevas dec");

	else if (strcmp(komanda,"hex") == 0)
	printk(KERN_WARNING "Zahtevas hex");

	else if(strcmp(komanda,"bin") == 0)
	printk(KERN_WARNING "Zahtevas bin");

	else
	printk(KERN_WARNING "Nisi uneo kako valja");

	}

	return length;
}

static int __init ALU_init(void)
{
        int ret = 0;
	//char buff[BUFF_SIZE];
	int i1 = 0;
	int number_of_minors = 6;

	//Initialize array
	for (i1=0; i1<15; i1++)
		ALU[i1] = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, number_of_minors, "ALU");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "ALU_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
 

   my_device = device_create(my_class, NULL,MKDEV(MAJOR(my_dev_id),0), NULL, "ALU_regA");

 	if(my_device == NULL)
	{
	printk(KERN_INFO "failed to create device\n");
	goto fail_1;	
	}
	
	
	my_device = device_create(my_class,NULL,MKDEV(MAJOR(my_dev_id),1),NULL,"ALU_regB");

	if(my_device == NULL)
	{
	printk(KERN_INFO "failed to create device\n");
	goto fail_1;

	}
	
	my_device = device_create(my_class,NULL,MKDEV(MAJOR(my_dev_id),2),NULL,"ALU_regC");

	if(my_device == NULL)
	{
	printk(KERN_INFO "failed to create device\n");
	goto fail_1;
	}

	my_device = device_create(my_class,NULL,MKDEV(MAJOR(my_dev_id),3),NULL,"ALU_regD");

     if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }

     my_device = device_create(my_class,NULL,MKDEV(MAJOR(my_dev_id),4),NULL,"ALU_op");

     if(my_device == NULL)
     {
     printk(KERN_INFO "failed to create device \n");
     goto fail_1;
     
     }

     my_device = device_create(my_class,NULL,MKDEV(MAJOR(my_dev_id),5),NULL,"ALU_result");

     if(my_device == NULL)
     {
     printk(KERN_INFO "failed to create device \n");
     goto fail_1;
     }
 
     printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, number_of_minors);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit ALU_exit(void)
{
	int i = 0;

   cdev_del(my_cdev);

   for(i = 0; i < number_of_minors; i++)
   device_destroy(my_class,MKDEV(MAJOR(my_dev_id),i));

   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,number_of_minors);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(ALU_init);
module_exit(ALU_exit);
