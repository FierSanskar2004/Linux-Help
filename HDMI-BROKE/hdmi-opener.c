#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define DRM_PATH "/sys/class/drm"

void force_connector(const char *connector_name) {
    char path[512];
    // Path to the trigger file that forces a hardware re-probe/override
    snprintf(path, sizeof(path), "%s/%s/status", DRM_PATH, connector_name);
    
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        perror("[-] Failed to open status file (Are you root?)");
        return;
    }

    // "detect" tells the Intel i915 driver to force-trigger a hardware probe 
    // even if the physical HDMI pin hasn't triggered an electrical hotplug interrupt
    fprintf(file, "detect");
    fclose(file);
    printf("[+] Successfully forced probe signal on: %s\n", connector_name);
}

int main() {
    DIR *dir = opendir(DRM_PATH);
    struct dirent *entry;

    if (dir == NULL) {
        perror("[-] Cannot open /sys/class/drm directory");
        return EXIT_FAILURE;
    }

    printf("[*] Scanning for video display interfaces...\n");
    while ((entry = readdir(dir)) != NULL) {
        // Look for Intel HDMI or DisplayPort-to-HDMI interfaces (e.g., card1-HDMI-A-1)
        if (strstr(entry->d_name, "HDMI-A") != NULL || strstr(entry->d_name, "DP") != NULL) {
            printf("[*] Found interface: %s\n", entry->d_name);
            force_connector(entry->d_name);
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
}
