#ifndef __TELLO_WIRELESS_H__
#define __TELLO_WIRELESS_H__ 1

int wireless_scan_all(const char *device);
int wifi_find(void);
int wifi_info(int sock);

#endif /* __TELLO_WIRELESS_H__ */
