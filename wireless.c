#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"
#include "wifi_scan.h"

// https://stackoverflow.com/questions/59087739/wireless-essid-and-signal-strength-in-c-using-wireless-h

const int BSS_INFOS=10;

static    struct iwreq wrq;

int wifi_find(void) {
      struct ifaddrs *ifaddr, *ifa;
      int sock = -1;

      if (getifaddrs(&ifaddr) == -1) {
        perror("Error getting interface addresses");
        return -1;
      }

      for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
	char *ifname = new_string_sz(ifa->ifa_name, IFNAMSIZ);
	printf("search ifname=%s\n", ifname);

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
          perror("Error opening INET socket");
	  return -1;
        }

	strncpy(wrq.ifr_name, ifa->ifa_name, IFNAMSIZ);
        if (ioctl(sock, SIOCGIWNAME, &wrq) == 0) {
          printf("found IF: %s\n", wrq.ifr_name);
          freeifaddrs(ifaddr);
          return sock;
        }
	close(sock);
      }
      freeifaddrs(ifaddr);
      return -1;
    }

int wifi_info(int sock) {
      char essid[IW_ESSID_MAX_SIZE+1];
      struct iw_statistics iwstats;

      wrq.u.essid.pointer = essid;
      if (ioctl(sock, SIOCGIWESSID, &wrq) == -1) {
        perror("Can't open socket to obtain essid");
        return(-1);
      }

      printf("ESSID is %s\n", (char*)wrq.u.essid.pointer);

      memset(&iwstats, 0, sizeof(iwstats));

      wrq.u.data.pointer = &iwstats;
      wrq.u.data.length = sizeof(struct iw_statistics);
      wrq.u.data.flags = 1;

      if (ioctl(sock, SIOCGIWSTATS, &wrq) == -1) {
        perror("Can't open socket to obtain iwstats");
        return(-1);
      }

      printf("Signal level is %d\n", iwstats.qual.updated);
      return(0);
    }

const char *bssid_to_string(const uint8_t bssid[BSSID_LENGTH], char bssid_string[BSSID_STRING_LENGTH])
{
	snprintf(bssid_string, BSSID_STRING_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
         bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
	return bssid_string;
}

int wireless_scan_all(const char *device)
{
	struct wifi_scan *wifi=NULL;    //this stores all the library information
	struct bss_info bss[BSS_INFOS]; //this is where we are going to keep informatoin about APs (Access Points)
	char mac[BSSID_STRING_LENGTH];  //a placeholder where we convert BSSID to printable hardware mac address
	int status, i;

	
	// initialize the library with network interface argv[1] (e.g. wlan0)
	wifi=wifi_scan_init(device);

	while(1)
	{
		status=wifi_scan_all(wifi, bss, BSS_INFOS);
		
		//it may happen that device is unreachable (e.g. the device works in such way that it doesn't respond while scanning)
		//you may test for errno==EBUSY here and make a retry after a while, this is how my hardware works for example
		if(status<0)
			perror("Unable to get scan data");
		else //wifi_scan_all returns the number of found stations, it may be greater than BSS_INFOS that's why we test for both in the loop
			for(i=0;i<status && i<BSS_INFOS;++i)	
				printf("%s %s signal %d dBm on frequency %u MHz seen %d ms ago status %s\n",
				   bssid_to_string(bss[i].bssid, mac), 
				   bss[i].ssid,  
				   bss[i].signal_mbm/100, 
				   bss[i].frequency,
				   bss[i].seen_ms_ago, 
				   (bss[i].status==BSS_ASSOCIATED ? "associated" : "")
				);

		printf("\n");

		sleep(2);
	}
	
	//free the library resources
	wifi_scan_close(wifi);

	return 0;
}
