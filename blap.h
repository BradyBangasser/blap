#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum packet_types {
    PT_SUP = 0x01,
    PT_ESUP = 0x02,
    PT_HB = 0x04,

    PT_PAYLOAD = 0xFF,
};

#ifdef __cplusplus
}
#endif
