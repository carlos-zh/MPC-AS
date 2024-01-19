
#include "Client.h"
//#include "Math/BitVec.h"     //@@@@@@@@@@@@@@@@@@@@@@@@@@
#include "Math/Bit.h"
// #include <cstdlib>
#include<stdlib.h>
#include <time.h>
#include <cmath>
inline
Client::Client(const vector<string>& hostnames, int port_base,
        int my_client_id) :
        ctx("C" + to_string(my_client_id))
{
    bigint::init_thread();

    // Setup connections from this client to each party socket
    int nparties = hostnames.size();
    plain_sockets.resize(nparties);
    sockets.resize(nparties);
    for (int i = 0; i < nparties; i++)
    {
        // set socket
        set_up_client_socket(plain_sockets[i], hostnames[i].c_str(), port_base + i);
        // send id
        octetStream(to_string(my_client_id)).Send(plain_sockets[i]);
        // save socket in sockets 
        sockets[i] = new client_socket(io_service, ctx, plain_sockets[i],
                "P" + to_string(i), "C" + to_string(my_client_id), true);
        if (i == 0)
            specification.Receive(sockets[0]);
    }
}

inline
Client::~Client()
{
    for (auto& socket : sockets)
    {
        delete socket;
    }
}

////////////////////////统计量：SUM FRQ ///////////////////////

