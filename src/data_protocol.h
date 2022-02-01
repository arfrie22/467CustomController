#ifndef COMMAND_PROTOCOL
#define COMMAND_PROTOCOL
#define COMMAND_PROTOCOL_VERSION 0x0100

enum data_command_id {
    id_get_protocol_version = 0x01,
    id_get_team_number = 0x02,
    id_get_controller_state = 0x03,
    id_get_led_data = 0x04,
    id_get_led = 0x05,
    id_set_led = 0x06,


    //...
    id_get_port_name = 0xFD,
    id_enter_bootloader = 0xFE,
    id_error = 0xFF
};

enum data_led_data {
    id_led_count = 0x01,
    id_section_count = 0x02
};

enum data_lighting_selection {
    id_single = 0x01,
    id_multiple = 0x02,
    id_section = 0x03,
    id_all = 0x04
};

enum data_lighting_value {
    id_led_base_color = 0x01,
    id_led_effect = 0x02,
    id_led_effect_spaced = 0x03,
    id_led_offset = 0x04,
    id_led_speed = 0x05,
    id_led_brightness = 0x06
};
#endif //COMMAND_PROTOCOL

