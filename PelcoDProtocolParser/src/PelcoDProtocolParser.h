#include <cstdint>


namespace pelco
{
    enum class PelcoDCommands
    {
        // Main commands.
        CAMERA_ON,
        CAMERA_OFF,
        IRIS_CLOSE,
        IRIS_OPEN,
        FOCUS_NEAR,
        FOCUS_FAR,
        FOCUS_STOP,
        ZOOM_WIDE,
        ZOOM_TELE,
        ZOOM_STOP,
        DOWN,
        UP,
        LEFT,
        RIGHT,
        UP_RIGHT,
        UP_LEFT,
        DOWN_RIGHT,
        DOWN_LEFT,
        STOP,

        // Extended commands.
        SET_PRESET,
        CLEAR_PRESET,
        GO_TO_PRESET,
        FLIP_180DEG_ABOUT,
        GO_TO_ZERO_PAN,
        SET_AUXILIARY,
        CLEAR_AUXILIARY,
        REMOTE_RESET,
        SET_ZONE_START,
        SET_ZONE_END,
        WRITE_CHAR_TO_SCREEN,
        CLEAR_SCREEN,
        ALARM_ACKNOWLEDGE,
        ZONE_SCAN_ON,
        ZONE_SCAN_OFF,
        SET_PATTERN_START,
        SET_PATTERN_STOP,
        RUN_PATTERN,
        SET_ZOOM_SPEED,
        SET_FOCUS_SPEED,
        RESET_CAMERA_TO_DEFAULT,
        AUTO_FOCUS_AUTO_ON_OFF,
        AUTO_IRIS_AUTO_ON_OFF,
        AGC_AUTO_ON_OFF,
        BACKLIGHT_COMPENSATION_ON_OFF,
        AUTO_WHITE_BALANCE_ON_OFF,
        ENABLE_DEVICE_PHASE_DELAY_MODE,
        SET_SHUTTER_SPEED,
        ADJUST_LINE_LOCK_PHASE_DELAY,
        ADJUST_WHITE_BALANCE_R_B,
        ADJUST_WHITE_BALANCE_M_G,
        ADJUST_GAIN,
        ADJUST_AUTO_IRIS_LEVEL,
        ADJUST_AUTO_IRIS_PEACK_VALUE,
        QUERY
    };


    class PelcoDProtocolParser
    {
    public:

        PelcoDProtocolParser();

        ~PelcoDProtocolParser();

        bool GetCommand(uint8_t* packet, uint8_t address, pelco::PelcoDCommands command_ID, uint8_t data_1 = 0, uint8_t data_2 = 0);

    private:

        void GetChecksum(uint8_t* packet);

    };
}