template<class T>
void Client::send_private_inputs(const vector< vector<T> >& values)
{
    int num_inputs = values.size();
    octetStream os;
    vector< vector<gfp> > triples(num_inputs, vector<gfp>(4));
    vector<gfp> triple_shares(4);
    bool active = true;

    /// Receive num_inputs triples from SPDZ
    /// Triples data are encrypted values used for secure computation
    for (size_t j = 0; j < sockets.size(); j++)
    {
#ifdef VERBOSE_COMM
        cerr << "receiving from " << j << endl << flush;
#endif

        os.reset_write_head();
        os.Receive(sockets[j]);

#ifdef VERBOSE_COMM
        cerr << "received " << os.get_length() << " from " << j << endl << flush;
#endif

        if (j == 0)
        {
            if (os.get_length() == 4 * values.size() * gfp::size())
                active = true;
            else
                active = false;
        }
        cout << j <<endl;
        cout << values.size() <<endl;

        int n_expected = active ? 4 : 1;
        if (os.get_length() != n_expected * gfp::size() * values.size())
            throw runtime_error("unexpected data length in sending");

        for (int j = 0; j < num_inputs; j++)
        {
            for (int k = 0; k < n_expected; k++)
            {
                triple_shares[k].unpack(os);
                triples[j][k] += triple_shares[k];
            }
        }
    }


    clock_t start,end, start_1, end_1, start_2, end_2;  //定义clock_t变量 /////////////////////////////////////
    start = clock();    //开始时间        /////////////////////////////////////
    start_1 = clock();
    if (active)
        /// Check triple relations (is a party cheating?)
        for (int i = 0; i < num_inputs; i++)
        {
            // if (i == 1){
            //     cout << triples[i][0] << endl;
            //     cout << triples[i][1] << endl;
            //     cout << triples[i][2] << endl;
            //     cout << triples[i][3] << endl;
            //     cout << triples[i][4] << endl;
            // }
            
            /// check the relationship of triple to judge cheat
            if (triples[i][0] * triples[i][0] != triples[i][1])
            {
                
                //cerr << triples[i][2] << " != " << triples[i][0] << " * " << triples[i][1] << endl;
                cerr << "Incorrect triple1 at " << i << ", aborting\n";
                throw mac_fail();
            }
            if (triples[i][0] * triples[i][2] != triples[i][3])
            {
                
                //cerr << triples[i][2] << " != " << triples[i][0] << " * " << triples[i][1] << endl;
                cerr << "Incorrect triple2 at " << i << ", aborting\n";
                throw mac_fail();
            }
        }
    end_1 = clock();    

    // Send inputs + triple[0], so SPDZ can compute shares of each value
    
    
    os.reset_write_head();
    
    gfp tmp_value, tmp_value2, y;
    T middle_value, middle_value2, tmp_pow, middle_y;
    vector<T> r_array(number_bits);
    start_2 = clock();

    
    end_2 = clock();
    int mask_bit;
    vector<T> x_array(number_bits, -1);
    for (int i = 0; i < num_inputs; i++)
    {
        mask_bit = 1 << 0;
        tmp_value = triples[i][2];
        for (int b = 0; b < number_bits; b++)
        {

            tmp_value2 = tmp_value;
            tmp_value2 &= mask_bit;
            r_array[b] = (tmp_value2 == mask_bit) * 2 - 1;
            /// 计算 r_array[b]，根据 triples[i][2] 是否等于 1，如果是则为 1，否则为 -1，转为二进制
            // cout << "r_array[" << b << "] = " << r_array[b] << endl;        // @@@@@@@@@@@@@@@
            mask_bit = mask_bit << 1;
        }

        // x_array

        switch (statistic_name)
        {
        // SUM
        /// Map each bit in its binary representation into the array x_array
        case 'S':
        {
            mask_bit = 1;
            middle_value = values[i][0];
            for (int b = 0; b < number_bits; b++)
            {
                middle_value2 = middle_value;
                middle_value2 &= mask_bit;
                if (middle_value2 == mask_bit)
                {
                    x_array[b] = 1;
                }
                else
                {
                    x_array[b] = -1;
                }
                
                mask_bit <<= 1;
            }
            break;
        }

        // VAR
        /// Map each bit in its binary representation into the array x_array
        case 'V':
        {
            mask_bit = 1;
            middle_value = values[i][0];
            for (int b = 0; b < number_bits; b++)
            {
                middle_value2 = middle_value;
                middle_value2 &= mask_bit;
                if (middle_value2 == mask_bit)
                {
                    x_array[b] = 1;
                }
                else
                {
                    x_array[b] = -1;
                }
                
                mask_bit <<= 1;
            }
            break;
        }

        // FRQ
        /// set corresponding bit to 1
        case 'F':
        {
            middle_value = values[i][0];
            x_array[middle_value] =1;
            break;
        }      

        default:
            cerr << "Statistic name " << statistic_name << " not implemented";
            exit(1);
        }


        /// Data transmission method
        // 第一种传输方式，传输一个十进制的p域值
        if (communication_method == 'S')
        {
            middle_y = 0;
            y = 0;        // 十进制Y，明文
            tmp_pow = 1;
            // cout << "y" <<endl;
            for (int j = 0; j < number_bits; j++)
            {
                y += (r_array[j] * x_array[j] + 1)/2  * tmp_pow;
                tmp_pow = tmp_pow * 2;
            }
            // y = y;
            y.pack(os);
        }
        
        // 第二种传输方式，传输一串二进制的值(这里还是p域值，一般是-1或1)
        else if (communication_method == 'M')
        {
            for (int j = 0; j < number_bits; j++)
            {
                y = r_array[j] * x_array[j];
                y.pack(os);
                // std::cout << "Test y" << j << " = " << y << endl;
            }
        }

        else
            throw runtime_error("unexpected communication method");

    }
    
    /// send results
    for (auto& socket : sockets)
        os.Send(socket);
    end = clock();                                                        //结束时间 /////////////////////
    cout<<"client running time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;   //输出时间（单位：ｓ）//////////
    cout<<"Time1 = "<<double(end_1-start_1)/CLOCKS_PER_SEC<<"s"<<endl;
    cout<<"Time2 = "<<double(end_2-start_2)/CLOCKS_PER_SEC<<"s"<<endl;
}



