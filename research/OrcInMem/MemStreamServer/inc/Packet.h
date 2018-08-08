/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Packet.h
 * Author: can.gursu
 *
 * Created on 07 August 2018, 09:54
 */

#ifndef PACKET_H
#define PACKET_H

class Packet
{
public:
    Packet();
    Packet(const Packet&);
    Packet(Packet&&);
    virtual ~Packet();
private:

};

#endif /* PACKET_H */

