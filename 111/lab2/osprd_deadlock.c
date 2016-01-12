#include <linux/version.h>
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kernel.h>  /* printk() */
#include <linux/errno.h>   /* error codes */
#include <linux/types.h>   /* size_t */
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/wait.h>
#include <linux/file.h>

#include "spinlock.h"
#include "osprd.h"

/* The size of an OSPRD sector. */
#define SECTOR_SIZE	512
#define WRITE 0
#define READ 1
/* This flag is added to an OSPRD file's f_flags to indicate that the file
 * is locked. */
#define F_OSPRD_LOCKED	0x80000

/* eprintk() prints messages to the console.
 * (If working on a real Linux machine, change KERN_NOTICE to KERN_ALERT or
 * KERN_EMERG so that you are sure to see the messages.  By default, the
 * kernel does not print all messages to the console.  Levels like KERN_ALERT
 * and KERN_EMERG will make sure that you will see messages.) */
#define eprintk(format, ...) printk(KERN_NOTICE format, ## __VA_ARGS__)

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("CS 111 RAM Disk");
// EXERCISE: Pass your names into the kernel as the module's authors.
MODULE_AUTHOR("Qinyi Yan & Xiaoxuan Wang");

#define OSPRD_MAJOR	222

/* This module parameter controls how big the disk will be.
 * You can specify module parameters when you load the module,
 * as an argument to insmod: "insmod osprd.ko nsectors=4096" */
static int nsectors = 32;
module_param(nsectors, int, 0);

struct pid_list{
	pid_t pid;
	struct pid_list* next;
	struct pid_list* prev;
}pid_list;
typedef struct pid_list* pid_list_t;


/* The internal representation of our device. */
typedef struct osprd_info {
	uint8_t *data;                  // The data array. Its size is
	                                // (nsectors * SECTOR_SIZE) bytes.

	osp_spinlock_t mutex;           // Mutex for synchronizing access to
					// this block device

	unsigned ticket_head;		// Currently running ticket for
					// the device lock

	unsigned ticket_tail;		// Next available ticket for
					// the device lock

	wait_queue_head_t blockq;       // Wait queue for tasks blocked on
					// the device lock

	/* HINT: You may want to add additional fields to help
	         in detecting deadlock. */

	// The following elements are used internally; you don't need
	// to understand them.
	struct request_queue *queue;    // The device request queue.
	spinlock_t qlock;		// Used internally for mutual
	                                //   exclusion in the 'queue'.
	struct gendisk *gd;             // The generic disk.
	//user added
	int number_read_lock;
	int number_write_lock;
	pid_t write_lock_holder;
	//pid_list_t read_pid;
	pid_list_t pid_list_head;
	pid_list_t pid_list_tail;
	//deadlock 
	pid_list_t check_deadlock_list_head;
	pid_list_t check_deadlock_list_tail;
} osprd_info_t;

#define NOSPRD 4
static osprd_info_t osprds[NOSPRD];
pid_list_t 	list_remove_element (pid_list_t head, pid_t p);


// Declare useful helper functions

/*
 * file2osprd(filp)
 *   Given an open file, check whether that file corresponds to an OSP ramdisk.
 *   If so, return a pointer to the ramdisk's osprd_info_t.
 *   If not, return NULL.
 */
static osprd_info_t *file2osprd(struct file *filp);

/*
 * for_each_open_file(task, callback, user_data)
 *   Given a task, call the function 'callback' once for each of 'task's open
 *   files.  'callback' is called as 'callback(filp, user_data)'; 'filp' is
 *   the open file, and 'user_data' is copied from for_each_open_file's third
 *   argument.
 */
static void for_each_open_file(struct task_struct *task,
			       void (*callback)(struct file *filp,
						osprd_info_t *user_data),
			       osprd_info_t *user_data);


/*
 * osprd_process_request(d, req)
 *   Called when the user reads or writes a sector.
 *   Should perform the read or write, as appropriate.
 */
