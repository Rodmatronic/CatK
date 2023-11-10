void sleep(int seconds);

// Function to wait for a specified number of seconds
void sleep(int seconds) {
    uint8 start_time = read_RTC_register(RTC_SECONDS);

    while (1) {
        uint8 current_time = read_RTC_register(RTC_SECONDS);
        if (current_time != start_time) {
            if (current_time < start_time) {
                current_time += 1; // Handle rollover from 59 to 00
            }

            if (current_time - start_time >= seconds) {
                break;
            }
        }
    }
}