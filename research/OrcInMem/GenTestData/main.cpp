/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: can.gursu
 *
 * Created on 13 June 2018, 13:38
 */

#include <cstdlib>
#include <cstdint>
#include <ostream>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

/*
 *
 */

 void PrintUsage()
 {
     cout << "Usage : \n";
     cout << "GenTestData FileNamePrefix lineCount\n";
     cout << "ex : $ GenTestData TestData 10000";
 }

int main(int argc, char** argv)
{
    const char *fname  = "TestData";
    const char *ext    = "csv";
    uint64_t    lcount = 10000;

    std::cout << "GenTestData v:0.0 - Test Data generator";

    std::cout << argc << " : " << argv[0] << std::endl;
    if (argc > 1) fname = argv[1];
    std::cout << " File : " << fname << std::endl;

    uint64_t l = 0;
    if (argc > 2 && (l = atoi(argv[2]))) lcount = l;
    std::cout << " Line : " << lcount << std::endl;

    std::string       str;
    const std::string strEmpty;
    std::stringstream ss;
    std::ofstream     fsIn;
    std::ofstream     fsOut;


    std::string fnameIn = fname;
    fnameIn += ".in";
    fsIn.open(fnameIn.c_str());


    std::string fnameOut = fname;
    fnameOut += ".out";
    fsOut.open(fnameOut.c_str());

    if (!fsIn.good())
    {
        std::cout << "Unable to open file : " << fnameIn << std::endl;
    }
    else if (!fsOut.good())
    {
        std::cout << "Unable to open file : " << fnameOut << std::endl;
    }
    else
    {
        for (l = 0; l < lcount; ++l)
        {
            ss.str(strEmpty);
            ss.clear();
            ss << l << "," << "col" << l << "," << double (l/10.0) << std::endl;
            str = ss.str();
            fsIn.write(str.c_str(), str.length());

            ss.str(strEmpty);
            ss.clear();
            ss << "{\"col1\": " << l << ", \"col2\": " << "col" << l << ", \"col3\": " << double (l/10.0) << "}" << std::endl;
            str = ss.str();
            fsOut.write(str.c_str(), str.length());
        }
    }
    return 0;
}

