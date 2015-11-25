/*
 * wk_lock.c
 *
 * Simple kernel module that locks a spin_lock on open and unlocks
 * it on close.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>

static struct proc_dir_entry *wk_file_dentry;
static spinlock_t wk_spin_lock, priv_spin_lock;
static int locked;

static
int wk_release(struct inode *inode, struct file *filp)
{
	spin_lock(&priv_spin_lock);
	locked = 0;
	spin_unlock(&wk_spin_lock);
	spin_unlock(&priv_spin_lock);
	printk("wk_lock: unlocked\n");

	return 0;
}

static
int wk_open(struct inode *inode, struct file *filp)
{
	spin_lock(&wk_spin_lock);
	spin_lock(&priv_spin_lock);
	locked = 1;
	spin_unlock(&priv_spin_lock);
	printk("wk_lock: locked\n");

	return 0;
}

static const
struct file_operations wk_file_operations = {
	.owner = THIS_MODULE,
	.open = wk_open,
	.release = wk_release,
};

static
int wk_init(void)
{
	int ret;

	spin_lock_init(&wk_spin_lock);
	spin_lock_init(&priv_spin_lock);
	wk_file_dentry = proc_create_data("wk_lock",
			S_IRUGO | S_IWUGO, NULL,
			&wk_file_operations, NULL);
	if (!wk_file_dentry) {
		printk(KERN_ERR "Error creating wk_file\n");
		ret = -ENOMEM;
		goto error;
	}
	ret = 0;

error:
	return ret;
}

static
void wk_exit(void)
{
	spin_lock(&priv_spin_lock);
	if (locked)
		spin_unlock(&wk_spin_lock);

	if (wk_file_dentry)
		remove_proc_entry("wk_lock", NULL);
	spin_unlock(&priv_spin_lock);
}

module_init(wk_init);
module_exit(wk_exit);
MODULE_AUTHOR("Julien Desfossez <jdesfossez@efficios.com>");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
