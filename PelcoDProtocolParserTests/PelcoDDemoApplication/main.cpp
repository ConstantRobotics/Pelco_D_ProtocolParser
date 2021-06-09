#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstring>
#include <cstdint>
#include <atomic>
#include <string.h>
#include "json.hpp"
#include "TCPPort.h"
#include "SerialPort.h"
#include <PelcoDProtocolParser.h>
#include <opencv2/opencv.hpp>

using json = nlohmann::json;    // Necсessary for JSON library.


/*
 * GLOBAL CONSTANTS.
 */
std::string configuration_file_name = "PelcoDDemoApplication.json"; // Config file name.


/*
 * GLOBAL VARIABLES.
 */
cv::VideoCapture video_source;  // Video source.
clib::SerialPort serial_port;   // Serial port.
clib::TCPPort tcp_port;         // TCP port.
int frame_width = 0;            // Video frame width.
int frame_height = 0;           // Video frame height.
int mouse_x = 0;                // Horizontal mouse position.
int mouse_y = 0;                // Vertical mouse position.
int camera_address = 1;         // Pelco-D camera address.
int max_PTU_speed = 63;         // Maximum PTU speed (max 63 according Pelco-D).
bool button_pushed = false;     // Mouse button pushed flag.
pelco::PelcoDProtocolParser pelco_protocol_parser;  // Pelco-D protocol parser.


/*
 * FUNCTIONS PROTOTYPES.
 */

/// Read and init params function prototype.
bool Load_And_Init_Params();

/// Mouse callback prototype function.
void Mouse_Call_Back_Function(int event, int x, int y, int flags, void* userdata);

/// Keyboard processing functions.
void Keyboard_Processing_Function(int key);

/// Draw info function.
void Draw_Info_Function(cv::Mat frame);


// Entry point.
int main(void)
{
    // Load and init params.
    if (!Load_And_Init_Params())
    {
        std::cout << "ERROR: Parameters not loaded. Exit..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return -1;
    }

    // Init variables.
    cv::Mat frame = cv::Mat(cv::Size(frame_width, frame_height), CV_8UC3);

    // Init windows.
    cv::namedWindow("PELCO-D DEMO APPLICATION", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("PELCO-D DEMO APPLICATION", 5, 5);
    cv::setMouseCallback("PELCO-D DEMO APPLICATION", Mouse_Call_Back_Function, nullptr);

    // Main loop.
    while (true)
    {
        // Capture video frame.
        video_source >> frame;
        if (frame.empty())
        {
            video_source.set(cv::CAP_PROP_POS_FRAMES, 1);
            continue;
        }

        // Control PTU according to mouse position.
        if (button_pushed)
        {
            // Calculate speed.
            int delta_pan = mouse_x - (frame_width / 2);
            int delta_tilt = mouse_y - (frame_height / 2);

            uint8_t pan_speed = (uint8_t)(((float)abs(delta_pan) / (float)(frame_width / 2)) * (float)max_PTU_speed);
            uint8_t tilt_speed = (uint8_t)(((float)abs(delta_tilt) / (float)(frame_height / 2)) * (float)max_PTU_speed);

            if (delta_pan < 0)
            {
                if (delta_tilt < 0)
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::UP_LEFT, pan_speed, tilt_speed))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
                else if (delta_tilt > 0)
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::DOWN_LEFT, pan_speed, tilt_speed))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
                else // delta_tilt == 0
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::LEFT, pan_speed, 0))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
            }
            else if (delta_pan > 0)
            {
                if (delta_tilt < 0)
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::UP_RIGHT, pan_speed, tilt_speed))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
                else if (delta_tilt > 0)
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::DOWN_RIGHT, pan_speed, tilt_speed))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
                else // delta_tilt == 0
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::RIGHT, pan_speed, 0))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
            }
            else // delta_pan == 0
            {
                if (delta_tilt < 0)
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::UP, pan_speed, tilt_speed))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
                else if (delta_tilt > 0)
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::DOWN, pan_speed, tilt_speed))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
                else // delta_tilt == 0
                {
                    uint8_t packet[7];
                    if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::STOP, 0, 0))
                    {
                        tcp_port.SendData(packet, 7);
                        serial_port.SendData(packet, 7);
                    }
                }
            }
        }

        // Draw info on video.
        Draw_Info_Function(frame);

        // Show video.
        cv::imshow("PELCO-D DEMO APPLICATION", frame);

        // Keyboard processing function.
        Keyboard_Processing_Function(cv::waitKey(1));

    }

    return 1;
}


