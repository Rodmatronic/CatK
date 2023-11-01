void encrypt(char* input)
{
    if (input == NULL)
    {
        printf("Error: Input is NULL\n");
        return;
    }

    while (*input != '\0')
    {
        if (*input >= 'a' && *input <= 'z')
        {
            *input = *input - 'a' + '0';
        }
        else if (*input >= 'A' && *input <= 'Z')
        {
            *input = *input - 'A' + '0' + 26;
        }
        else if (*input >= '0' && *input <= '9')
        {
            *input = *input - '0' + 'a';
        }

        input++;
        input++;
    }
}
