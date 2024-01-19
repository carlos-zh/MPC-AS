# MPC-AS: Privacy Preserving Computation of Aggregate Statistics Against Malicious Adversaries

This repository includes our realization of MPC-AS, which is a secure MPC protocol for efficiently calculating aggregate statistics against malicious adversaries, i.e., ensuring security against malicious servers as well as protecting functionality against malicious clients. As long as one of the servers is not malicious, all the servers learn nothing about clients' private data except the outputs, and corrupted servers cannot change the output illegally without being detected. We theoretically prove the malicious security of our MPC-AS. In addition, experimental results demonstrate that our MPC-AS reduces the online computational costs of the most efficient semi-honest protocol several times.

## Requirements

*   GCC 5 or later (tested with up to 11) or LLVM/clang 6 or later (tested with up to 14). The default is to use clang because it performs better. Note that GCC 5/6 and clang 9 don't support libOTe, so you need to deactivate its use for these compilers (see the next section).
*   For protocols using oblivious transfer, libOTe with the necessary patches but without SimplestOT. The easiest way is to run make libote, which will install it as needed in a subdirectory. libOTe requires CMake of version at least 3.15, which is not available by default on older systems such as Ubuntu 18.04. You can run make cmake to install it locally. libOTe also requires boost of version at least 1.75, which is not available by default on relatively recent systems such as Ubuntu 22.04. You can install it locally by running make boost.
*   GMP library, compiled with C++ support (use flag --enable-cxx when running configure). Tested against 6.2.1 as supplied by Ubuntu.
*   libsodium library, tested against 1.0.18
*   OpenSSL, tested against 3.0.2
*   Boost.Asio with SSL support (libboost-dev on Ubuntu), tested against 1.81
*   Boost.Thread for BMR (libboost-thread-dev on Ubuntu), tested against 1.81
*   x86 or ARM 64-bit CPU (the latter tested with AWS Gravitron and Apple Silicon)
*   Python 3.5 or later
*   NTL library for homomorphic encryption (optional; tested with NTL 11.5.1)
    On Ubuntu, the following might suffice:
    `sudo apt-get install automake build-essential clang cmake git libboost-dev libboost-thread-dev libgmp-dev libntl-dev libsodium-dev libssl-dev libtool python3`
    For other operating systems, you can follow the instruction of [MP-SPDZ](https://mp-spdz.readthedocs.io/en/latest/).

## Compilation

The SPDZ protocol uses preprocessing, that is, in a first (sometimes called offline) phase correlated randomness is generated independent of the actual inputs of the computation. Only the second (“online”) phase combines this randomness with the actual inputs in order to produce the desired results. The preprocessed data can only be used once, thus more computation requires more preprocessing.

For the offline and online benchmarking, you can do the following steps:

*   make clean and compile the virtual machine for highgear protocol.

```Linux
make clean
make -j8 highgear-party.x
```

*   generate the necessary certificates and keys for servers and clients.

```Linux
Scripts/setup-ssl.sh 2
Scripts/setup-clients.sh 3
```

*   compile the client and servers' programs

```Linux
make bankers-bonus-client.x
./compile.py -P 140737488486401 bankers_bonus 1
```

For the online only benchmarking, you can do the following steps:

*   add MY\_CFLAGS = -DINSECURE to CONFIG.

*   compile the virtual machine.

<!---->

    make -j 8 online
    make Fake-Offline.x highgear-party.x
    Scripts/setup-online.sh

*   generate the necessary certificates and keys for servers and clients.

<!---->

    Scripts/setup-ssl.sh 2
    Scripts/setup-clients.sh 3

*   compile the client and servers' programs

<!---->

    make bankers-bonus-client.x
    ./compile.py -P 140737488355333 bankers_bonus 1

## Running Computation

For the localhost execution, you can do as follows.

*   online and offline benchmarking
    *   run on one terminal
    <!---->
        PLAYERS=2 Scripts/highgear.sh bankers_bonus-1
    *   run on another terminal
    <!---->
        ./bankers-bonus-client.x 0 2 1
*   online only benchmarking
    *   run on one terminal
    <!---->
        PLAYERS=2 Scripts/run-online.sh bankers_bonus-1
    *   run on another terminal
    <!---->
        ./bankers-bonus-client.x 0 2 1
