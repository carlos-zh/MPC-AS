

#ifndef EXTERNALIO_CLIENT_H_
#define EXTERNALIO_CLIENT_H_

#include "Networking/ssl_sockets.h"

const char statistic_name = 'S';                 // 聚合统计量
// 如果是SUM，number_bits为1 8 16 32；如果是MAX或者FRQ，number_bits为16 32 64 128

// const int number_elements = 8;                // MAX的范围，也等于编码位数 number_bits
const int number_variants = 1;                   // 一个数据中元素的个数（除了LR之外，都是1）
const int number_bits = 8 * number_variants;    // 编码位数
const int number_inputs = 1000;                    // 一个客户端持有的数据个数
const char communication_method = 'S';           // 通信方式，传一个(Single)p域数字，传多个(Multiple){-1,1}的数字

#ifdef NO_CLIENT_TLS

class client_ctx
{
public:
    client_ctx(string)
    {
    }
};

class client_socket
{
public:
    int socket;

    client_socket(boost::asio::io_service&,
            client_ctx&, int plaintext_socket, string,
            string, bool) : socket(plaintext_socket)
    {
    }

    ~client_socket()
    {
        close(socket);
    }
};

inline void send(client_socket* socket, octet* data, size_t len)
{
    send(socket->socket, data, len);
}

inline void receive(client_socket* socket, octet* data, size_t len)
{
    receive(socket->socket, data, len);
}

#else

typedef ssl_ctx client_ctx;
typedef ssl_socket client_socket;

#endif

/**
 * Client-side interface
 */
class Client
{
    vector<int> plain_sockets;
    client_ctx ctx;
    ssl_service io_service;

public:
    /**
     * Sockets for cleartext communication
     */
    vector<client_socket*> sockets;

    /**
     * Specification of computation domain
     */
    octetStream specification;

    /**
     * Start a new set of connections to computing parties.
     * @param hostnames location of computing parties
     * @param port_base port base
     * @param my_client_id client identifier
     */
    Client(const vector<string>& hostnames, int port_base, int my_client_id);
    ~Client();

    /**
     * Securely input private values.
     * @param values vector of integer-like values
     */
    template<class T>
    // void send_private_inputs(const vector<T>& values);
    void send_private_inputs(const vector< vector<T> >& values);

    template<class T>
    // void send_private_inputs(const vector<T>& values);
    void send_private_inputs_longint(const vector< vector<T> >& values);

    template<class T>
    // void send_private_inputs_LR(const vector<T>& values);
    void send_private_inputs_LR(const vector< vector<T> >& values);

    template<class T>
    // void send_private_inputs_MAX(const vector<T>& values);
    void send_private_inputs_MAX(const vector< vector<T> >& values);

    template<class T>
    // void send_private_inputs_MAX(const vector<T>& values);
    void send_private_inputs_AND(const vector< vector<T> >& values);

    // template<class T>
    // // void send_private_inputs_final(const vector<T>& values);
    // void send_private_inputs_final(const vector< vector<T> >& values);

    // template<class T>
    // vector<T> toBinary(T value, int number_bits);

    // template<class T>
    // void send_inputs(const vector<T>& values, int number_bits);

    // /**
    //  * Securely receive output values.
    //  * @param n number of values
    //  * @returns vector of integer-like values
    //  */
    // template<class T, class U = T>
    // vector<U> receive_outputs(int n);
};

#endif /* EXTERNALIO_CLIENT_H_ */

