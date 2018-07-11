/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: can.gursu@medyasoft.com.tr
 *
 * Created on 13 June 2018, 13:38
 */

#include <cstdlib>
#include <cstdint>
#include <ostream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

/*
 *
 */

 void PrintUsage()
 {
     std::cout << "Usage : \n";
     std::cout << "GenTestData FileNamePrefix lineCount\n";
     std::cout << "ex : $ GenTestData TestData 10000";
 }

template<typename T>
std::string tostring(const T &n)
{
    std::string s =  std::to_string(n);

    int dotpos = s.find_first_of('.');
    if(dotpos != std::string::npos)
    {
        int ipos = s.size()-1;
        while ((s[ipos] == '0') && (ipos > dotpos))
            --ipos;
        if (s[ipos] == '.')
            --ipos;
        s.erase (ipos + 1, std::string::npos);
    }
    return s;
}


int main(int argc, char** argv)
{
    const char *fname  = "TestData";
    const char *ext    = "csv";
    uint64_t    lcount = 10000;

    std::cout << "GenTestData v:0.0.2 - Test Data generator : \n";
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
        std::string valNum;
        std::string valDouble;
        std::string valText;



        for (l = 0; l < lcount; ++l)
        {
            valNum    = std::to_string(l);
            valText   = std::string("cln-") + std::to_string(l);

            valDouble = tostring<double>(double (l/10.0));

            ss.str(strEmpty);
            ss.clear();
            ss << valNum << "," << valText << "," << valDouble << std::endl;
            str = ss.str();
            fsIn.write(str.c_str(), str.length());

            ss.str(strEmpty);
            ss.clear();
            ss << "{\"col1\": " << valNum << ", \"col2\": \"" << valText << "\", \"col3\": " << valDouble << "}";
            if (l+1 < lcount) ss << std::endl;
            str = ss.str();
            fsOut.write(str.c_str(), str.length());
        }
    }
    return 0;
}

