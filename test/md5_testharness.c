
#include <stdio.h>
#include <string.h>

#include <md5.h>


#if defined (__i386__)
 #define RDTSC(dst) { asm volatile ("rdtsc" : "=a" (dst) : : "edx"); }
 #define ITERATIONS 10
#else
 #define RDTSC(dst) { dst = 0; }
 #define ITERATIONS 1
#endif


static const char *rawdata = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
                             "bcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
                             "cdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
                             "defghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
                             "efghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
                             "fghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
                             "ghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
                             "hijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789";


/*
   You will just have to trust me that these are correct... :-)
*/

static const char hash_outer_expected_a[32+1] = "bc788dae573a93e79e421afa7750b7a9";   /* aligned test */
static const char hash_outer_expected_u[32+1] = "4385419c182a1d79f9a1b9e2a757265d";   /* unaligned test */


int main (int argc, char *argv[])
{
   int i, j;
   int errorcode = 0;

   unsigned int start;
   unsigned int stop;
   unsigned int fastest;

   MD5_CTX Context_inner;
   MD5_CTX Context_outer;

   unsigned char hash_inner[16];
   unsigned char hash_outer[16];

   char hash_outer_as_ascii[32+1];

   char *result;

   /*
      Test the aligned case (all hashed strings start from begining of rawdata).
   */

   /*
      Test the aligned case (all hashed strings start from begining of rawdata).
   */

   fastest = 0xffffffff;

   for (i = 0; i < ITERATIONS; i++)
   {
      RDTSC(start);

      MD5Init (&Context_outer);

      for (j = strlen(rawdata); j; j--)
      {
         MD5Init   (&Context_inner);
         MD5Update (&Context_inner, rawdata, j);
         MD5Final  (hash_inner, &Context_inner);
         
         MD5Update (&Context_outer, hash_inner, sizeof(hash_inner));
      }

      MD5Final (hash_outer, &Context_outer);
   
      RDTSC(stop);
      
      fastest = ((stop - start) < fastest) ? (stop - start) : fastest;
   }      
   
   sprintf (hash_outer_as_ascii, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            hash_outer[ 0], hash_outer[ 1], hash_outer[ 2], hash_outer[ 3],
            hash_outer[ 4], hash_outer[ 5], hash_outer[ 6], hash_outer[ 7],
            hash_outer[ 8], hash_outer[ 9], hash_outer[10], hash_outer[11],
            hash_outer[12], hash_outer[13], hash_outer[14], hash_outer[15]);

   if (strcmp (hash_outer_expected_a, hash_outer_as_ascii) == 0)
   {
      result = " (Passed)";
   }
   else
   {
      result = " (Failed !!)";
      errorcode++;
   }
   
   printf ("\n");
   printf ("Aligned\n");
   printf ("-------\n");
   printf ("Expected Outer Hash = %s\n", hash_outer_expected_a);
   printf ("Actual Outer Hash   = %s%s\n", hash_outer_as_ascii, result);
   printf ("Execution time      = %d\n", fastest);
   printf ("\n");


   /*
      Test the unaligned case (all hashed strings end at the end of rawdata).
   */

   fastest = 0xffffffff;

   for (i = 0; i < ITERATIONS; i++)
   {
      int datalength = strlen(rawdata);

      RDTSC(start);

      MD5Init (&Context_outer);

      for (j = datalength; j; j--)
      {
         MD5Init   (&Context_inner);
         MD5Update (&Context_inner, &rawdata[j], (datalength - j));
         MD5Final  (hash_inner, &Context_inner);
         
         MD5Update (&Context_outer, hash_inner, sizeof(hash_inner));
      }

      MD5Final (hash_outer, &Context_outer);
   
      RDTSC(stop);
      
      fastest = ((stop - start) < fastest) ? (stop - start) : fastest;
   }      
   
   sprintf (hash_outer_as_ascii, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            hash_outer[ 0], hash_outer[ 1], hash_outer[ 2], hash_outer[ 3],
            hash_outer[ 4], hash_outer[ 5], hash_outer[ 6], hash_outer[ 7],
            hash_outer[ 8], hash_outer[ 9], hash_outer[10], hash_outer[11],
            hash_outer[12], hash_outer[13], hash_outer[14], hash_outer[15]);

   if (strcmp (hash_outer_expected_u, hash_outer_as_ascii) == 0)
   {
      result = " (Passed)";
   }
   else
   {
      result = " (Failed !!)";
      errorcode++;
   }
   
   printf ("\n");
   printf ("Unaligned\n");
   printf ("---------\n");
   printf ("Expected Outer Hash = %s\n", hash_outer_expected_u);
   printf ("Actual Outer Hash   = %s%s\n", hash_outer_as_ascii, result);
   printf ("Execution time      = %d\n", fastest);
   printf ("\n");
   
   return (errorcode);
}
