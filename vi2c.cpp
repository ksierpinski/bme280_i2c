#include <iostream>
#include <functional>

/*
class Device
{
    int fd;

    uint8_t read(uint8_t reg) const
    {
    //    return wiringPiI2CReadReg8(fd,reg);

        return 0xFF;
    }

    void write(uint8_t reg, uint8_t data) const
    {
       //wiringPiI2CWriteReg8(fd, reg, data);
    }


public:
    explicit Device(int id)
    {
       //fd = wiringPiI2CSetup(id);

        fd = id;
    }

    int get(int address, int mask)
    {
        return read(address) & mask;
    }

    void set(int value, int address, int mask)
    {
        // TODO: waht if mask = 0x00 ?
        if (mask != 0xFF)
        {
            uint8_t cache = get(address, 0xFF) & ~mask;
            value &= mask;
            value |= cache;
        }

        write(address, value);
    }
};
*/

enum class RegisterAttribute
{
    do_not_use,
    read,
    write,
    read_write
};

namespace magic
{
int get(int fd, int address, int mask)
{
   // return read(address) & mask;
    return fd + address + mask;
}

void set(int fd, int value, int address, int mask)
{
    // TODO: waht if mask = 0x00 ?
    if (mask != 0xFF)
    {
        uint8_t cache = get(fd, address, 0xFF) & ~mask;
        value &= mask;
        value |= cache;
    }

//    write(address, value);
}
};

constexpr int get_shift(int bitmask)
{
    return bitmask & 0x01 ? 0 : get_shift(bitmask >> 1) + 1;
}
/*
struct Device
{
     Device(int fd_) :
        get([fd_](int address) {return magic::get(fd_, address, 0xFF);} ),
        set([fd_] (int value, int address) {magic::set(fd_, value, address, 0xFF);} )
    {}

    std::function<int(int)> get;
    std::function<void(int,int)> set;
};
*/
struct Register
{
    const int address;
    const RegisterAttribute attribute;
    Register(int address_, RegisterAttribute attribute_) : address(address_), attribute(attribute_)
    {}
};

struct SubRegister
{
    const int mask;
    SubRegister(int mask_) : mask(mask_) {}

    SubRegister operator|(const SubRegister &other)
    {
        return { mask | other.mask};
    }
};

namespace BME280
{
    constexpr static auto address = 0x77;

    struct hum_lsb_ : public Register
    {
        constexpr static auto address = 0xFE;
        hum_lsb_() : Register(address, RegisterAttribute::read_write){}
    } hum_lsb;

    struct hum_msb_ : public Register
    {
        constexpr static auto address = 0xFD;
        hum_msb_() : Register(address, RegisterAttribute::read_write){}
    } hum_msb;

    struct temp_xlsb_: public Register
    {
        constexpr static auto address = 0xFC;
        temp_xlsb_() : Register(address, RegisterAttribute::read_write){}
    } temp_xlsb;

    struct temp_lsb_: public Register
    {
        constexpr static auto address = 0xFB;
        temp_lsb_() : Register(address, RegisterAttribute::read_write){}
    } temp_lsb;

    struct temp_msb_: public Register
    {
        constexpr static auto address = 0xFA;
        temp_msb_() : Register(address, RegisterAttribute::read_write){}
    } temp_msb;

    struct press_xlsb_: public Register
    {
        constexpr static auto address = 0xF9;
        press_xlsb_() : Register(address, RegisterAttribute::read_write){}
    } press_xlsb;

    struct press_lsb_: public Register
    {
        constexpr static auto address = 0xF8;
        press_lsb_() : Register(address, RegisterAttribute::read_write){}
    } press_lsb;

    struct press_msb_: public Register
    {
        constexpr static auto address = 0xF7;
        press_msb_() : Register(address, RegisterAttribute::read_write){}
    } press_msb;

    struct config_: public Register
    {
        constexpr static auto address = 0xF5;
        config_() : Register(address, RegisterAttribute::read){}

        struct t_sb_ : public SubRegister
        {
            constexpr static auto mask = 0b11100000;
            t_sb_() : SubRegister(mask) {}

            enum
            {
                standbay_0_5ms  = 0b000,
                standbay_62_5ms = 0b001 << get_shift(mask),
                standbay_125ms  = 0b010 << get_shift(mask),
                standbay_250ms  = 0b011 << get_shift(mask),
                standbay_500ms  = 0b100 << get_shift(mask),
                standbay_1000ms = 0b101 << get_shift(mask),
                standbay_10ms   = 0b110 << get_shift(mask),
                standbay_20ms   = 0b100 << get_shift(mask)
            };
        } t_sb;

        struct filter_ : public SubRegister
        {
            constexpr static auto mask = 0b00011100;
            filter_() : SubRegister(mask) {}

