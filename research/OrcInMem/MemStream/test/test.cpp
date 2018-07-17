


#include "MemStream.h"
#include "StreamPacket.h"

#include <iostream>


bool CheckBufferWrite(MemStream<std::uint8_t> *m, const char *data)
{
    size_t len = std::strlen(data);
    m->write(data, len);
    std::cout << "Written : " << data << " - " << "Len:" << m->Len() << ", Size:" << m->Size() << ", Count:" << m->Cnt() << std::endl;

    char buff[len + 1] = "";
    m->read(buff, len, m->Len() - len);
    std::cout << "Readed : " << std::string(buff, len) << std::endl;

    return std::strncmp(buff, data, len) == 0;
}

void Test_MemStream()
{
    std::cout << "MemStream Test Suit\n";
    int resTest = true;
    int isPass  = true;

    MemStream<std::uint8_t> m;

    const char *data[] =
    {
        "",
        "1234567890abcdefghi",
        "jkl",
        "mn",
        "opqr",
        nullptr
    };

    for (const char **d = data; *d; ++d)
    {
        std::cout << "Adding data : " << *d << std::endl;
        resTest = CheckBufferWrite(&m, *d);
        isPass &= resTest;
    }


    std::cout << "******* Test " << (isPass ? "SUCCEED" : "FAILED") << " *******\n";
}





void Test_StreamPacket()
{
    std::cout << "StreamPacket Test Suit\n";

    StreamPacket pck;
    const char *data = "denemem";
    pck.Create(data, strlen(data) * sizeof(char));
    pck.Check();

    data = "kenemem";
    pck.Create(data, strlen(data) * sizeof(char));
    pck.Check();
}


int main(int argc, char** argv)
{
    Test_StreamPacket();
}


