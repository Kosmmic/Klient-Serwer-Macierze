Language: [English] | [Po polsku](README.pl.md)

---

# Concurrent IPC Computing Server (C / Linux)

This project implements a client-server system in a Linux environment using System V IPC mechanisms and asynchronous process management in POSIX.

## Features
* **Concurrent Server:** Handles multiple clients simultaneously using child processes (`fork()`). Features an automatic cleanup mechanism for zombie processes via `SIGCHLD`.
* **System V Communication:** Data exchange (matrices and computation results) is handled via system message queues (`msgget`, `msgsnd`, `msgrcv`).
* **Text User Interface (TUI):** The client features a dynamic text interface built using the `ncurses` library.

### 1. Communication Layer (System V Message Queues)
Information exchange takes place through a single system message queue identified by a unique numerical key. The queue acts as an asynchronous FIFO (First-In, First-Out) buffer that handles two types of messages (identified by the `long mtype` field):
* **Mtype = 1 (Computation Request):** The client places a structure in the queue containing its unique Process Identifier (PID), matrix dimensions ($M \times N$), and a two-dimensional array of real numbers (`double`).
* **Mtype = Client's PID (Response):** The server sends back a structure containing the computed result (sum of elements). Using the PID as the message type ensures complete data isolation—in a multi-client environment, each client retrieves only the response packet dedicated to them.

### 2. Server Concurrency and Zombie Process Elimination
The main server process (parent) runs in an infinite loop, blocking on the `msgrcv()` function while waiting for messages of type 1. Upon receiving a structure, the server does not perform computations sequentially. Instead, it immediately calls the `fork()` system function to create an isolated child process:
* **Main Process (Parent):** Instantly returns to the beginning of the loop to wait for new requests via `msgrcv()`, remaining fully responsive to other clients.
* **Child Process:** Inherits a copy of the message data, executes the matrix summation algorithm, sends the result using `msgsnd()` with `mtype = PID`, and terminates via `exit(0)`.

## System Requirements
* Operating System: **Linux**
* Compiler: `gcc`
* Developer Library: `libncurses-dev` (or equivalent for your distribution)

On Ubuntu/Debian, install the ncurses library with:
```bash
sudo apt update && sudo apt install libncurses5-dev libncursesw5-dev
```

## Compilation
The project includes a `Makefile`. To compile both programs, run:
```bash
make
```
To clean up binary files, run:
```bash
make clean
```

## Running the Application
The IPC mechanism requires the server to be started **first**:
1. In the first terminal window, start the server: `./serwer`
2. In a second window (or multiple separate windows), start the client: `./klient`

## File Structure
* `serwer.c` – Source code for the concurrent server.
* `klient.c` – Source code for the client with the TUI interface (`ncurses`).
* `wspolny.h` – Common header file containing IPC structure definitions and the queue key.
* `Makefile` – Script automating the compilation process.