bool Load_And_Init_Params()
{
    // Init variables.
    json json_conf;

    // Open JSON file.
    std::ifstream configuration_file(configuration_file_name);
    if (!configuration_file.is_open())
    {
        return false;
    }

    // Parse JSON file.
    try {
        json_conf = json::parse(configuration_file);
    }
    catch (json::parse_error& e)
    {
        // Close file.
        configuration_file.close();
        return false;
    }

    // Close file.
    configuration_file.close();

    // Read params.
    try
    {
        // Read video source params.
        json video_source_params = json_conf.at("Video_Source_Params");
        std::string video_source_init_string = video_source_params.at("video_source_init_string").get<std::string>();

        // Init video source.
        if (video_source_init_string.length() < 4)
        {
            if (!video_source.open(std::stoi(video_source_init_string)))
                return false;
        }
        else
        {
            if (!video_source.open(video_source_init_string))
                return false;
        }

        // Get video frame size.
        frame_width = (int)video_source.get(cv::CAP_PROP_FRAME_WIDTH);
        frame_height = (int)video_source.get(cv::CAP_PROP_FRAME_HEIGHT);

        // Read communication params.
        json communication_params = json_conf.at("Communication_Params");
        std::string camera_IP = communication_params.at("camera_IP").get<std::string>();
        int TCP_port = communication_params.at("TCP_port").get<int>();
        std::string serial_port_name  = communication_params.at("serial_port_name").get<std::string>();
        int serial_port_baudrate = communication_params.at("serial_port_baudrate").get<int>();
        camera_address = communication_params.at("camera_address").get<int>();
        max_PTU_speed = communication_params.at("max_PTU_speed").get<int>();

        // Init TCP port.
        tcp_port.SetIPAddr(camera_IP.c_str());
        if (!tcp_port.Open(TCP_port, 100, false))
        {
            std::cout << "PTU Pelco-D TCP port not open" << std::endl;
        }

        // Init serial port.
        if (!serial_port.Open(serial_port_name.c_str(), serial_port_baudrate, 100))
        {
            std::cout << "PTU Pelco-D serial port port not open" << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}



void Mouse_Call_Back_Function(int event, int x, int y, int flags, void* userdata)
{
    flags = 0;
    userdata = nullptr;

    // Update mouse position.
    mouse_x = x;
    mouse_y = y;

    // Check ivent.
    switch (event)
    {
    case cv::EVENT_LBUTTONDOWN: // Capture/Reset function
        button_pushed = true;
        break;
    case cv::EVENT_LBUTTONUP: // Capture/Reset function
        button_pushed = false;
        // Create stop command.
        {
            uint8_t packet[7];
            if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::STOP))
            {
                tcp_port.SendData(packet, 7);
            }
        }
        break;
    case cv::EVENT_RBUTTONDOWN: break;
    case cv::EVENT_RBUTTONUP: break;
    case cv::EVENT_MBUTTONDOWN:	break;
    case cv::EVENT_MOUSEMOVE: break;
    }
}



void Keyboard_Processing_Function(int key)
{
    // Init variables.
    uint8_t packet[7];

    switch (key)
    {
    // Exit.
    case 27: // "ESC"
        // Stop video recording.
        cv::destroyAllWindows();
        exit(0);

    // STOP camera and PTU.
    case 32: // "SPACE"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::STOP))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

        // Camera ON.
    case 49: // "1"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::CAMERA_ON))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Camera OFF.
    case 50: // "2"
       if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::CAMERA_OFF))
       {
           tcp_port.SendData(packet, 7);
           serial_port.SendData(packet, 7);
       }
       break;

    // Focus NEAR.
    case 51: // "3"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::FOCUS_NEAR))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Focus FAR.
    case 52: // "4"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::FOCUS_FAR))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Zoom WIDE.
    case 53: // "5"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::ZOOM_WIDE))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Zoom tele.
    case 54: // "6"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::ZOOM_TELE))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Move RIGHT.
    case 100: // "D"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::RIGHT, 0x10, 0))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Move LEFT.
    case 97: // "A"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::LEFT, 0x10, 0))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Move UP.
    case 119: // "W"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::UP, 0, 0x10))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Move DOWN.
    case 120: // "X"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::DOWN, 0, 0x10))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Move UP-LEFT.
    case 113: // "Q"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::UP_LEFT, 0x10, 0x10))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Mode UP-RIGHT.
    case 101: // "E"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::UP_RIGHT, 0x10, 0x10))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Move DOWN-LEFT.
    case 122: // "Z"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::DOWN_LEFT, 0x10, 0x10))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;

    // Mode DOWN-RIGHT.
    case 99: // "C"
        if (pelco_protocol_parser.GetCommand(packet, camera_address, pelco::PelcoDCommands::DOWN_RIGHT, 0x10, 0x10))
        {
            tcp_port.SendData(packet, 7);
            serial_port.SendData(packet, 7);
        }
        break;
    }
}



void Draw_Info_Function(cv::Mat frame)
{
    // Draw central lines.
    cv::line(frame, cv::Point(0, frame_height / 2), cv::Point(frame_width, frame_height / 2), cv::Scalar(0, 0, 0));
    cv::line(frame, cv::Point(frame_width / 2, 0), cv::Point(frame_width / 2, frame_height), cv::Scalar(0, 0, 0));

    // Draw mouse line.
    cv::line(frame, cv::Point(frame_width / 2, frame_height / 2), cv::Point(mouse_x, mouse_y), cv::Scalar(255, 255, 0));

    // Draw buttons info.
    cv::putText(frame, "BUTTONS:", cv::Point(5, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "SPACE - Stop PTU, Zoom, Focus", cv::Point(5, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "1 - Camera ON", cv::Point(5, 65), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "2 - Camera OFF", cv::Point(5, 90), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "3 - Focus NEAR", cv::Point(5, 115), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "4 - Focus FAR", cv::Point(5, 140), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "5 - Zoom WIDE", cv::Point(5, 165), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "6 - Zoom TELE", cv::Point(5, 190), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "D - Move RIGHT (fixed speed)", cv::Point(5, 215), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "A - Move LEFT (fixed speed)", cv::Point(5, 240), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "W - Move UP (fixed speed)", cv::Point(5, 265), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "X - Move DOWN (fixed speed)", cv::Point(5, 290), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "Q - Move UP-LEFT (fixed speed)", cv::Point(5, 315), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "E - Move UP-RIGHT (fixed speed)", cv::Point(5, 340), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "Z - Move DOWN-LEFT (fixed speed)", cv::Point(5, 365), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    cv::putText(frame, "C - Move DOWN-RIGHT (fixed speed)", cv::Point(5, 390), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
}
