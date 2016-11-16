#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

//#include "../include/TransmitRequest.hpp"
#include "../include/ReceivePacket.hpp"
#include "../include/Xbee.hpp"
#include "../include/Utility.hpp"
#include "../lib/Utility.cpp"

namespace XBEE {

  ReceivePacket::ReceivePacket() {
    frame_type = 0x90;
    source_mac_64 = static_cast<uint64_t> (BroadcastType_64::UNKNOWN);
    source_mac_16 = static_cast<uint16_t> (BroadcastType_16::DEFAULT);
    ReceivePacket::SetLength();
    ReceivePacket::SetChecksum();
  }

  ReceivePacket::ReceivePacket(uint64_t source_64) {
    frame_type = 0x90;
    source_mac_64 = source_64;
    source_mac_16 = static_cast<uint64_t> (BroadcastType_16::DEFAULT);
    ReceivePacket::SetLength();
    ReceivePacket::SetChecksum();
  }

  ReceivePacket::ReceivePacket(uint64_t source_64, uint16_t source_16) {
    frame_type = 0x90;
    source_mac_64 = source_64;
    source_mac_16 = source_16;
    ReceivePacket::SetLength();
    ReceivePacket::SetChecksum();
  }

  void ReceivePacket::SetLength() {
    length = sizeof (frame_type)
            + sizeof (source_mac_64)
            + sizeof (source_mac_16)
            + sizeof (options);
    for (auto it = data.begin(); it != data.end(); ++it) {
      length += (*it != 0x00) ? sizeof (*it) : 0x00;
    }
  }

  std::string ReceivePacket::GetData() {

    // TODO: Test
    std::stringstream data_str;
    for (auto itr = data.begin(); itr != data.end(); ++itr)
      if (*itr != 0x00)
        data_str << HexString(*itr, false);
    return data_str.str();
  }

  std::string ReceivePacket::ToHexString(HexFormat spacing) const {
    
        std::stringstream tmp;
    // TODO: Implement HexString function without third argument
      bool even_space = false;
      bool data_space = false;
      switch (spacing) {
        case HexFormat::BYTE_SPACING:
          even_space = true;
        case HexFormat::DATA_SPACING:
          data_space = true;
        case HexFormat::NO_SPACING:
          tmp << HexString(start, even_space, data_space);
          tmp << HexString(length, even_space, data_space);
          tmp << HexString(frame_type, even_space, data_space);
          tmp << HexString(source_mac_64, even_space, data_space);
          tmp << HexString(source_mac_16, even_space, data_space);
          tmp << HexString(options, even_space, data_space);
          for (auto itr = data.begin(); itr != data.end(); ++itr)
            if (*itr != 0x00)
              tmp << HexString(*itr, even_space, even_space);
          
          // TODO: Figure out how to avoid if statement
          // checksum wont be spaced for DATA_SPACING case
          // if a ' ' is thrown in, checksum wont be not spaced for NO_SPACING case
          if (spacing == HexFormat::DATA_SPACING) tmp << ' ';
          tmp << HexString(checksum, even_space, false);
      }
      return tmp.str();
    
//    std::stringstream tmp;
//    bool even_space = false;
//    std::string space = "";
//    switch (spacing) {
//      case HexFormat::BYTE_SPACING:
//        even_space = true;
//      case HexFormat::DATA_SPACING:
//        space = ' ';
//      case HexFormat::NO_SPACING:
//        tmp << HexString(start, even_space) << space;
//        tmp << HexString(length, even_space) << space;
//        tmp << HexString(frame_type, even_space) << space;
//        tmp << HexString(source_mac_64, even_space) << space;
//        tmp << HexString(source_mac_16, even_space) << space;
//        tmp << HexString(options, even_space) << space;
//        for (auto itr = data.begin(); itr != data.end(); ++itr)
//          if (*itr != 0x00)
      
      // this loop will not work as intended for DATA_SPACING because the data array is iterated per char
//            tmp << HexString(*itr, even_space) << space;
//        tmp << HexString(checksum, even_space) << space;
//        break;
//    }
//    return tmp.str();
  }

  void ReceivePacket::SetChecksum() {

    uint8_t byte_sum = ByteSum(frame_type);
    byte_sum += ByteSum(source_mac_64);
    byte_sum += ByteSum(source_mac_16);
    byte_sum += options;

    for (auto itr = data.begin(); itr != data.end(); ++itr) {
      byte_sum += *itr;
    }
    checksum = kMaxChecksum - byte_sum;
  }

}