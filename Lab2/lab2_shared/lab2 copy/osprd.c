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

/* This flag is added to an OSPRD file's f_flags to indicate that the file
 * is locked. */
#define F_OSPRD_LOCKED	0x80000

/* eprintk() prints messages to the console.
 * (If working on a real Linux machine, change KERN_NOTICE to KERN_ALERT or
 * KERN_EMERG so that you are sure to see the messages.  By default, the
 * kernel does not print all messages to the console.  Levels like KERN_ALERT
 * and KERN_EMERG will make sure that you will see messages.) */
#define eprintk(format, ...) printk(KERN_ALERT format, ## __VA_ARGS__) //changed to kern_alert!

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("CS 111 RAM Disk");
// EXERCISE: Pass your names into the kernel as the module's authors.
MODULE_AUTHOR("Markus Notti and Kyle Baker");

#define OSPRD_MAJOR	222

/* This module parameter controls how big the disk will be.
 * You can specify module parameters when you load the module,
 * as an argument to insmod: "insmod osprd.ko nsectors=4096" */
static int nsectors = 32;
module_param(nsectors, int, 0);

int osprd_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);


typedef struct exited_node{
	struct exited_node* next;
	unsigned ticket_no;
}exited_node;

typedef struct pid_tracker_node{
	struct pid_tracker_node* next;
	int pid;
}pid_tracker_node;

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

	unsigned n_read_locks;				//ADDED
	unsigned n_write_locks;				//ADDED
	exited_node* exited_head;			//ADDED
	pid_tracker_node* pid_tracker_head;	//ADDED


} osprd_info_t;

#define NOSPRD 4
static osprd_info_t osprds[NOSPRD];


// Declare useful helper functions



void find_next_ticket(osprd_info_t* d);
void remove_pid(osprd_info_t* d);
void add_pid_to_tracker(osprd_info_t* d);
int did_find_pid(osprd_info_t* d);

int did_find_pid(osprd_info_t* d)
{
	pid_tracker_node* it_temp;
	while(it_temp != NULL)
		{
			if (it_temp->pid == current->pid)	//current ticket holder is on the exit list
			{
				return 1;
			}
			it_temp = it_temp->next;
		}
	return 0;
}

void add_pid_to_tracker(osprd_info_t* d)
{
	pid_tracker_node* temp_node = kmalloc(sizeof(pid_tracker_node), GFP_ATOMIC);
	temp_node->pid = current->pid;

	temp_node->next = d->pid_tracker_head;
	d->pid_tracker_head = temp_node;
}

void find_next_ticket(osprd_info_t* d)
{
	///////////////////////
	//Finding Next ticket//
	///////////////////////

	//d->ticket_head++;
						
	//check if next ticket has been killed
	exited_node* it_temp = NULL;
	exited_node* it_temp2 = NULL;
	for(;;)	//loop until you reach the end of exited node list
	{
		it_temp = d->exited_head;

		while(it_temp != NULL)
		{
			if (it_temp->ticket_no == d->ticket_head)	//current ticket holder is on the exit list
			{
				d->ticket_head++;
				if (it_temp2)
					it_temp2->next = it_temp->next;
				else
					d->exited_head = it_temp->next;
				kfree(it_temp);

				break;
			}
			it_temp2 = it_temp;
			it_temp	= it_temp->next;
		}
		if(it_temp == NULL)		//has reached the end of the list
		break;
	}
	///////////////////////////////
	//Next ticket should be found//
	///////////////////////////////
}

