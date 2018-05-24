/* klibf.c */
#include "kernel.h"
const int HZ = 100;
extern int get_ticks();
extern void print_str(char *str);

PUBLIC void delay(int milli_sec);
PUBLIC void reverse(char s[],int intlen);
PUBLIC void itoa(int n, char s[]);
PUBLIC void print_int(int num);

PUBLIC void delay(int milli_sec)
{

    int ticks = get_ticks();
    while( ((get_ticks() - ticks)*1000/HZ) < milli_sec ){}
}

PUBLIC void reverse(char s[], int intlen)
 {
     char c;
     for (int i = 0,j = intlen - 1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }


PUBLIC void itoa(int n, char s[])
{
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s,i);
}


PUBLIC void print_int(int num)
{
    char str[16]={0};
    itoa(num,str);
    print_str(str);
}
