#include <stdio.h>

#include "event.hpp"
#include "logging.h"
#include "bt.h"

int main() {
	return !verifyBluetooth();
}
