#include "bt.h"
#include "logging.h"

#ifdef BLAP_EMBEDDED

#else
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#endif

bool verifyBluetooth() {
    WARN("Bluetooth Interface not yet implemented\n");
	#ifdef BLAP_EMBEDDED
		
	#else
	int sock, devid;
	devid = hci_get_route(NULL);

	if (devid < 0) {
        DEBUGF("No Bluetooth Device Detected, hci_get_route returned: %d\n", devid);
		return false;
	}

	#endif
	return true;
}