static void osprd_process_request(osprd_info_t *d, struct request *req)
{
	if (!blk_fs_request(req)) {
		end_request(req, 0);
		return;
	}

	// EXERCISE: Perform the read or write request by copying data between
	// our data array and the request's buffer.
	// Hint: The 'struct request' argument tells you what kind of request
	// this is, and which sectors are being read or written.
	// Read about 'struct request' in <linux/blkdev.h>.
	// Consider the 'req->sector', 'req->current_nr_sectors', and
	// 'req->buffer' members, and the rq_data_dir() function.
	//  sector_t sector;                /* next sector to submit */
	//unsigned int current_nr_sectors;  /* no. of sectors left to submit in the current segment */         
	//__REQ_RW,               /* not set, read. set, write */
	// #define rq_data_dir(rq)         ((rq)->flags & 1)
	// Your code here.
	
	uint8_t * r_src_addr = d->data + req->sector * SECTOR_SIZE;
	uint8_t * r_dest_addr = req->buffer;
	size_t r_data_len = req->current_nr_sectors * SECTOR_SIZE;
	uint8_t * w_src_addr = req->buffer;
	uint8_t * w_dest_addr = d->data + req->sector * SECTOR_SIZE;
	size_t w_data_len = req->current_nr_sectors * SECTOR_SIZE;
	if (rq_data_dir (req) == READ)		//not set, read
	{
		memcpy ((void*)r_src_addr, (void*)r_dest_addr, r_data_len);		//char* buffer, uint8_t * data
	}
	else if (rq_data_dir(req) == WRITE)
	{
		memcpy ((void*)w_src_addr, (void*)w_dest_addr, w_data_len );
	}
	else
	{
	eprintk("Error read/wirte.\n");
	end_request(req, 0);	
	}
	end_request(req, 1);
	
	
}


// This function is called when a /dev/osprdX file is opened.
// You aren't likely to need to change this.
static int osprd_open(struct inode *inode, struct file *filp)
{
	// Always set the O_SYNC flag. That way, we will get writes immediately
	// instead of waiting for them to get through write-back caches.
	filp->f_flags |= O_SYNC;
	return 0;
}


// This function is called when a /dev/osprdX file is finally closed.
// (If the file descriptor was dup2ed, this function is called only when the
// last copy is closed.)
static int osprd_close_last(struct inode *inode, struct file *filp)
{
	if (filp) {
		osprd_info_t *d = file2osprd(filp);
		int filp_writable = filp->f_mode & FMODE_WRITE;

		// EXERCISE: If the user closes a ramdisk file that holds
		// a lock, release the lock.  Also wake up blocked processes
		// as appropriate.
		osp_spin_lock (&d->mutex);
		if (filp->f_flags & F_OSPRD_LOCKED)
		{	
			if (filp_writable)
			{
				d->number_write_lock --;
				d->write_lock_holder = -1;
			}

			else
			{
				d->number_read_lock--;
				d->pid_list_head = list_remove_element(d->pid_list_head,current->pid);
				//if (d->pid_list_head == NULL)
				//	return -ENOTTY;
			}
		}
		filp->f_flags = filp->f_flags & ~F_OSPRD_LOCKED;
		osp_spin_unlock (&d->mutex);
		wake_up_all (&(d->blockq));

			}

	return 0;
}


/*
 * osprd_lock
 */

/*
 * osprd_ioctl(inode, filp, cmd, arg)
 *   Called to perform an ioctl on the named file.
 */
