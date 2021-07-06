#include <iostream>
#include <stdexcept>
#include "fifo.hpp"

using namespace std;

typedef struct {
    int value;
} Example;

int main(int aArgumentCount, char* apArguments[]) {
    Example test = {0};
    Fifo<Example> queue(32);
    try {
        while(true) {
            test.value++;
            queue.push(&test);
            printf("Write: %u\n", test.value);
        }
    } catch (overflow_error& error) {
        cout << "Write failed: " << error.what() << endl;
    }

    try {
        while(true) {
            Example* obj = queue.pop();
            printf("Read: %u\n", obj->value);
        }
    } catch (underflow_error& error) {
        cout << "Read failed: " << error.what() << endl;
    }
    return 0;
}