template<class T>
void Client::send_private_inputs_longint(const vector< vector<T> >& values)
{
    int num_inputs = values.size();
    octetStream os;
    vector< vector<gfp> > triples(num_inputs, vector<gfp>(4));
    vector<gfp> triple_shares(4);
    bool active = true;

    // Receive num_inputs triples from SPDZ
    for (size_t j = 0; j < sockets.size(); j++)
    {
#ifdef VERBOSE_COMM
        cerr << "receiving from " << j << endl << flush;
#endif

        os.reset_write_head();
        os.Receive(sockets[j]);

#ifdef VERBOSE_COMM
        cerr << "received " << os.get_length() << " from " << j << endl << flush;
#endif

        if (j == 0)
        {
            if (os.get_length() == 4 * values.size() * gfp::size())
                active = true;
            else
                active = false;
        }

        int n_expected = active ? 4 : 1;
        if (os.get_length() != n_expected * gfp::size() * values.size())
            throw runtime_error("unexpected data length in sending");

        for (int j = 0; j < num_inputs; j++)
        {
            for (int k = 0; k < n_expected; k++)
            {
                triple_shares[k].unpack(os);
                triples[j][k] += triple_shares[k];
            }
        }
    }


    // clock_t start,end, middle1, middle2, test1, test2;  //定义clock_t变量 /////////////////////////////////////
    clock_t start,end;                                     //定义clock_t变量 /////////////////////////////////////
    start = clock();    //开始时间        /////////////////////////////////////

    if (active)
        // Check triple relations (is a party cheating?)
        for (int i = 0; i < num_inputs; i++)
        {
            if (triples[i][0] * triples[i][0] != triples[i][1])
            {
                
                //cerr << triples[i][2] << " != " << triples[i][0] << " * " << triples[i][1] << endl;
                //cerr << "Incorrect triple at " << i << ", aborting\n";
                throw mac_fail();
            }
            if (triples[i][0] * triples[i][2] != triples[i][3])
            {
                
                //cerr << triples[i][2] << " != " << triples[i][0] << " * " << triples[i][1] << endl;
                //cerr << "Incorrect triple at " << i << ", aborting\n";
                throw mac_fail();
            }
        }
        

    // Send inputs + triple[0], so SPDZ can compute shares of each value
    
    
    os.reset_write_head();
    
    gfp tmp_value, tmp_value2, y;
    T middle_value, middle_value2, tmp_pow, middle_y;
    vector<T> r_array(number_bits);
    
    // 这个赋值语句非常消耗时间
    // tmp_value = triples[0][0];
    
    // int和gfp无法转换
    //T tmp_value, tmp_value2;
    //vector<T> r_array(number_bits);
    int mask_bit;
    vector<T> x_array(number_bits, -1);
    for (int i = 0; i < num_inputs; i++)
    {
        // r_array  总共时间是0.072，r_array占用0.057，
        mask_bit = 1<<0;
        tmp_value = triples[i][2];
        for (int b = 0; b < 31; b++)
        {
            tmp_value2 = tmp_value;
            tmp_value2 &= mask_bit;
            r_array[b] = (tmp_value2 == mask_bit)*2-1;
            mask_bit = mask_bit << 1;
        }
        for (int b=31; b< number_bits; b++)
        {
            tmp_value2 = tmp_value;
            tmp_value2 = tmp_value2 >>b;
            tmp_value2 &= 1;
            r_array[b] = (tmp_value2 == 1)*2-1;
        }

        // x_array

        switch (statistic_name)
        {
        // SUM
        case 'S':
        {
            middle_value = values[i][0];
            for (int b = 0; b < number_bits; b++)
            {
                middle_value2 = middle_value;
                middle_value2 &= 1;
                if (middle_value2 == 1)
                {
                    x_array[b] = 1;
                }
                else
                {
                    x_array[b] = -1;
                }
                
                middle_value >>= 1;
            }
            break;
        }


        // FRQ
        case 'F':
        {
            middle_value = values[i][0];
            x_array[middle_value] =1;
            break;
        }      

        default:
            cerr << "Statistic name " << statistic_name << " not implemented";
            exit(1);
        }
        
        // 第一种传输方式，传输一个十进制的p域值
        if (communication_method == 'S')
        {
            middle_y = 0;
            y = 0;        // 十进制Y，明文
            tmp_pow = 1;
            // cout << "y" <<endl;
            for (int j = 0; j < number_bits; j++)
            {

                y += (r_array[number_bits-1-j] * x_array[number_bits -1 -j] + 1) / 2;
                y *= 2;
            }
            y = y/2;
            y.pack(os);
        }
        
        // 第二种传输方式，传输一串二进制的值(这里还是p域值，一般是-1或1)
        else if (communication_method == 'M')
        {
            for (int j = 0; j < number_bits; j++)
            {
                y = r_array[j] * x_array[j];
                y.pack(os);
                // std::cout << "Test y" << j << " = " << y << endl;
            }
        }

        else
            throw runtime_error("unexpected communication method");

    }
    for (auto& socket : sockets)
        os.Send(socket);
    end = clock();
    // gfp test=0;                                                       //结束时间 /////////////////////
    
    
    
    cout<<"client running time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;   //输出时间（单位：ｓ）//////////
}





