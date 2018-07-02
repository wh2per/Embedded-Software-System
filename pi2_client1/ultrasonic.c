//ultrasonic.c
#include <linux/init.h>			 //init, exit 등
#include <linux/module.h>		 //꼭 넣어야하는
#include <linux/kernel.h>		 //필수 헤더파일
#include <linux/gpio.h>		         //GPIO (General-Purpose Input Output) 관련
#include <linux/delay.h>
#include <linux/fs.h>                
#include <linux/interrupt.h>
#include <linux/cdev.h>                  //cdev 관련. cdev_alloc 등
#include <linux/time.h>

#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/uaccess.h>        //copy_to_user, copy_from_user

MODULE_LICENSE("GPL");			 //모듈 라이센스

#define DEV_NAME "simple_ultrasonic_dev" // ultrasonic 커널모듈 이름

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1

#define SIMPLE_ULTRASONIC_IOCTL_NUM 'z'
#define GET_ULTRASONIC_DISTANCE _IOWR(SIMPLE_ULTRASONIC_IOCTL_NUM, IOCTL_NUM1, unsigned long *)

#define TRIG_GPIO_PIN 23
#define ECHO_GPIO_PIN 24

static int irq_num;
struct timeval after, before; // 시간을 계산하기 위한 변수
long int ultrasonic_distance;

static int waiting=1;
wait_queue_head_t my_wq;


static void ultrasonic_read(void)
{
    gpio_set_value (TRIG_GPIO_PIN, 1);
    udelay(10);
    gpio_set_value (TRIG_GPIO_PIN, 0);

    gpio_direction_input(ECHO_GPIO_PIN);
    
    do_gettimeofday(&before);
    printk("before : %ld, waiting = %d\n", before.tv_usec, waiting);
}

static long int get_ultrasonic_distance(unsigned long arg)
{
    // trig 신호 전송
    ultrasonic_read(); 

    // 락 - echo가 올 때까지 대기
    waiting--;
    wait_event(my_wq, waiting > 0); //Non-exclusive 

    // ultrasonic_distance값 구하기
    ultrasonic_distance = (after.tv_usec - before.tv_usec)/80;

    printk("%ldcm\n", ultrasonic_distance);
    
    // arg를 통해 ultrasonic_distance값을 반환한다.   
    copy_to_user((long int*)arg, &ultrasonic_distance, sizeof(long int));//ultrasonic_distance값을 유저영역으로 보낸다.

    return ultrasonic_distance; //디버깅용으로 그냥 ultrasonic_distance값도 반환함.
}

static long ku_pir_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    long int ret;

    switch(cmd){
    case GET_ULTRASONIC_DISTANCE:
        ret = get_ultrasonic_distance(arg);
        printk("get_ultrasonic_distance. ret = %ld\n", ret);
        break;
    default:
        return -1;
    }
    return 0;
}


static int simple_ultrasonic_open(struct inode *inode, struct file *file){
    printk("simple_ultrasonic open\n");
    return 0;
}

static int simple_ultrasonic_release(struct inode *inode, struct file *file){
    printk("simple_ultrasonic release\n");
    return 0;
}

struct file_operations simple_ultrasonic_fops = {
    .unlocked_ioctl = ku_pir_ioctl,
    .open = simple_ultrasonic_open,
    .release = simple_ultrasonic_release
};

static irqreturn_t simple_sensor_isr(int irq, void* dev_id){
   
    do_gettimeofday(&after);
    printk("after : %ld, waiting = %d\n", before.tv_usec, waiting);
    
    gpio_direction_output(ECHO_GPIO_PIN, 0);

    //언락
    waiting++;
    wake_up(&my_wq);

    return IRQ_HANDLED;
}

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init simple_ultrasonic_init(void){
    int ret;
    printk("Init simple_ultrasonic Module\n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &simple_ultrasonic_fops);
    ret = cdev_add(cd_cdev, dev_num, 1);
    if (ret < 0) {
        printk("fail to add character device");
        return -1;
    }

    //~~ 추가 내용
    gpio_request_one(TRIG_GPIO_PIN, GPIOF_OUT_INIT_LOW, "TRIG_GPIO_PIN");
    gpio_request_one(ECHO_GPIO_PIN, GPIOF_OUT_INIT_LOW, "ECHO_GPIO_PIN");

    irq_num = gpio_to_irq(ECHO_GPIO_PIN);
    ret = request_irq(irq_num, simple_sensor_isr, IRQF_TRIGGER_FALLING, "echo_irq", NULL);

    init_waitqueue_head(&my_wq);

    return 0;
}

static void __exit simple_ultrasonic_exit(void){
    printk("Exit simple_ultrasonic Module\n");
    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
    gpio_free(TRIG_GPIO_PIN);
    gpio_free(ECHO_GPIO_PIN);
    free_irq(irq_num, NULL);
}

module_init(simple_ultrasonic_init);
module_exit(simple_ultrasonic_exit);
