uint8 seedpasswd = 7;

char* cryptobuffer[128];

void encrypt(char* input)
{
    add_data_to_file(&rootfs, "logs.d", "crypto: [ .. ] encrypting...\n");

    char* working_dir = current_directory;
    current_directory = "/etc";

    if (input == NULL)
    {
        printf("Error: Input is NULL\n");
        return;
    }

    size len = strlen(input);

    for (size i = 0; i < len; i++)
    {
        // Use a more complex transformation based on the seed
        input[i] = (input[i] + seedpasswd) % 256;
    }

    read_from_file(&rootfs, "session.catk", cryptobuffer, sizeof(cryptobuffer), 1);

    write_to_file(&rootfs, "passwd", "");
    add_data_to_file(&rootfs, "passwd", cryptobuffer);
    add_data_to_file(&rootfs, "passwd", ":*:");
    add_data_to_file(&rootfs, "passwd", input);
    add_data_to_file(&rootfs, "passwd", ":*:");
    add_data_to_file(&rootfs, "passwd", rootUser.shell);

    current_directory = working_dir;

}

void decrypt(char* input)
{
    add_data_to_file(&rootfs, "logs.d", "crypto: [ .. ] decrypting...\n");

    if (input == NULL)
    {
        printf("Error: Input is NULL\n");
        return;
    }

    size len = strlen(input);

    for (size i = 0; i < len; i++)
    {
        // Reverse the substitution cipher to decrypt
        input[i] = (input[i] - seedpasswd + 256) % 256;
    }
}