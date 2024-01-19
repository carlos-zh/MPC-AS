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
void run(vector< vector<double> > salary_value, Client& client)
{
    // sint
    vector< vector<int> > values_SUM(number_inputs, vector<int>(number_variants));
    vector< vector<int> > values_M(number_inputs, vector<int>(number_variants));
    vector< vector<int> > values_F(number_inputs, vector<int>(number_variants));
    vector< vector<int> > values_L(number_inputs, vector<int>(number_variants));
    vector< vector<int> > values_A(number_inputs, vector<int>(number_variants));
    vector< vector<int> > values_V(number_inputs, vector<int>(number_variants));

    switch(statistic_name)
    {
    case 'S':
       
       for (int i = 0; i < number_inputs; i++)
           for (int j = 0; j < number_variants; j++)
               values_SUM[i][j] = int(salary_value[i][j]);

       client.send_private_inputs<int>(values_SUM);        // SUM
       break;
    case 'M':
       
       for (int i = 0; i < number_inputs; i++)
           for (int j = 0; j < number_variants; j++)
               values_M[i][j] = int(salary_value[i][j]);
       client.send_private_inputs_MAX<int>(values_M);    // MAX
       break;
    case 'F':
       
       for (int i = 0; i < number_inputs; i++)
           for (int j = 0; j < number_variants; j++)
               values_F[i][j] = int(salary_value[i][j]);
       if (number_bits <32)
       {
            client.send_private_inputs<int>(values_F);
       }
       else{
            client.send_private_inputs_longint<int>(values_F);
       }
      // FRQ
       break;
    case 'L':
       
       for (int i = 0; i < number_inputs; i++)
           for (int j = 0; j < number_variants; j++)
               values_L[i][j] = int(salary_value[i][j]);
       client.send_private_inputs_LR<int>(values_L);     // LR
       break;
    case 'A':

        for (int i = 0; i < number_inputs; i++)
            for (int j = 0; j < number_variants; j++)
                values_A[i][j] = int(salary_value[i][j]);
        values_A[0][0] = 0;
        client.send_private_inputs_AND<int>(values_A);    
        break;
    case 'V':

        for (int i = 0; i < number_inputs; i++)
            for (int j = 0; j < number_variants; j++)
                values_V[i][j] = int(salary_value[i][j]);
        values_V[0][0] = 2;
        client.send_private_inputs<int>(values_V);    
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
    vector< vector<double> > salary_value(number_inputs, vector<double>(number_variants));

    if (argc < 4) {
        cout << "Usage is bankers-bonus-client <client identifier> <number of spdz parties> "
           << "<salary to compare> <finish (0 false, 1 true)> <optional host names..., default localhost> "
           << "<optional spdz party port base number, default 14000>" << endl;
        exit(0);
    }

    my_client_id = atoi(argv[1]);
    nparties = atoi(argv[2]);
    finish = atoi(argv[3]);

    for (int i = 0; i < number_inputs; i++)
    {
        for (int j = 0; j < number_variants; j++)
        {
            if (statistic_name == 'L')
                salary_value[i][j] = j;
            else
                salary_value[i][j] = 1;
        }
    }

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