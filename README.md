# MPC-AS: Privacy Preserving Computation of Aggregate Statistics Against Malicious Adversaries

This repository includes the implementation of our **MPC-AS**, which is a secure multi-party computation (MPC) protocol for efficiently calculating aggregate statistics against malicious adversaries, i.e., ensuring security against malicious servers as well as protecting functionality against malicious clients. As long as one of the servers is not malicious, all the servers learn nothing about clients' private data except the outputs, and corrupted servers cannot change the output illegally without being detected. We theoretically prove the malicious security of our MPC-AS. In addition, experimental results demonstrate that our MPC-AS reduces the online computational costs of the most efficient semi-honest protocol several times.


## Requirements

[MP-SPDZ](https://github.com/data61/MP-SPDZ) is a widely-used MPC platform. We utilize this platform to implement our **MPC-AS** protocol.

On Linux, MP-SPDZ requires a working toolchain and [all requirements](https://github.com/data61/MP-SPDZ?tab=readme-ov-file#requirements). On Ubuntu, the following might suffice:

    sudo apt-get install automake build-essential clang cmake git libboost-dev libboost-thread-dev libgmp-dev libntl-dev libsodium-dev libssl-dev libtool python3

> Please note that the CMake version should be at least 3.15, which is not available by default on older systems such as Ubuntu 18.04. You can install it manually ([CMake](https://cmake.org/download/)).

For other operating systems, you can follow the instructions of [MP-SPDZ](https://mp-spdz.readthedocs.io/en/latest/).



## Dataset

We constructed datasets for different domains with varying statistics. Each dataset consists of 100,000 private inputs, simulating a scenario involving 100,000 clients. 

The datasets include both cases with all valid inputs and cases with partially invalid inputs. The choice between these two cases can be made by adjusting the `<is_valid>` parameter. In the dataset with partially invalid inputs, there are 10 invalid inputs positioned at 1, 10001, 20001, 30001, ..., and 90001. It is worth noting that the MAX statistic does not require validation checks for data inputs due to its unique encoding method.


## Supported Protocols

*   SUM/MEAN: sum/mean of `<key_param>`-bit integers 
*   VAR/STD: variance/standard deviation of `<key_param>`-bit integers
*   MAX/MIN: integer maximum/minimum, with values in the set [0, `<key_param>`)
*   FRQ: frequency count, with values in the set [0, `<key_param>`)
*   LR: `<key_param>` degree linear regression on 8-bit integers

### Parameters
* Server arguments: `<statistic_name>, <key_param>, <client_num>`
* Client arguments: `<statistic_name>, <key_param>, <is_valid>, <client_num>`
* `<statistic_name>` is used to select the desired statistic function to compute. You can set this variable as `S, V, F, M, L`, representing SUM, VAR, FRQ, MAX, and LR, respectively. Note that the implementation of MEAN, STD, and MIN is similar to that of SUM, VAR, and MAX respectively.
* `<key_param>` determines the data domain of clients.
  * For SUM/MEAN/VAR/STD, `<key_param>` represents the input bit length. You can set this variable as `2, 4, 8, 16`.
  * For MAX/MIN/FRQ, `<key_param>` represents the input range length. You can set the variable as `10, 20, 30, 40`.
  * For LR: `<key_param>` denotes the degree of linear regression. You can set this variable as `2, 3, 4, 5`.
* `<is_valid>` is used to specify whether the dataset is valid or partially invalid. You can set this variable as `Y` for valid datasets or `N` for invalid datasets.
* `<client_num>` is used to specify the number of clients. You can set this variable as an integer less than 100,000, as our datasets support inputs for up to 100,000 clients.

## Online experiments
Our MPC-AS protocol consists of two phases: the offline phase and the online phase.
To execute the online phase, please follow the steps below:

### Compilation
* run the following command to prepare the environment, which includes compiling the virtual machine, generating certificates, and generating keys.
    ```
    ./online-setup.sh
    ```
* compile the programs for both the clients and servers to support a specific statistical function.
    ```
    ./online-compile.sh <statistic_name> <key_param> <client_num>
    ```
### Running computation
* run the servers' program on one terminal.
    ```
    ./online-server-run.sh <statistic_name> <key_param> <client_num>
    ```
* run the clients' program on another terminal.
    ```
    ./online-client-run.sh <statistic_name> <key_param> <is_valid> <client_num>
    ```


## Offline-online experiments
To execute both the offline and online phases, follow the steps below:

### Compilation
* run the following command to prepare the environment, which includes compiling the virtual machine, generating certificates, and generating keys.
    ```
    ./complete-setup.sh
    ```
* compile the programs for both the clients and servers to support a specific statistical function.
    ```
    ./complete-compile.sh <statistic_name> <key_param> <client_num>
    ```
### Running computation
* run the servers' program on one terminal.
    ```
    ./complete-server-run.sh <statistic_name> <key_param> <client_num>
    ```
* run the clients' program on another terminal.
    ```
    ./complete-client-run.sh <statistic_name> <key_param> <is_valid> <client_num>
    ```

## Usage example
To calculate the SUM for 1000 clients, each possessing a 16-bit integer, follow the steps below:
1. compile the virtual machine and generate the necessary certificates and keys: `./online-setup.sh`
2. compile the clients' and servers' programs for computing SUM `./online-compile.sh S 16 1000`
3. run the server program on one terminal: `./online-server-run.sh S 16 1000`
4. run the client program on another terminal: `./online-client-run.sh S 16 N 1000`
