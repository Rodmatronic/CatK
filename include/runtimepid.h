// Define a structure to represent an application
typedef struct {
    int Pid;
    char AppName[32];
    void (*AppClose)();
} Application;

// Function to initialize an application
void InitApplication(Application* app, int pid, const char* appName, void (*closeFunction)()) {
    app->Pid = pid;
    strncpy(app->AppName, appName, sizeof(app->AppName));
    app->AppClose = closeFunction;
}

// Function to close an application
void AppClose(Application* app) {
    if (app->AppClose != NULL) {
        app->AppClose();
    }
}

// Example application close function
void MyAppClose() {
    // Perform any cleanup or shutdown operations for your application here
}