////////////////////////统计量：LR ///////////////////////

template<class T>
void Client::send_private_inputs_LR(const vector< vector<T> >& values)
{
    // （x1 x2 ... xk）  xk = c0 + c1 * x1 + ... + c_{k-1} * x_{k-1}
    // 一个客户端持有数据是一个长度为number_variants (k)的向量，需要客户端传递一个triple（此处方案是可以修改的，比如改成需要number_variants个triple）
    int num_inputs = values.size();       // num_inputs = number_imputs   
    
    if (num_inputs != number_inputs)
        throw runtime_error("num_inputs != number_inputs");
    
    octetStream os;
    vector< vector<gfp> > triples(num_inputs, vector<gfp>(4));
    vector<gfp> triple_shares(4);
    bool active = true;

    // Receive num_inputs triples from SPDZ
    for (size_t j = 0; j < sockets.size(); j++)
    {
#ifdef VERBOSE_COMM
        cerr << "receiving from " << j << endl << flush;
#endif

        os.reset_write_head();
        os.Receive(sockets[j]);

#ifdef VERBOSE_COMM
        cerr << "received " << os.get_length() << " from " << j << endl << flush;
#endif

        if (j == 0)
        {
            if (os.get_length() == 4 * values.size() * gfp::size())
                active = true;
            else
                active = false;
            // std::cout << "active = " << active << endl;
        }

        int n_expected = active ? 4 : 1;
        if (os.get_length() != n_expected * gfp::size() * values.size())
            throw runtime_error("unexpected data length in sending");

        for (int j = 0; j < num_inputs; j++)
        {
            for (int k = 0; k < n_expected; k++)
            {
                triple_shares[k].unpack(os);
                triples[j][k] += triple_shares[k];
            }
        }

    }

    clock_t start,end;     // 定义clock_t变量 /////////////////////////////////////
    start = clock();       // 开始时间        /////////////////////////////////////

    if (active)
        for (int i = 0; i < num_inputs; i++)
        {
            if (triples[i][0] * triples[i][0] != triples[i][1])
            {
                cerr << triples[i][2] << " != " << triples[i][0] << " * " << triples[i][1] << endl;
                cerr << "Incorrect triple at " << i << ", aborting\n";
                throw mac_fail();
            }
            if (triples[i][0] * triples[i][2] != triples[i][3])
            {
                throw mac_fail();
            }
        }


    // Send inputs + triple[0], so SPDZ can compute shares of each value
    

    os.reset_write_head();
    gfp tmp_value,tmp_value2, y;
    int mask_bit;
    for (int i = 0; i < num_inputs; i++)
    {
        // r_array
        vector<T> r_array(number_bits);
        tmp_value = triples[i][2];
        mask_bit = 1<<0;
        
        for (int b = 0; b < number_bits; b++)
        {
            if (b<31)
            {
                tmp_value2 = tmp_value;
                tmp_value2 &= mask_bit;
                r_array[b] = (tmp_value2 == mask_bit)*2-1;
                mask_bit = mask_bit << 1;
            }
            else
            {
                tmp_value2 = tmp_value;
                tmp_value2 = tmp_value2 >>b;
                tmp_value2 &= 1;
                r_array[b] = (tmp_value2 == 1)*2-1;
            }
        }
        

        // x_array
        T middle_value, middle_value2, tmp_pow;
        vector<T> x_array(number_bits);
        for (int j = 0; j < number_variants; j++)
        {

            middle_value = values[i][j];
            for (int b = j * number_bits / number_variants; b < (j + 1) * number_bits / number_variants; b++)
            {
                middle_value2 = middle_value;
                middle_value2 &= 1;
                if (middle_value2 == 1)
                {
                    x_array[b] = 1;
                }
                else
                {
                    x_array[b] = -1;
                }
                
                middle_value >>= 1;
            }   

        }

        // 计算y_array并发送给服务器

        // 第一种传输方式，传输一个十进制的p域值
        if (communication_method == 'S')
        {
            y = 0;        // 十进制Y，明文
            tmp_pow = 1;
            for (int j = 0; j < number_bits; j++)
            {
                y += (r_array[number_bits-1-j] * x_array[number_bits -1 -j] + 1) / 2;
                y *= 2;
            }
            // std::cout << "Test Y=" << Y << endl;
            y = y / 2;
            y.pack(os);
        }

        // 第二种传输方式，传输一串二进制的值(这里还是p域值，一般是-1或1)
        else if (communication_method == 'M')
        {
            for (int j = 0; j < number_bits; j++)
            {
                y = r_array[j] * x_array[j];
                y.pack(os);
                // std::cout << "Test y" << j << " = " << y << endl;
            }
        }

        else
            throw runtime_error("unexpected communication method");

    }
    for (auto& socket : sockets)
        os.Send(socket);
    
    end = clock();                                                                // 结束时间 /////////////////////
    cout<<"client running time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  // 输出时间（单位：ｓ）//////////
    
}