pid_list_t
initPidlist(pid_t pid)
{
  pid_list_t new_pid =(pid_list_t)kmalloc(sizeof(struct pid_list),GFP_ATOMIC);
  //void *kmalloc(size_t size, gfp_t flags)	
  //Allocate size bytes of memory. Returns a pointer to the allocated memory. 
  //flags specifies details about the allocation; for the purposes of this lab, you should generally give GFP_ATOMIC.
  new_pid->pid = pid;
  new_pid->next = NULL;
  new_pid->prev = NULL;
  return new_pid;
}
void
add_read_pid(pid_t pid, osprd_info_t *d)
{
  
  if(d->pid_list_tail== NULL)
  {
    d->pid_list_tail = initPidlist(pid);
    d->pid_list_head= d->pid_list_tail;
  }
  else
  {
    pid_list_t pid_new = initPidlist(pid);
    //command_stream_head->next = command_stream_new;
    d->pid_list_tail->next = pid_new;
    pid_new->prev = d->pid_list_tail;
    d->pid_list_tail = d->pid_list_tail->next;
    d->pid_list_tail->next = NULL;
  }
}
void
add_check_deadlock_list(pid_t pid, osprd_info_t *d)
{
  
  if(d->check_deadlock_list_tail== NULL)
  {
    d->check_deadlock_list_tail = initPidlist(pid);
    d->check_deadlock_list_head= d->check_deadlock_list_tail;
  }
  else
  {
    pid_list_t pid_new = initPidlist(pid);
    //command_stream_head->next = command_stream_new;
    d->check_deadlock_list_tail->next = pid_new;
    pid_new->prev = d->check_deadlock_list_tail;
    d->check_deadlock_list_tail = d->check_deadlock_list_tail->next;
    d->check_deadlock_list_tail->next = NULL;
  }
}
void 
delete_check_deadlock_list (pid_t current_pid)
{
	if (check_deadlock_list_head ->pid != current_pid)
	{
		eprintk ("the head pid is not the current pid");
		return;
	}
	else
	{
		if (check_deadlock_list_head == check_deadlock_list_tail)
			kfree(check_deadlock_list_head);
		else 
		{
			pid_list_t tmp = check_deadlock_list_head;
			check_deadlock_list_head = check_deadlock_list_head ->next;
			check_deadlock_list_head->prev = NULL;
			kfree(tmp);
		}
	}
}
int
find_until_count (pid_list_t od_head, pid_list_t od_tail, pid_t current_pid, int count)
{
	if (od_head == NULL)
		return 0;	//没有找到
	int i;
	i=0;
	pid_list_t tmp;
	tmp = od_head;
	while (tmp != od_tail)
	{
		if (i == count)
			return 0;
		else if (tmp->pid == current_pid)	//i <count
		{
				return 1;
		}
		tmp = tmp->next;
		i++;
	}
	if (i != count)
		eprintk ("error: count.");
}
/*int
clear_current_reader (pid_list_t pid_list_head, pid_list_t pid_list_tail)
{
	pid_list_t tmp = pid_list_head;
	pid_list_t prev = NULL;
	while (tmp ->pid != current->pid && tmp != pid_list_tail)
	{
		prev = tmp;
		tmp = tmp->next;
	}
	if (tmp ->pid == current->pid)
	{
		tmp -> pid = -1;
		if (tmp==pid_list_tail)
		{
			prev->next = NULL;
			pid_list_tail == prev;
		}
		else if (tmp == pid_list_head)
		{
			pid_list_head = tmp->next;
		}
		else
			prev->next = tmp->next;
		kfree(tmp);
	}
	else 
		return -ENOTTY;

}
*/
pid_list_t 
list_remove_element (pid_list_t head, pid_t p)
{
	pid_list_t current_node, last_node;

	if(head == NULL)
		return NULL;

	if(head->pid == p)
	{
		current_node = head;
		current_node->pid = -1;
		head = head->next;
		head->prev = NULL;
		kfree(current_node);
		return head;
	}

	current_node = last_node = head;
	while((current_node = current_node->next) != NULL)
	{
		if(current_node->pid == p)
		{ 
			current_node->pid = -1;
			last_node->next = current_node->next;
			current_node->next->prev = current_node->prev;
			kfree(current_node);
			return head;
		}

		last_node = current_node;
	}

	// Element not found
	return head;
}
void list_free_all (pid_list_t head)
{
	while (head != NULL)
		head = list_remove_element(head, head->pid);
}
int osprd_ioctl(struct inode *inode, struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	osprd_info_t *d = file2osprd(filp);	// device info
	int r = 0;			// return value: initially 0

	// is file open for writing?
	int filp_writable = (filp->f_mode & FMODE_WRITE) != 0;

	// This line avoids compiler warnings; you may remove it.
	(void) filp_writable, (void) d;

	// Set 'r' to the ioctl's return value: 0 on success, negative on error

	
	if (cmd == OSPRDIOCACQUIRE) {

		// EXERCISE: Lock the ramdisk.
		osp_spin_lock(&d->mutex); 
		unsigned cur_ticket;
		cur_ticket = d->ticket_head;
		d->ticket_head ++ ;
		osp_spin_unlock(&d->mutex);
		if(filp_writable)
		{
			//osp_spin_lock(&d->mutex);	
			int w = wait_event_interruptible(d->blockq, (d->number_write_lock==0&&d->number_read_lock==0&&cur_ticket==d->ticket_tail));
			//Blocks the current task on a wait queue until a CONDITION becomes true. 
			//A request for a write lock on a ramdisk file will block until no other files on that 
			//ramdisk have a read or writeloc
			if(w == -ERESTARTSYS)
			{
				osp_spin_lock(&d->mutex);	
				//if the process is interrupted by signal
				if (cur_ticket == d->ticket_tail)
					d->ticket_tail++;
				//already in the next avalable ticket
				else
					d->ticket_head--;
				//destory this ticket
				osp_spin_unlock(&d->mutex);	
				return w;
			}
			
			osp_spin_lock(&d->mutex);	
			//Acquire a mutex (lock the mutex)
			d->ticket_tail++;
			d->write_lock_holder = current->pid;
			d->number_write_lock = 1;
			filp->f_flags |= F_OSPRD_LOCKED;
			osp_spin_unlock(&d->mutex);	
			//Release (unlock) the mutex
		}
		else
		{

			int w = wait_event_interruptible(d->blockq, (d->number_write_lock==0&&cur_ticket==d->ticket_tail));
			//Blocks the current task on a wait queue until a CONDITION becomes true. 
			//A request for a write lock on a ramdisk file will block until no other files on that 
			//ramdisk have a read or writeloc
			if(w == -ERESTARTSYS)
			{
				osp_spin_lock(&d->mutex);
				//if the process is interrupted by signal
				if (cur_ticket == d->ticket_tail)
					d->ticket_tail++;
				//already in the next avalable ticket
				else
					d->ticket_head--;
				osp_spin_unlock(&d->mutex);	
				//destory this ticket
				return w;
			}
			
			osp_spin_lock(&d->mutex);	
			//Acquire a mutex (lock the mutex)
			d->ticket_tail++;
			add_read_pid(current->pid,d);
			d->number_read_lock++;
			filp->f_flags |= F_OSPRD_LOCKED;
			osp_spin_unlock(&d->mutex);	
			//Release (unlock) the mutex
		}

	} else if (cmd == OSPRDIOCTRYACQUIRE) {

		// EXERCISE: ATTEMPT to lock the ramdisk.
		//
		// This is just like OSPRDIOCACQUIRE, except it should never
		// block.  If OSPRDIOCACQUIRE would block or return deadlock,
		// OSPRDIOCTRYACQUIRE should return -EBUSY.
		// Otherwise, if we can grant the lock request, return 0.

		// Your code here (instead of the next two lines).
		if (filp_writable)
		{
			//atomically acquire write lock
			osp_spin_lock (&d->mutex);	//atomicity
			if ((d->number_read_lock >0) || (d->number_write_lock>0))
			{
				osp_spin_unlock (&d->mutex);
				return -EBUSY;
			}
			else 	//d->number_read_lock ==0) && (d->number_write_lock==0)
			{
				d->write_lock_holder = current->pid;
				d->number_write_lock ++;
				d->ticket_tail++;
				d->ticket_head++;
				filp -> f_flags |= F_OSPRD_LOCKED;
				osp_spin_unlock (&d->mutex);
			}
		}
		else 	//opened for read 
		{
			//atomically acquire read lock
			osp_spin_lock (&d->mutex);
			{
				if (d->number_write_lock>0)		//can't get read lock
				{
					osp_spin_unlock(&d->mutex);
					return -EBUSY;
				}
				else
				{
					add_read_pid (current->pid,d);
					d->number_read_lock++;
					d->ticket_tail++;
					d->ticket_head++;
					filp -> f_flags |= F_OSPRD_LOCKED;
					osp_spin_unlock (&d->mutex);
				}
			}
		}

		
		//eprintk("Attempting to try acquire\n");
		//r = -ENOTTY;

	} else if (cmd == OSPRDIOCRELEASE) {

		// EXERCISE: Unlock the ramdisk.
		//
		// If the file hasn't locked the ramdisk, return -EINVAL.
		// Otherwise, clear the lock from filp->f_flags, wake up
		// the wait queue, perform any additional accounting steps
		// you need, and return 0.
		//osp_spin_lock (&d->mutex);
		if ((filp->f_flags & F_OSPRD_LOCKED)==0)
		{
			//osp_spin_unlock (&d->mutex);
			return -EINVAL;
		}
		else
		{
			osp_spin_lock (&d->mutex);	
			if (filp_writable)		//release the write locker
			{
				d->write_lock_holder = -1;
				d->number_write_lock --;
			}
			else 	//release the read locker
			{
				d->number_read_lock --;
				d->pid_list_head = list_remove_element(d->pid_list_head,current->pid);
				/*if (list_free_all (pid_list_head) == -ENOTTY)
					return -ENOTTY;*/
				if (d->pid_list_head == NULL)
					return -ENOTTY;
			}
			filp->f_flags &= ~F_OSPRD_LOCKED; 
			osp_spin_unlock (&d->mutex);
			wake_up_all (&d->blockq);
			delete_check_deadlock_list (current->pid);
		}
		// Your code here (instead of the next line).
		//r = -ENOTTY;

	}
	else
		r = -ENOTTY; /* unknown command */
	return r;

}