            enum
            {
                off            = 0b000,
                coefficient_2  = 0b001 << get_shift(mask),
                coefficient_4  = 0b010 << get_shift(mask),
                coefficient_8  = 0b011 << get_shift(mask),
                coefficient_16 = 0b100 << get_shift(mask)
            };
        } filter;

        struct reserved_ : public SubRegister
        {
            constexpr static auto mask = 0b00000010;
            reserved_() : SubRegister(mask) {}

        } reserved;

        struct spi3w_en_ : public SubRegister
        {
            constexpr static auto mask = 0b00000001;
            spi3w_en_() : SubRegister(mask) {}

            enum
            {
                off = 0b0,
                on  = 0b1,
            };
        } spi3w_en;
    } config;

    struct ctrl_meas_ : public Register
    {
        constexpr static auto address = 0xF4;
        ctrl_meas_() : Register(address, RegisterAttribute::read_write) {}

        struct osrs_t_ : public SubRegister
        {
            constexpr static auto mask = 0b11100000;
            osrs_t_() : SubRegister(mask) {}

            enum
            {
                skipped           = 0b000,
                oversampling_x_1  = 0b001 << get_shift(mask),
                oversampling_x_2  = 0b010 << get_shift(mask),
                oversampling_x_4  = 0b011 << get_shift(mask),
                oversampling_x_8  = 0b100 << get_shift(mask),
                oversampling_x_16 = 0b101 << get_shift(mask)
            };
        } osrs_t;

        struct osrs_p : public SubRegister
        {
            constexpr static auto mask = 0b00011100;
            osrs_p() : SubRegister(mask) {}

            enum
            {
                skipped           = 0b000,
                oversampling_x_1  = 0b001 << get_shift(mask),
                oversampling_x_2  = 0b010 << get_shift(mask),
                oversampling_x_4  = 0b011 << get_shift(mask),
                oversampling_x_8  = 0b100 << get_shift(mask),
                oversampling_x_16 = 0b101 << get_shift(mask)
            };
        } osrs_p;

        struct mode_ : public SubRegister
        {
            constexpr static auto mask = 0b00000011;
            mode_() : SubRegister(mask) {}

            enum
            {
                sleep    = 0b00,
                forced   = 0b01 << get_shift(mask),
                forced_2 = 0b10 << get_shift(mask),
                normal   = 0b11 << get_shift(mask)
            };
        } mode;
    } ctrl_meas;

    struct status_ : public Register
    {
        constexpr static auto address = 0xF3;
        status_() : Register(address, RegisterAttribute::read) {}


        struct measuring_ : public SubRegister
        {
            constexpr static auto mask = 0b1000;
            measuring_() : SubRegister(mask) {}

            enum
            {
                done     = 0,
                working  = 1
            };
        } measuring;

        struct im_update_ : public SubRegister
        {
            constexpr static auto mask = 0b0001;
            im_update_() : SubRegister(mask) {}

            enum
            {
                done    = 0,
                working = 1
            };
        } im_update;
    } status;

    struct ctrl_hum_ : public Register
    {
        constexpr static auto address = 0xF2;
        ctrl_hum_() : Register(address, RegisterAttribute::do_not_use) {}

        struct osrs_h_ : public SubRegister
        {
            constexpr static auto mask = 0b111;
            osrs_h_() : SubRegister(mask) {}

            enum
            {
                skipped           = 0b000,
                oversampling_x_1  = 0b001 << get_shift(mask),
                oversampling_x_2  = 0b010 << get_shift(mask),
                oversampling_x_4  = 0b011 << get_shift(mask),
                oversampling_x_8  = 0b100 << get_shift(mask),
                oversampling_x_16 = 0b101 << get_shift(mask)
            };
        } osrs_h;
    } ctrl_hum;

    struct reset_ : public Register
    {
        constexpr static auto address = 0xE0;
        reset_() : Register(address, RegisterAttribute::write) {}

        enum
        {
            reset_code = 0xB6
        };
    } reset;

    struct id_: public Register
    {
        constexpr static auto address = 0xD0;
        id_() : Register(address, RegisterAttribute::read) {}
    } id;


    /* Calibration data */
    struct dig_T1_7_0_ : public Register
    {
        constexpr static auto address = 0x88;
        dig_T1_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_T1_7_0;
    struct dig_T1_15_8_: public Register
    {
        constexpr static auto address = 0x89;
        dig_T1_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_T1_15_8;

    struct dig_T2_7_0_ : public Register
    {
        constexpr static auto address = 0x8A;
        dig_T2_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_T2_7_0;
    struct dig_T2_15_8_ : public Register
    {
        constexpr static auto address = 0x8B;
        dig_T2_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_T2_15_8;