////////////////////////统计量：MAX ///////////////////////
// 几乎没有要优化的地方
template<class T>
void Client::send_private_inputs_MAX(const vector< vector<T> >& values)
{
    int num_inputs = values.size();   // num_inputs = number_inputs
    if (num_inputs != number_inputs)
        throw runtime_error("num_inputs != number_inputs");
    octetStream os;
    // 一个客户端私有值需要传给服务器 number_bits个p域数，也就需要number_bits个triple
    vector< vector<gfp> > triples(number_inputs * number_bits, vector<gfp>(3));   
    vector<gfp> triple_shares(3);
    bool active = true;

    // Receive num_inputs triples from SPDZ
    for (size_t j = 0; j < sockets.size(); j++)
    {
#ifdef VERBOSE_COMM
        cerr << "receiving from " << j << endl << flush;
#endif

        os.reset_write_head();
        os.Receive(sockets[j]);

#ifdef VERBOSE_COMM
        cerr << "received " << os.get_length() << " from " << j << endl << flush;
#endif

        if (j == 0)
        {
            if (int(os.get_length()) == 3 * number_inputs * number_bits * gfp::size())
                active = true;
            else
                active = false;
        }

        int n_expected = active ? 3 : 1;
        if (int(os.get_length()) != n_expected * gfp::size() * number_inputs * number_bits)
            throw runtime_error("unexpected data length in sending");

        for (int j = 0; j < number_inputs * number_bits; j++)
        {
            for (int k = 0; k < n_expected; k++)
            {
                triple_shares[k].unpack(os);
                triples[j][k] += triple_shares[k];
            }
        }
    }

    clock_t start,end;    // 定义clock_t变量 /////////////////////////////////////
    start = clock();      // 开始时间        /////////////////////////////////////

    if (active)
        // Check triple relations (is a party cheating?)
        for (int i = 0; i < number_inputs * number_bits; i++)
        {
            if (triples[i][0] * triples[i][1] != triples[i][2])
            {
                cerr << triples[i][2] << " != " << triples[i][0] << " * " << triples[i][1] << endl;
                cerr << "Incorrect triple at " << i << ", aborting\n";
                throw mac_fail();
            }
        }

    // Send inputs + triple[0], so SPDZ can compute shares of each value
    os.reset_write_head();
    // T tmp_value;
    gfp y;
    for (int i = 0; i < number_inputs; i++)
    {
        //tmp_value = values[i][0];
        srand((unsigned)time(NULL));   // 防止程序每次运行产生的随机数都相同
        for (int j = 0; j <= values[i][0]; j++)
        {
            
            // std::cout << "The" << i << "th element of x_array is" << x_array[i] << endl;
            y = triples[i * number_bits + j][0] + rand();
            y.pack(os);
        }

        for (int j = values[i][0] + 1; j < number_bits; j++)
        {

            // std::cout << "The" << i << "th element of x_array is" << x_array[i] << endl;
            y = triples[i * number_bits + j][0];
            y.pack(os);
        }
    }

    for (auto& socket : sockets)
        os.Send(socket);
    
    end = clock();                                                                // 结束时间 /////////////////////
    cout<<"client running time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  // 输出时间（单位：ｓ）//////////

}