// Initialize internal fields for an osprd_info_t.

static void osprd_setup(osprd_info_t *d)
{
	/* Initialize the wait queue. */
	init_waitqueue_head(&d->blockq);
	osp_spin_lock_init(&d->mutex);
	d->ticket_head = d->ticket_tail = 0;
	/* Add code here if you add fields to osprd_info_t. */
	d->number_write_lock = 0;
	d->number_read_lock = 0;
	
}


/*****************************************************************************/
/*         THERE IS NO NEED TO UNDERSTAND ANY CODE BELOW THIS LINE!          */
/*                                                                           */
/*****************************************************************************/

// Process a list of requests for a osprd_info_t.
// Calls osprd_process_request for each element of the queue.

static void osprd_process_request_queue(request_queue_t *q)
{
	osprd_info_t *d = (osprd_info_t *) q->queuedata;
	struct request *req;

	while ((req = elv_next_request(q)) != NULL)
		osprd_process_request(d, req);
}


// Some particularly horrible stuff to get around some Linux issues:
// the Linux block device interface doesn't let a block device find out
// which file has been closed.  We need this information.

static struct file_operations osprd_blk_fops;
static int (*blkdev_release)(struct inode *, struct file *);

static int _osprd_release(struct inode *inode, struct file *filp)
{
	if (file2osprd(filp))
		osprd_close_last(inode, filp);
	return (*blkdev_release)(inode, filp);
}

