#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;
CvCapture* capture;
sem_t bin_sem1;
sem_t bin_sem2;
sem_t bin_sem3;
sem_t bin_sem4;
sem_t bin_sem5;
sem_t bin_sem6;
sem_t bin_sem7;
#define HRES 640
#define VRES 480
#define BILLION 1E9;
time_t timestamp[2000];
time_t diff[2001];
signed int jitter[2000];
static double timestamp1(void)
{
 int i,ret = 0;
 struct timespec curr_time;
 double time;
 int8_t arr1[10],arr2[10];
 ret = clock_gettime(CLOCK_REALTIME, &curr_time);
 {
 if(ret != 0)
 printf("clock_gettime failed!");
 }

 time = ((curr_time.tv_sec) + (curr_time.tv_nsec)) / BILLION;
 return time;
}
void *timer_thread(void *arg)
{
struct timespec curr_time;
time_t cu_timer;
int count = 1;
while(1)
{
sem_wait(&bin_sem5);
clock_gettime(CLOCK_REALTIME, &curr_time);
time_t aq_timer = curr_time.tv_sec;
while(1)
{
clock_gettime(CLOCK_REALTIME, &curr_time);
cu_timer = curr_time.tv_sec;
if(cu_timer - aq_timer == 1)
{
sem_post(&bin_sem4);
count++;
break;
}
}
if(count == 2000)
{
break;
}
}
}
void *capture_thread(void *arg)
{
 int i,ppm=1,sleep = 1000000;
 char a[15];
 CvCapture* capture;
 IplImage* frame;
 struct timespec curr_time;

 cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,
HRES);
 cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,