void remove_pid(osprd_info_t* d)
{
    pid_tracker_node* curr = d->pid_tracker_head;
    pid_tracker_node* prev = NULL;
    
    while (curr != NULL)
    {
        if (curr->pid == current->pid)
        {
            //If its at the front of the list
            if(prev == NULL)
            {
                d->pid_tracker_head = d->pid_tracker_head->next;
                kfree(curr);
            }
            // It's at the end/middle
            else
            {
                prev->next = curr->next;
                kfree(curr);
            }
            break;
        }
        prev = curr;
        curr = curr->next;
            
    }
}

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


	// Your code here.
    
    if (req->sector > nsectors || req->sector < 0)
    {
        eprintk("Invalid sector number\n");
        end_request(0, req);
    }

    int data_size = req->current_nr_sectors * SECTOR_SIZE;
    int offset = req->sector * SECTOR_SIZE;
    
    if (rq_data_dir(req) == WRITE)
        memcpy(d->data + offset, req->buffer, data_size);
    else if (rq_data_dir(req) == READ)
        memcpy(req->buffer, d->data + offset, data_size);
    else
    {
        eprintk("Invalid read/write type\n");
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

		// Your code here.
        
        if(filp->f_flags & F_OSPRD_LOCKED)
        {
            osprd_ioctl(inode, filp, OSPRDIOCRELEASE, 0);
        }

		// This line avoids compiler warnings; you may remove it.
		(void) filp_writable, (void) d;

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
		//
		// If *filp is open for writing (filp_writable), then attempt
		// to write-lock the ramdisk; otherwise attempt to read-lock
		// the ramdisk.
		//
                // This lock request must block using 'd->blockq' until:
		// 1) no other process holds a write lock;
		// 2) either the request is for a read lock, or no other process
		//    holds a read lock; and
		// 3) lock requests should be serviced in order, so no process
		//    that blocked earlier is still blocked waiting for the
		//    lock.
		//
		// If a process acquires a lock, mark this fact by setting
		// 'filp->f_flags |= F_OSPRD_LOCKED'.  You also need to
		// keep track of how many read and write locks are held:
		// change the 'osprd_info_t' structure to do this.
		//
		// Also wake up processes waiting on 'd->blockq' as needed.
		//
		// If the lock request would cause a deadlock, return -EDEADLK.
		// If the lock request blocks and is awoken by a signal, then
		// return -ERESTARTSYS.
		// Otherwise, if we can grant the lock request, return 0.

		// 'd->ticket_head' and 'd->ticket_tail' should help you
		// service lock requests in order.  These implement a ticket
		// order: 'ticket_tail' is the next ticket, and 'ticket_head'
		// is the ticket currently being served.  You should set a local
		// variable to 'd->ticket_head' and increment 'd->ticket_head'.
		// Then, block at least until 'd->ticket_tail == local_ticket'.
		// (Some of these operations are in a critical section and must
		// be protected by a spinlock; which ones?)

		// Your code here (instead of the next two lines).

		//lock 
		osp_spin_lock(&d->mutex);

		if (did_find_pid(d))
		{
			osp_spin_unlock(&d->mutex);
			return -EDEADLK;
		}

		//assign a ticket to current process and increment ticket tracker
		unsigned tickety_current = d->ticket_tail;
		d->ticket_tail++;

		add_pid_to_tracker(d);

		osp_spin_unlock(&d->mutex);

		int waity_return_val;

		if(filp_writable)
		{
			waity_return_val = wait_event_interruptible(d->blockq, tickety_current == d->ticket_head && d->n_read_locks == 0 && d->n_write_locks == 0);
		}
		else
		{
			waity_return_val = wait_event_interruptible(d->blockq, tickety_current == d->ticket_head && d->n_write_locks== 0);
		}
		if(waity_return_val == -ERESTARTSYS)
		{
			//TODO: add the current ticket to the exited ticket list
					//return -ERESTARTSYS
			osp_spin_lock(&d->mutex);

			exited_node* temp_node = kmalloc(sizeof(exited_node), GFP_ATOMIC);
			temp_node->ticket_no = tickety_current;

			temp_node->next = d->exited_head;
			d->exited_head = temp_node;

			find_next_ticket(d);
            remove_pid(d);
			osp_spin_unlock(&d->mutex);

			wake_up_all(&d->blockq);
			return -ERESTARTSYS;

		}

		//handle sys signals...kill tickets if signals TODO
		

		osp_spin_lock(&d->mutex);

        filp->f_flags |= F_OSPRD_LOCKED;

		if (filp_writable)
		{
			d->n_write_locks++;
		}
		else
		{
			d->n_read_locks++;
		}

		d->ticket_head++;
		find_next_ticket(d);

		osp_spin_unlock(&d->mutex);

		wake_up_all(&d->blockq);

		return 0;





	} else if (cmd == OSPRDIOCTRYACQUIRE) {

		// EXERCISE: ATTEMPT to lock the ramdisk.
		//
		// This is just like OSPRDIOCACQUIRE, except it should never
		// block.  If OSPRDIOCACQUIRE would block or return deadlock,
		// OSPRDIOCTRYACQUIRE should return -EBUSY.
		// Otherwise, if we can grant the lock request, return 0.

		// Your code here (instead of the next two lines).
        
		osp_spin_lock(&d->mutex);

		unsigned tickety_current = d->ticket_tail;

		d->ticket_tail++;

		osp_spin_unlock(&d->mutex);

		if(filp_writable)
		{
			if(tickety_current == d->ticket_head && d->n_read_locks == 0 && d->n_write_locks == 0)
			{
				osp_spin_lock(&d->mutex);

				filp->f_flags |= F_OSPRD_LOCKED;
				d->n_write_locks++;

				d->ticket_head++;
				find_next_ticket(d);

				osp_spin_unlock(&d->mutex);

				wake_up_all(&d->blockq);
				return 0;
			}
			else
			{
				osp_spin_lock(&d->mutex);

				d->ticket_head++;
				find_next_ticket(d);
				osp_spin_unlock(&d->mutex);

				wake_up_all(&d->blockq);
				return -EBUSY;
			}
		}
		else
		{
			if(tickety_current == d->ticket_head && d->n_write_locks == 0)
			{

				osp_spin_lock(&d->mutex);

				filp->f_flags |= F_OSPRD_LOCKED;
				d->n_read_locks++;

				d->ticket_head++;
				find_next_ticket(d);

				osp_spin_unlock(&d->mutex);
				
				wake_up_all(&d->blockq);
				return 0;
			}
			else
			{
				osp_spin_lock(&d->mutex);

				d->ticket_head++;
				find_next_ticket(d);

				wake_up_all(&d->blockq);
			
				osp_spin_unlock(&d->mutex);

				return -EBUSY;
			}
        }

	} else if (cmd == OSPRDIOCRELEASE) {

		// EXERCISE: Unlock the ramdisk.
		//
		// If the file hasn't locked the ramdisk, return -EINVAL.
		// Otherwise, clear the lock from filp->f_flags, wake up
		// the wait queue, perform any additional accounting steps
		// you need, and return 0.

		// Your code here (instead of the next line).
        if (filp->f_flags & F_OSPRD_LOCKED)
        {
            osp_spin_lock(&d->mutex);
            
            if (filp_writable)
                d->n_write_locks--;
            else
                d->n_read_locks--;
            
            filp->f_flags ^= F_OSPRD_LOCKED;
            remove_pid(d);
            osp_spin_unlock(&d->mutex);
            wake_up_all(&d->blockq);
            
            r = 0;
        }
        else
            r = -EINVAL;

	} else
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
	d->n_read_locks = 0;
	d->n_write_locks = 0;
	d->exited_head = NULL;
	d->pid_tracker_head = NULL;
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
