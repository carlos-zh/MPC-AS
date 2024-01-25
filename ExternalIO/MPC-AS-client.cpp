#include "Math/gfp.h"
#include "Math/gf2n.h"
#include "Networking/sockets.h"
#include "Networking/ssl_sockets.h"
#include "Tools/int.h"
#include "Math/Setup.h"
#include "Protocols/fake-stuff.h"

#include "Math/gfp.hpp"
#include "Client_MPC-AS.hpp"

#include <sodium.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include <cmath>


template<class T, class U>
void run(vector< vector<int> > salary_value, Client& client)
{
    switch(statistic_name)
    {
    case 'S':
       client.send_private_inputs<int>(salary_value);
       break;
    case 'M':
       client.send_private_inputs_MAX<int>(salary_value);
       break;
    case 'm':
       client.send_private_inputs_MIN<int>(salary_value);
       break;
    case 'F':
       if (number_bits <32) client.send_private_inputs<int>(salary_value);
       else client.send_private_inputs_longint<int>(salary_value);
       break;
    case 'L':
       client.send_private_inputs_LR<int>(salary_value);
       break;
    case 'A':
        client.send_private_inputs_AND<int>(salary_value);    
        break;
    case 'V':
        client.send_private_inputs<int>(salary_value);    
        break;
    default:
       break;
    }

    cout << "Sent inputs to each SPDZ engine, waiting for result..." << endl;

}


int main(int argc, char** argv)
{
    int my_client_id;
    int nparties;
    int finish;
    int port_base = 14000;
    vector< vector<int> > salary_value(number_inputs, vector<int>(number_variants));

    if (argc < 4) {
        cout << "Usage is bankers-bonus-client <client identifier> <number of spdz parties> "
           << "<salary to compare> <finish (0 false, 1 true)> <optional host names..., default localhost> "
           << "<optional spdz party port base number, default 14000>" << endl;
        exit(0);
    }

    my_client_id = atoi(argv[1]);
    nparties = atoi(argv[2]);
    finish = atoi(argv[3]);


    // dataset input
    string file_path = "./Datasets/";
    string file_name;

    if (statistic_name == 'S' || statistic_name == 'V') 
        file_name = file_path + statistic_name + "_" + to_string(int(pow(2, number_bits))) + "_" + isValid + ".txt";
    else if (statistic_name == 'M' || statistic_name == 'F')
        file_name = file_path + statistic_name + "_" + to_string(number_bits) + "_" + isValid + ".txt";
    else
        file_name = file_path + statistic_name + "_" + to_string(number_variants) + "_" + isValid + ".txt";
    
    cout << "Client inputs read from " << file_name << endl;
    std::ifstream ifs(file_name);
    std::string line;

    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to read " + file_name);
    }
    // std::cout << file_name << endl;

    for (int i = 0; i < number_inputs; i++){
        std::getline(ifs, line);
        std::istringstream ls(line);
        std::string s;
        for (int j = 0; j < number_variants; j++) {
            ls >> s;
            salary_value[i][j] = std::stoi(s);
        }
    }

    // // dataset input (test)
    // for (int i = 0; i < number_inputs; i++)
    // {
    //     for (int j = 0; j < number_variants; j++)
    //     {
    //         if (statistic_name == 'L')        
    //             salary_value[i][j] = j;
    //         else
    //             salary_value[i][j] = 1;
    //     }
    // }
    // // salary_value[0][0] = 0;


    vector<string> hostnames(nparties, "localhost");
    if (argc > 4)
    {
        if (argc < 4 + nparties)
        {
            cerr << "Not enough hostnames specified";
            exit(1);
        }

        for (int i = 0; i < nparties; i++)
            hostnames[i] = argv[4 + i];
    }

    if (argc > 4 + nparties)
        port_base = atoi(argv[4 + nparties]);

    bigint::init_thread();

    // Setup connections from this client to each party socket
    Client client(hostnames, port_base, my_client_id);
    auto& specification = client.specification;
    auto& sockets = client.sockets;
    for (int i = 0; i < nparties; i++)
    {
        octetStream os;
        os.store(finish);
        os.Send(sockets[i]);
    }
    cout << "Finish setup socket connections to SPDZ engines." << endl;

    int type = specification.get<int>();
    switch (type)
    {
    case 'p':
    {
        gfp::init_field(specification.get<bigint>());
        cerr << "using prime " << gfp::pr() << endl;
        run<gfp, gfp>(salary_value, client);
        break;
    }

    case 'R':
    {
        int R = specification.get<int>();
        switch (R)
        {
        case 64:
            run<Z2<64>, Z2<64>>(salary_value, client);
            break;
        case 104:
            run<Z2<104>, Z2<64>>(salary_value, client);
            break;
        case 128:
            run<Z2<128>, Z2<64>>(salary_value, client);
            break;
        default:
            cerr << R << "-bit ring not implemented";
            exit(1);
        }
        break;
    }
    default:
        cerr << "Type " << type << " not implemented";
        exit(1);
    }

    return 0;

}