# C TCP Socket Programming Examples

A collection of TCP client-server examples written in C to demonstrate how to build networked applications using low-level socket APIs on Linux.

## Contents

| Example | Description |
|--------|-------------|
| `00-basic-udp/` | A minimal **UDP** client and server example using `socket()`, `bind()`, `connect()`,`send()/recv()` and `sendto()/recvfrom()`. Demonstrates connectionless communication. |
| `01-basic-tcp/` | A minimal **TCP** client and server example using `socket()`, `bind()`, `listen()`, `accept()`, `select()` and `connect()`. Demonstrates reliable connection-oriented communication and basic server multiplexing. |

## Build and Run

Each example has its own `Makefile`. To build and run an example:

```bash
cd 00-basic-udp/
make

# Start the server in one terminal
./server

# Start the client in another terminal
./client 127.0.0.1 12345
```