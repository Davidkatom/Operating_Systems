# Operating Systems EX3

This repository contains the source code for the EX3 project in the Operating Systems course. The project is structured as follows:

## Files

### bounded_buffer.c & bounded_buffer.h

These files implement a bounded buffer using a circular array. The buffer has a fixed size and supports insertion and removal operations.

### dispatcher.c & dispatcher.h

The dispatcher is responsible for managing the producers and co-editors. It creates and initializes the producers and co-editors, and starts their threads.

### main.c & main.h

The main function of the program. It reads the configuration file, creates the dispatcher and starts the producer and co-editor threads.

### producer.c & producer.h

The producer creates items and inserts them into its buffer. The items are strings that represent news items.

### screenManager.c & screenManager.h

The screen manager is responsible for printing the news items to the screen.

## How to Run

To run the program, compile the source code and run the resulting executable with the path to the configuration file as an argument.

```bash
gcc -o ex3 main.c bounded_buffer.c dispatcher.c producer.c screenManager.c -lpthread
./ex3 config.txt
```

The configuration file should specify the number of producers, the number of items each producer should produce, and the size of the co-editor queues.
## Configuration File

The configuration file (`conf.txt`) should contain the following parameters for each producer:

- `Producer id`: The id of the producer.
- `Num of items`: The number of items each producer should produce.
- `Queue size`: The size of the producer's queue.

Each parameter should be on a separate line. After the parameters for all producers, the size of the co-editor queue should be specified. For example:

```
1
10
100
2
20
200
3
30
300
1000
```

This configuration will create 3 producers with ids 1, 2, and 3. The producers will produce 10, 20, and 30 items respectively, and their queue sizes will be 100, 200, and 300. The size of the co-editor queue will be 1000.
