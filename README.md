# MPC-AS: Privacy Preserving Computation of Aggregate Statistics Against Malicious Adversaries

This repository includes our realization of **MPC-AS**, which is a secure MPC protocol for efficiently calculating aggregate statistics against malicious adversaries, i.e., ensuring security against malicious servers as well as protecting functionality against malicious clients. As long as one of the servers is not malicious, all the servers learn nothing about clients' private data except the outputs, and corrupted servers cannot change the output illegally without being detected. We theoretically prove the malicious security of our MPC-AS. In addition, experimental results demonstrate that our MPC-AS reduces the online computational costs of the most efficient semi-honest protocol several times.

## Getting Started

[MP-SPDZ](https://github.com/data61/MP-SPDZ) is a framework for multi-party computation, a privacy-enhancing technology focused on input privacy. We utilize this widely-used MPC framework MP-SPDZ to implement our **MPC-AS**.

### Requirements

On Linux, this requires a working toolchain and [all requirements](https://github.com/data61/MP-SPDZ#requirements). On Ubuntu, the following might suffice:

    sudo apt-get install automake build-essential clang cmake git libboost-dev libboost-thread-dev libgmp-dev libntl-dev libsodium-dev libssl-dev libtool python3

> Notice that the CMake version should be at least 3.15, which is not available by default on older systems such as Ubuntu 18.04. You can install it manually ([CMake](https://cmake.org/download/)).

For other operating systems, you can follow the instructions of [MP-SPDZ](https://mp-spdz.readthedocs.io/en/latest/).

### Download

Use `--recursive` to download corresponding dependencies in `./deps` file.

    git clone --recursive https://github.com/carlos-zh/MPC-AS.git

## Dataset

To demonstrate the malicious security of our MPC-AS protocol, we constructed datasets for four representative statistical measures: SUM, VAR, FRQ, and LR. The datasets include cases with all valid inputs and cases with some invalid inputs. You can choose between using a dataset with all valid inputs or a dataset with some invalid inputs by adjusting the `<is_valid>` parameter.

All datasets consist of 100k private values, simulating the scenario of inputs from 100k clients. For each dataset with some invalid inputs, there are 10 invalid data points located at positions 1, 10001, 20001, 30001, ..., 90001. The MAX statistic does not require an invalid dataset because of its special encoding method.


## Supported Protocols

*   SUM/MEAN: `<key_param>`-bit integers sum/mean
*   VAR/STD: variance/standard deviation of `<key_param>`-bit integers
*   MAX/MIN: integer maximum/minimum, for values vary between 0 and `<key_param>`-1
*   FRQ: frequency count, for values vary between 0 and `<key_param>`-1
*   LR: `<key_param>` degree linear regression on 8-bit integers

### Parameters
* Server arguments are `<statistic_name>, <key_param>, <client_number>`
* Client arguments are `<statistic_name>, <key_param>, <is_valid>, <client_number>`
* `<statistic_name>` is used to choose which statistic function you want to compute, you can set the variable as `S, V, F, M, L`, which stands for SUM, VAR, FRQ, MAX, and LR, respectively.
* `<key_param>` determines the domain of clients' data.
  * For SUM/MEAN/VAR/STD, `<key_param>` means the encoding bit length, you can set the variable as `2, 4, 8, 16`.
  * For MAX/MIN/FRQ, `<key_param>` represents the input value range, you can set the variable as `10, 20, 30, 40`.
  * For LR: `<key_param>` means the linear regression degree, you can set the variable as `2, 3, 4, 5`.
* `<is_valid>` is used for selecting valid or invalid datasets, you can set the variable as `Y` for valid dataset or `N` for invalid dataset.

## Online-only experiments
Our MPC-AS protocol is divided into the offline phase and online phase.
For the online-only experiments, you can do the following steps:

### Compilation
* run the following command for environment preparation including the virtual machine compilation and necessary certificates and keys generation.
    ```
    ./online-setup.sh
    ```
* compile the clients' and servers' programs for certain statistic function.
    ```
    ./online-compile.sh <statistic_name> <key_param> <client_number>
    ```
### Running computation
* run the server program on one terminal.
    ```
    ./online-server-run.sh <statistic_name> <key_param> <client_number>
    ```
* run the client program on another terminal.
    ```
    ./online-client-run.sh <statistic_name> <key_param> <is_valid> <client_number>
    ```


## Offline-online experiments
For the offline and online experiments, you can do the following steps:

### Compilation
* run the following command for environment preparation including the virtual machine compilation and necessary certificates and keys generation.
    ```
    ./complete-setup.sh
    ```
* compile the clients' and servers' programs for certain statistic function.
    ```
    ./complete-compile.sh <statistic_name> <key_param> <client_number>
    ```
### Running computation
* run the server program on one terminal.
    ```
    ./complete-server-run.sh <statistic_name> <key_param> <client_number>
    ```
* run the client program on another terminal.
    ```
    ./complete-client-run.sh <statistic_name> <key_param> <is_valid> <client_number>
    ```

## Usage example

1. compile the virtual machine and generate necessary certificates: `./online-setup.sh`
2. compile the clients' and servers' programs for certain statistic function `./online-compile.sh S 2 100000`
3. run the server program on one terminal: `./online-server-run.sh S 2 100000`
4. run the client program on another terminal: `./online-client-run.sh S 2 N 100000`