VRES);
 capture = cvCreateCameraCapture(2);
 for(i=0;i<4;i++)
 {
 frame = cvQueryFrame(capture);
 }
 for(i=0;i<2000;i++)
 { sem_wait(&bin_sem4);
 frame = cvQueryFrame(capture);
 sem_post(&bin_sem5);
 //clock_gettime(CLOCK_REALTIME, &curr_time);
 //timestamp[i] = curr_time.tv_sec;
 sprintf(a,"/mnt/ramfs/image%d.ppm",i);
 cvSaveImage(a,frame,&ppm);
 sem_post(&bin_sem1);
 
 }

 cvReleaseCapture(&capture);
}
void *header_thread(void *arg)
{
 FILE *fp1;
 FILE *fp2;
 int i=0;
 char uname[1000],date[100],out[1000],move[100];
 char a[15];
 IplImage* src;
for(i=0;i<2000;i++)
{
 sem_wait(&bin_sem1);
 /* Open the command for reading. */
 fp1 = popen("date", "r");
 if (fp1 == NULL) {
 printf("command fail\n" );
 exit(1);
 }
 /* Open the command for reading. */
 fp2 = popen("uname -a", "r");
 if (fp2 == NULL) {
 printf("command fail\n" );
 exit(1);
 }
 fgets(date, sizeof(date), fp1);
 fgets(uname, sizeof(uname), fp2);
 date[strcspn(date, "\n")] = 0;
 uname[strcspn(uname, "\n")] = 0;


 sprintf(out,"exiftool '-Comment=%s %s'
/mnt/ramfs/image%d.ppm",date,uname,i);
 system(out);
 sprintf(move,"mv /mnt/ramfs/image%d.ppm
/home/aasheesh/Desktop/Backup3/",i);
 system(move);
 /* close */
 pclose(fp1);
 pclose(fp2);
 sem_post(&bin_sem2);

}
pthread_exit(NULL);
}
void *sharpening_thread(void *arg)
{
int i, j, kernel_length = 31,jpg_index = 2;
Mat dst_frame;
IplImage* src;
char a[15],b[15],remove[100];
IplImage* result;
 for(i=0;i<2000;i++)
 { sem_wait(&bin_sem2);
 sprintf(a,"/mnt/ramfs/image%d.ppm_original",i);
 src = cvLoadImage(a);
 Mat mat_frame(src);
 GaussianBlur(mat_frame, dst_frame, Size(0, 0), 3);
 addWeighted(mat_frame, 1.5, dst_frame, -1, 0,
dst_frame);
 sprintf(b,"sharp_image%d.ppm",i);
 imwrite(b, dst_frame);
sprintf(remove,"rm /mnt/ramfs/image%d.ppm_original",i);
system(remove);
 sem_post(&bin_sem3);
 }
}
void *compression_thread(void *arg)
{
 int i, png_index = 2;
 char a[15],b[15];
 IplImage* comp_frame;
 double start,end;
 for(i = 0; i < 2000; i++)
 { sem_wait(&bin_sem3);
 sprintf(a,"sharp_image%d.ppm",i);
 comp_frame = cvLoadImage(a);
 sprintf(b,"image%d.jpg",i);
 cvSaveImage(b,comp_frame,&png_index);
 end = timestamp1();
 }
}
int main(void)
{
sem_init(&bin_sem1,0,0);
sem_init(&bin_sem2,0,0);
sem_init(&bin_sem3,0,0);
sem_init(&bin_sem4,0,1);
sem_init(&bin_sem5,0,0);
int ret,max_priority,min_priority;
pthread_t thread0,thread1,thread2,thread3,thread4;
pthread_attr_t
thread0_attr,thread1_attr,thread2_attr,thread3_attr,thread4_at
tr;
struct sched_param scheduling_value;
cpu_set_t core0,core1,core2,core3;
CPU_ZERO(&core0);
CPU_SET(0,&core0);
CPU_ZERO(&core1);
CPU_SET(1,&core1);
CPU_ZERO(&core2);
CPU_SET(2,&core2);
CPU_ZERO(&core3);
CPU_SET(3,&core3);
ret = pthread_attr_init(&thread0_attr);
if (ret != 0) {
printf("Attribute creation failed");
}
ret = pthread_attr_init(&thread1_attr);
if (ret != 0) {
printf("Attribute creation failed");
}
ret = pthread_attr_init(&thread2_attr);
if (ret != 0) {
printf("Attribute creation failed");
}
ret = pthread_attr_init(&thread3_attr);
if (ret != 0) {
printf("Attribute creation failed");
}
ret = pthread_attr_init(&thread4_attr);
if (ret != 0) {
printf("Attribute creation failed");
}
ret = pthread_attr_setschedpolicy(&thread0_attr, SCHED_FIFO);
if (ret != 0) {
printf("Setting Scheduling policy failed");
}
ret = pthread_attr_setschedpolicy(&thread1_attr, SCHED_FIFO);
if (ret != 0) {
printf("Setting Scheduling policy failed");
}
ret = pthread_attr_setschedpolicy(&thread2_attr, SCHED_FIFO);
if (ret != 0) {
printf("Setting Scheduling policy failed");
}
ret = pthread_attr_setschedpolicy(&thread3_attr, SCHED_FIFO);
if (ret != 0) {
printf("Setting Scheduling policy failed");
}
ret = pthread_attr_setschedpolicy(&thread4_attr, SCHED_FIFO);
if (ret != 0) {
printf("Setting Scheduling policy failed");
}
max_priority = sched_get_priority_max(SCHED_FIFO);
min_priority = sched_get_priority_min(SCHED_FIFO);
scheduling_value.sched_priority = max_priority;
ret = pthread_attr_setschedparam(&thread0_attr,
&scheduling_value);
if (ret != 0) {
printf("Setting scheduling priority failed");
}
scheduling_value.sched_priority = max_priority-10;
ret = pthread_attr_setschedparam(&thread1_attr,
&scheduling_value);
if (ret != 0) {
printf("Setting scheduling priority failed");
}
scheduling_value.sched_priority = max_priority-30;
ret = pthread_attr_setschedparam(&thread2_attr,
&scheduling_value);
if (ret != 0) {
printf("Setting scheduling priority failed");
}
scheduling_value.sched_priority = max_priority-40;
ret = pthread_attr_setschedparam(&thread3_attr,
&scheduling_value);
if (ret != 0) {
printf("Setting scheduling priority failed");
}
scheduling_value.sched_priority = max_priority-20;
ret = pthread_attr_setschedparam(&thread4_attr,
&scheduling_value);
if (ret != 0) {
printf("Setting scheduling priority failed");
}
pthread_attr_setaffinity_np(&thread0_attr, sizeof(cpu_set_t),
&core0);
pthread_attr_setaffinity_np(&thread1_attr, sizeof(cpu_set_t),
&core1);
pthread_attr_setaffinity_np(&thread2_attr, sizeof(cpu_set_t),
&core2);
pthread_attr_setaffinity_np(&thread3_attr, sizeof(cpu_set_t),
&core3);
pthread_attr_setaffinity_np(&thread4_attr, sizeof(cpu_set_t),
&core1);
ret = pthread_create(&thread0, &thread0_attr, timer_thread,
NULL);
if (ret != 0) {
printf("Thread creation failed");
}
ret = pthread_create(&thread1, &thread1_attr, capture_thread,
NULL);
if (ret != 0) {
printf("Thread creation failed");
}
ret = pthread_create(&thread2, &thread2_attr, sharpening_thread,
NULL);
if (ret != 0) {
printf("Thread creation failed");
}
ret = pthread_create(&thread3, &thread3_attr, compression_thread,
NULL);
if (ret != 0) {
printf("Thread creation failed");
}
ret = pthread_create(&thread4, &thread4_attr, header_thread,
NULL);
if (ret != 0) {
printf("Thread creation failed");
}
ret = pthread_join(thread0,NULL);
if (ret != 0) {
printf("Thread join failed");
}
ret = pthread_join(thread1,NULL);
if (ret != 0) {
printf("Thread join failed");
}
ret = pthread_join(thread2,NULL);
if (ret != 0) {
printf("Thread join failed");
}
ret = pthread_join(thread3,NULL);
if (ret != 0) {
printf("Thread join failed");
}
ret = pthread_join(thread4,NULL);
if (ret != 0) {
printf("Thread join failed");
}
/*int i;
int j=1;
signed int total_jitter = 0;
time_t total_exe = 0;
time_t match[10];
float avg_jitter;
float avg_exe;
for(i=1;i<2000;i++)
{
diff[i] = timestamp[i] - timestamp[i-1];
if(diff[i]==0)
{
match[j] == timestamp[i];
j++;
}
jitter[i] = diff[i]-1;
printf("time : %ld\n",timestamp[i-1]);
printf("diff : %ld\n",diff[i]);
printf("jitter : %d\n",jitter[i]);
}
for(i=1;i<2000;i++)
{
total_exe = total_exe+diff[i];
total_jitter = total_jitter+jitter[i];
avg_jitter = total_jitter/1999;
avg_exe = total_exe/1999;
}
/*for( i=1;i<=2000;i++)
{
printf("%ld\n",match[i]);
printf("%d\n",j);
}
printf("total_jitter : %d\n",total_jitter);
printf("avg_jitter : %f\n",avg_jitter);
printf("avg_exe_time : %f\n",av
