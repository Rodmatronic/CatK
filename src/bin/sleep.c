void sleep(int seconds);

void sleep(int seconds) {
    uint8 start_time = read_RTC_register(RTC_SECONDS);
    uint8 current_time;

    while (1) {
        current_time = read_RTC_register(RTC_SECONDS);

        if (current_time != start_time) {
            if (current_time < start_time) {
                current_time += 60; // Handle rollover from 59 to 00 by adding a minute
            }

            if (current_time - start_time >= seconds) {
                break;
            }
        }
    }
}