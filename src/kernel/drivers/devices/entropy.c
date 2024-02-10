#include "entropy.h"
#include "printk.h"
#include "stdint.h"
#include "config.h"

void feed_entropy(int num1, int num2, int num3, int num4, int num5)
{
    printk("feed_entropy: setting entropy seed\n");
    num1 = num1 * 2;
    num2 = num2 * 3;
    num3 = num3 * 4;
    seed = num1 * num2 + num3 * num4 + num5 * num1 / num4;
    printk("feed_entropy: seed is %d\n", seed);
}