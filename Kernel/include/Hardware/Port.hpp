#pragma once

#include <Lib/Types.hpp>

class Port {
protected:
    u16 portNumber;

    explicit Port(u16 portNumber);
    ~Port() = default;
};

class Port8Bit : public Port {
public:
    explicit Port8Bit(u16 portNumber);

    virtual void write(u8 data);
    virtual u8 read();
};

class Port8BitSlow : public Port8Bit {
public:
    explicit Port8BitSlow(u16 portNumber);

    void write(u8 data) override;
    u8 read() override;
};

class Port16Bit : public Port {
public:
    explicit Port16Bit(u16 portNumber);

    void write(u16 data);
    u16 read();
};

class Port32Bit : public Port {
public:
    explicit Port32Bit(u16 portNumber);

    void write(u32 data);
    u32 read();
};