////////////////////////统计量：AND ///////////////////////
template<class T>
void Client::send_private_inputs_AND(const vector< vector<T> >& values)
{
    int num_inputs = values.size();   // num_inputs = number_inputs
    if (num_inputs != number_inputs)
        throw runtime_error("num_inputs != number_inputs");
    octetStream os;
    // 一个客户端私有值需要传给服务器 number_bits个p域数，也就需要number_bits个triple
    vector< vector<gfp> > triples(number_inputs * number_bits, vector<gfp>(3));   
    vector<gfp> triple_shares(3);
    bool active = true;

    // Receive num_inputs triples from SPDZ
    for (size_t j = 0; j < sockets.size(); j++)
    {
#ifdef VERBOSE_COMM
        cerr << "receiving from " << j << endl << flush;
#endif

        os.reset_write_head();
        os.Receive(sockets[j]);

#ifdef VERBOSE_COMM
        cerr << "received " << os.get_length() << " from " << j << endl << flush;
#endif

        if (j == 0)
        {
            if (int(os.get_length()) == 3 * number_inputs * number_bits * gfp::size())
                active = true;
            else
                active = false;
        }

        int n_expected = active ? 3 : 1;
        if (int(os.get_length()) != n_expected * gfp::size() * number_inputs * number_bits)
            throw runtime_error("unexpected data length in sending");

        for (int j = 0; j < number_inputs * number_bits; j++)
        {
            for (int k = 0; k < n_expected; k++)
            {
                triple_shares[k].unpack(os);
                triples[j][k] += triple_shares[k];
            }
        }
    }

    clock_t start,end;    // 定义clock_t变量 /////////////////////////////////////
    start = clock();      // 开始时间        /////////////////////////////////////

    if (active)
        // Check triple relations (is a party cheating?)
        for (int i = 0; i < number_inputs * number_bits; i++)
        {
            if (triples[i][0] * triples[i][1] != triples[i][2])
            {
                cerr << triples[i][2] << " != " << triples[i][0] << " * " << triples[i][1] << endl;
                cerr << "Incorrect triple at " << i << ", aborting\n";
                throw mac_fail();
            }
        }

    // Send inputs + triple[0], so SPDZ can compute shares of each value
    os.reset_write_head();
    // T tmp_value;
    gfp y;
    for (int i = 0; i < number_inputs; i++)
    {
        //tmp_value = values[i][0];
        srand((unsigned)time(NULL));   // 防止程序每次运行产生的随机数都相同
        if (values[i][0] == 0) 
        {
            y = triples[i][0] + rand();
            y.pack(os);
        }
        else
        {
            // std::cout << "The" << i << "th element of x_array is" << x_array[i] << endl;
            y = triples[i][0];
            y.pack(os);
        }
    }

    for (auto& socket : sockets)
        os.Send(socket);
    
    end = clock();                                                                // 结束时间 /////////////////////
    cout<<"client running time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  // 输出时间（单位：ｓ）//////////

}