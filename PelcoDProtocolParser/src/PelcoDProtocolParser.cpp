#include "PelcoDProtocolParser.h"



pelco::PelcoDProtocolParser::PelcoDProtocolParser()
{

}



pelco::PelcoDProtocolParser::~PelcoDProtocolParser()
{
    
}



void pelco::PelcoDProtocolParser::GetChecksum(uint8_t* packet)
{
    uint32_t checksum = (uint32_t)packet[1] + (uint32_t)packet[2] + (uint32_t)packet[3] + (uint32_t)packet[4] + (uint32_t)packet[5];
    checksum %= 0x100;

    packet[6] = (uint8_t)checksum;
}


bool pelco::PelcoDProtocolParser::GetCommand(
            uint8_t* packet,
            uint8_t address,
            pelco::PelcoDCommands command_ID,
            uint8_t data_1,
            uint8_t data_2)
{
    // Check command.
    switch (command_ID)
    {
    case pelco::PelcoDCommands::CAMERA_ON:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x0A;
        packet[3] = 0x00;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;
    
    case pelco::PelcoDCommands::CAMERA_OFF:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x02;
        packet[3] = 0x00;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::IRIS_CLOSE:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x00;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::IRIS_OPEN:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x0A;
        packet[3] = 0x00;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::FOCUS_NEAR:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x09;
        packet[3] = 0x00;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::FOCUS_FAR:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x80;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::FOCUS_STOP:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x00;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ZOOM_WIDE:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x40;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ZOOM_TELE:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x20;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ZOOM_STOP:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x00;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::DOWN:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x10;
        packet[4] = 0x00;
        packet[5] = data_2;
        GetChecksum(packet);
        return true;
    
    case pelco::PelcoDCommands::UP:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x08;
        packet[4] = 0x00;
        packet[5] = data_2;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::LEFT:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x04;
        packet[4] = data_1;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;
    
    case pelco::PelcoDCommands::RIGHT:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x02;
        packet[4] = data_1;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::UP_RIGHT:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x0A;
        packet[4] = data_1;
        packet[5] = data_2;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::UP_LEFT:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x0C;
        packet[4] = data_1;
        packet[5] = data_2;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::DOWN_RIGHT:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x12;
        packet[4] = data_1;
        packet[5] = data_2;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::DOWN_LEFT:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x14;
        packet[4] = data_1;
        packet[5] = data_2;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::STOP:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x08;
        packet[3] = 0x00;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_PRESET:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x03;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::CLEAR_PRESET:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x05;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::GO_TO_PRESET:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x07;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::FLIP_180DEG_ABOUT:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x07;
        packet[4] = 0x00;
        packet[5] = 0x21;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::GO_TO_ZERO_PAN:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x07;
        packet[4] = 0x00;
        packet[5] = 0x22;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_AUXILIARY:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x09;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::CLEAR_AUXILIARY:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x0B;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::REMOTE_RESET:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x0F;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_ZONE_START:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x11;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_ZONE_END:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x13;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::WRITE_CHAR_TO_SCREEN:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x15;
        packet[4] = data_1;
        packet[5] = data_2;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::CLEAR_SCREEN:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x17;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ALARM_ACKNOWLEDGE:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x19;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ZONE_SCAN_ON:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x1B;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ZONE_SCAN_OFF:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x1D;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_PATTERN_START:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x1F;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_PATTERN_STOP:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x21;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::RUN_PATTERN:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x23;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_ZOOM_SPEED:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x25;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_FOCUS_SPEED:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x27;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::RESET_CAMERA_TO_DEFAULT:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x29;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::AUTO_FOCUS_AUTO_ON_OFF:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x2B;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::AUTO_IRIS_AUTO_ON_OFF:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x2D;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::AGC_AUTO_ON_OFF:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x2F;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::BACKLIGHT_COMPENSATION_ON_OFF:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x31;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::AUTO_WHITE_BALANCE_ON_OFF:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x33;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ENABLE_DEVICE_PHASE_DELAY_MODE:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x35;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::SET_SHUTTER_SPEED:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x37;
        packet[4] = 0x00;
        packet[5] = data_1;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ADJUST_LINE_LOCK_PHASE_DELAY:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = data_1;
        packet[3] = 0x39;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ADJUST_WHITE_BALANCE_R_B:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = data_1;
        packet[3] = 0x3B;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ADJUST_WHITE_BALANCE_M_G:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = data_1;
        packet[3] = 0x3D;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ADJUST_GAIN:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = data_1;
        packet[3] = 0x3F;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ADJUST_AUTO_IRIS_LEVEL:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = data_1;
        packet[3] = 0x41;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::ADJUST_AUTO_IRIS_PEACK_VALUE:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = data_1;
        packet[3] = 0x43;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    case pelco::PelcoDCommands::QUERY:
        packet[0] = 0xFF;
        packet[1] = address;
        packet[2] = 0x00;
        packet[3] = 0x45;
        packet[4] = 0x00;
        packet[5] = 0x00;
        GetChecksum(packet);
        return true;

    default:
        return false;
    }
}
