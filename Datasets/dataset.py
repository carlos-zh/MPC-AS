import numpy as np
import os

"""
number_clients: The number of clients; number_values: The number of values for each client.

number_bits: The Max value of clients' inputs
    SUM : 2**2  2**4  2**8  2**16
    VAR : 2**2  2**4  2**8  2**16
    MAX : 10 20 30 40
    FRQ : 10 20 30 40
    LR : 2**8

"""   


def DataGeneration(statistic_name, number_clients, number_values, number_max, isValid):
    if not os.path.exists(data_path):
        os.makedirs(data_path)
    if (statistic_name == "L"):
        data_file = os.path.join(data_path, statistic_name + "_" + str(number_values) + "_" + isValid + ".txt")
    else:
        data_file = os.path.join(data_path, statistic_name + "_" + str(number_max) + "_" + isValid + ".txt")

    # generate data
    # data = np.ones((number_clients, number_values), dtype=np.int8)
    np.random.seed(42)
    data = np.random.randint(0, number_max, size=(number_clients, number_values), dtype=np.int32)
    # save data
    np.savetxt(data_file, data, fmt='%i', delimiter=' ')

    return data


if __name__ == '__main__':
    data_path = "./"
    statistic_name = "F"           # 
    number_clients = 100000        # 
    number_values = 1              # 
    number_max = 10
    isValid = "Y"                  #  Y: valid N: invalid


    statistic_name_list = ["M", "F"]
    number_max_list = [10, 20, 30, 40]
    for j in range(len(statistic_name_list)):
        for i in range(len(number_max_list)):
            DataGeneration(statistic_name_list[j], number_clients, number_values, number_max_list[i], isValid)

    statistic_name_list = ["S", "V"]
    number_max_list = [2**2, 2**4, 2**8, 2**16]
    for j in range(len(statistic_name_list)):
        for i in range(len(number_max_list)):
            DataGeneration(statistic_name_list[j], number_clients, number_values, number_max_list[i], isValid)

    statistic_name = "L"
    number_values_list = [2, 3, 4, 5]
    number_max = 2**8
    for i in range(len(number_values_list)):
        DataGeneration("L", number_clients, number_values_list[i], number_max, isValid)