static int _osprd_open(struct inode *inode, struct file *filp)
{
	if (!osprd_blk_fops.open) {
		memcpy(&osprd_blk_fops, filp->f_op, sizeof(osprd_blk_fops));
		blkdev_release = osprd_blk_fops.release;
		osprd_blk_fops.release = _osprd_release;
	}
	filp->f_op = &osprd_blk_fops;
	return osprd_open(inode, filp);
}


// The device operations structure.

static struct block_device_operations osprd_ops = {
	.owner = THIS_MODULE,
	.open = _osprd_open,
	// .release = osprd_release, // we must call our own release
	.ioctl = osprd_ioctl
};


// Given an open file, check whether that file corresponds to an OSP ramdisk.
// If so, return a pointer to the ramdisk's osprd_info_t.
// If not, return NULL.

static osprd_info_t *file2osprd(struct file *filp)
{
	if (filp) {
		struct inode *ino = filp->f_dentry->d_inode;
		if (ino->i_bdev
		    && ino->i_bdev->bd_disk
		    && ino->i_bdev->bd_disk->major == OSPRD_MAJOR
		    && ino->i_bdev->bd_disk->fops == &osprd_ops)
			return (osprd_info_t *) ino->i_bdev->bd_disk->private_data;
	}
	return NULL;
}


