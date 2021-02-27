#ifndef __USB_H
#define __USB_H

int psix_usb_init();
void psix_usb_start();
int psix_usb_stop();
void psix_usb_shutdown();
void psix_usb_toggle();
int psix_usb_enabled();
void psix_usb_enable();
void psix_usb_disable();
#endif
