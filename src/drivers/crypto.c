uint8 seedpasswd;

void encrypt(char* input)
{
    seedpasswd = seconds;
    if (input == NULL)
    {
        printf("Error: Input is NULL\n");
        return;
    }

    size len = strlen(input);

    for (size i = 0; i < len; i++)
    {
        input[i] ^= seedpasswd;
    }
}

void decrypt(char* input)
{
    if (input == NULL)
    {
        printf("Error: Input is NULL\n");
        return;
    }

    size len = strlen(input);

    for (size i = 0; i < len; i++)
    {
        input[i] ^= seedpasswd; // XOR with the same key to decrypt
    }
}