// Call the function 'callback' with data 'user_data' for each of 'task's
// open files.

static void for_each_open_file(struct task_struct *task,
		  void (*callback)(struct file *filp, osprd_info_t *user_data),
		  osprd_info_t *user_data)
{
	int fd;
	task_lock(task);
	spin_lock(&task->files->file_lock);
	{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 13)
		struct files_struct *f = task->files;
#else
		struct fdtable *f = task->files->fdt;
#endif
		for (fd = 0; fd < f->max_fds; fd++)
			if (f->fd[fd])
				(*callback)(f->fd[fd], user_data);
	}
	spin_unlock(&task->files->file_lock);
	task_unlock(task);
}


// Destroy a osprd_info_t.

static void cleanup_device(osprd_info_t *d)
{
	wake_up_all(&d->blockq);
	if (d->gd) {
		del_gendisk(d->gd);
		put_disk(d->gd);
	}
	if (d->queue)
		blk_cleanup_queue(d->queue);
	if (d->data)
		vfree(d->data);
}


// Initialize a osprd_info_t.

static int setup_device(osprd_info_t *d, int which)
{
	memset(d, 0, sizeof(osprd_info_t));

	/* Get memory to store the actual block data. */
	if (!(d->data = vmalloc(nsectors * SECTOR_SIZE)))
		return -1;
	memset(d->data, 0, nsectors * SECTOR_SIZE);

	/* Set up the I/O queue. */
	spin_lock_init(&d->qlock);
	if (!(d->queue = blk_init_queue(osprd_process_request_queue, &d->qlock)))
		return -1;
	blk_queue_hardsect_size(d->queue, SECTOR_SIZE);
	d->queue->queuedata = d;

	/* The gendisk structure. */
	if (!(d->gd = alloc_disk(1)))
		return -1;
	d->gd->major = OSPRD_MAJOR;
	d->gd->first_minor = which;
	d->gd->fops = &osprd_ops;
	d->gd->queue = d->queue;
	d->gd->private_data = d;
	snprintf(d->gd->disk_name, 32, "osprd%c", which + 'a');
	set_capacity(d->gd, nsectors);
	add_disk(d->gd);

	/* Call the setup function. */
	osprd_setup(d);

	return 0;
}

static void osprd_exit(void);


// The kernel calls this function when the module is loaded.
// It initializes the 4 osprd block devices.

static int __init osprd_init(void)
{
	int i, r;

	// shut up the compiler
	(void) for_each_open_file;
#ifndef osp_spin_lock
	(void) osp_spin_lock;
	(void) osp_spin_unlock;
#endif

	/* Register the block device name. */
	if (register_blkdev(OSPRD_MAJOR, "osprd") < 0) {
		printk(KERN_WARNING "osprd: unable to get major number\n");
		return -EBUSY;
	}

	/* Initialize the device structures. */
	for (i = r = 0; i < NOSPRD; i++)
		if (setup_device(&osprds[i], i) < 0)
			r = -EINVAL;

	if (r < 0) {
		printk(KERN_EMERG "osprd: can't set up device structures\n");
		osprd_exit();
		return -EBUSY;
	} else
		return 0;
}


// The kernel calls this function to unload the osprd module.
// It destroys the osprd devices.

static void osprd_exit(void)
{
	int i;
	for (i = 0; i < NOSPRD; i++)
		cleanup_device(&osprds[i]);
	unregister_blkdev(OSPRD_MAJOR, "osprd");
}


// Tell Linux to call those functions at init and exit time.
module_init(osprd_init);
module_exit(osprd_exit);


//functions for deadlock

