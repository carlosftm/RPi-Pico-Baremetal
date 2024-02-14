# 14_bootrom_func_data

This time we are exploring bootrom of the RP2040. According to the RP2040 datasheet: ```The bootrom is software that is built into the chip, performing the "processor controlled" part of the boot sequence.```

Additioanlly, the bootrom provides:
-- Routines for programming and manipulating the external flash.
-- Fast floating point library.
-- Fast bit counting / manipulation functions.
-- Fast memory fill / copy functions.

This example access the bootrom by:
- [x] read the copyright string
- [x] call the popcount32() function
- [x] call the memset() function

The key here is to understand the function pointers:
```A function pointer in C is a variable that can store the memory address of a function. It allows you to treat functions as data, enabling you to pass functions as arguments to other functions, store them in data structures, and call them indirectly through the function pointer.```

Here an example on how a function pointer works:

```
// Function declaration
int add(int a, int b) {
    return a + b;
}

int main() {
    // Declare a function pointer
    int (*operation)(int, int);

    // Assign the address of the add function to the pointer
    operation = &add;

    // Call the function indirectly through the function pointer
    int result = (*operation)(3, 4);

    printf("The result is: %d\n", result);

    return 0;
}
```