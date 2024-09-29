#include <kernel/tty/tty.h>
#include <kprintf.h>
#include <stdint.h>

static void print_string(const char* str)
{
    tty_put_str(str);
}

static void print_char(char c)
{
    tty_put_char(c);
}

static void print_int(int num)
{
    static char digits[] = "0123456789";

    if (num < 0)
    {
        print_char('-');
        num = -num;
    }

    if (num > 9)
    {
        print_int(num / 10);
    }

    print_char(digits[num % 10]);
}


static void print_hex(uint32_t num)
{
    const char* hex_chars = "0123456789ABCDEF";
    char buffer[9];  // 8 hex digits + null terminator
    buffer[8] = '\0';

    for (int i = 7; i >= 0; i--)
    {
        buffer[i] = hex_chars[num & 0xF];  // Get the last nibble (4 bits)
        num >>= 4;                         // Shift the number by 4 bits
    }

    print_string("0x");
    print_string(buffer);
}

void kprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    for (size_t i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%' && format[i + 1] != '\0')
        {
            i++;
            switch (format[i])
            {
            case 'c':
                print_char(va_arg(args, int));
                break;
            case 's':
                print_string(va_arg(args, char*));
                break;
            case 'd':
                print_int(va_arg(args, int));
                break;
            case 'p':
            case 'x':
                print_hex(va_arg(args, uint32_t));
                break;
            default:
                print_char('%');
                print_char(format[i]);
                break;
            }
        }
        else
        {
            print_char(format[i]);
        }
    }

    va_end(args);
}