    struct dig_T3_7_0_ : public Register
    {
        constexpr static auto address = 0x8C;
        dig_T3_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_T3_7_0;
    struct dig_T3_15_8_ : public Register
    {
        constexpr static auto address = 0x8D;
        dig_T3_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_T3_15_8;

    /* Preasure */
    struct dig_P1_7_0_ : public Register
    {
        constexpr static auto address = 0x8E;
        dig_P1_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P1_7_0;
    struct dig_P1_15_8_ : public Register
    {
        constexpr static auto address = 0x8F;
        dig_P1_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P1_15_8;

    struct dig_P2_7_0_ : public Register
    {
        constexpr static auto address = 0x90;
        dig_P2_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P2_7_0;
    struct dig_P2_15_8_ : public Register
    {
        constexpr static auto address = 0x91;
        dig_P2_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P2_15_8;

    struct dig_P3_7_0_ : public Register
    {
        constexpr static auto address = 0x92;
        dig_P3_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P3_7_0;
    struct dig_P3_15_8_ : public Register
    {
        constexpr static auto address = 0x93;
        dig_P3_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P3_15_8;

    struct dig_P4_7_0_ : public Register
    {
        constexpr static auto address = 0x94;
        dig_P4_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P4_7_0;
    struct dig_P4_15_8_ : public Register
    {
        constexpr static auto address = 0x95;
        dig_P4_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P4_15_8;

    struct dig_P5_7_0_ : public Register
    {
        constexpr static auto address = 0x96;
        dig_P5_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P5_7_0;
    struct dig_P5_15_8_ : public Register
    {
        constexpr static auto address = 0x97;
        dig_P5_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P5_15_8;

    struct dig_P6_7_0_ : public Register
    {
        constexpr static auto address = 0x98;
        dig_P6_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P6_7_0;
    struct dig_P6_15_8_ : public Register
    {
        constexpr static auto address = 0x99;
        dig_P6_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P6_15_8;

    struct dig_P7_7_0_ : public Register
    {
        constexpr static auto address = 0x9A;
        dig_P7_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P7_7_0;
    struct dig_P7_15_8_ : public Register
    {
        constexpr static auto address = 0x9B;
        dig_P7_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P7_15_8;

    struct dig_P8_7_0_ : public Register
    {
        constexpr static auto address = 0x9C;
        dig_P8_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P8_7_0;
    struct dig_P8_15_8_ : public Register
    {
        constexpr static auto address = 0x9D;
        dig_P8_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P8_15_8;

    struct dig_P9_7_0_ : public Register
    {
        constexpr static auto address = 0x9E;
        dig_P9_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_P9_7_0;
    struct dig_P9_15_8_ : public Register
    {
        constexpr static auto address = 0x9F;
        dig_P9_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_P9_15_8;

    /* Humidity */
    struct dig_H1_ : public Register
    {
        constexpr static auto address = 0xA1;
        dig_H1_() : Register(address, RegisterAttribute::read) {}
    } dig_H1;

    struct dig_H2_7_0_ : public Register
    {
        constexpr static auto address = 0xE1;
        dig_H2_7_0_() : Register(address, RegisterAttribute::read) {}
    } dig_H2_7_0;
    struct dig_H2_15_8_ : public Register
    {
        constexpr static auto address = 0xE2;
        dig_H2_15_8_() : Register(address, RegisterAttribute::read) {}
    } dig_H2_15_8;

    struct dig_H3_ : public Register
    {
        constexpr static auto address = 0xE3;
        dig_H3_() : Register(address, RegisterAttribute::read) {}
    } dig_H3;

    struct dig_H4_11_4_ : public Register
    {
        constexpr static auto address = 0xE4;
        dig_H4_11_4_() : Register(address, RegisterAttribute::read) {}
    } dig_H4_11_4;

    struct dig_H4_H5_ : public Register
    {
        constexpr static auto address = 0xE5;
        dig_H4_H5_() : Register(address, RegisterAttribute::read) {}

        struct dig_H4_3_0_ : public SubRegister
        {
            constexpr static auto mask = 0b00001111;
            dig_H4_3_0_() : SubRegister(mask) {}
        } dig_H4_3_0;

        struct dig_H5_3_0_ : public SubRegister
        {
            constexpr static auto mask = 0b11110000;
            dig_H5_3_0_() : SubRegister(mask) {}
        } dig_H5_3_0;
    } dig_H4_H5;

    struct dig_H5_11_4_ : public Register
    {
        constexpr static auto address = 0xE6;
        dig_H5_11_4_() : Register(address, RegisterAttribute::read) {}
    } dig_H5_11_4;

    struct dig_H6_ : public Register
    {
        constexpr static auto address = 0xE7;
        dig_H6_() : Register(address, RegisterAttribute::read) {}
    } dig_H6;
}

int main()
{
    return 0;
}
