void encrypt(char* input)
{
    if (input == NULL)
    {
        printf("Error: Input is NULL\n");
        return;
    }

    // Use "seconds" to determine a random symbol
    char randomSymbol1 = (char)(seed * seconds % 26 + 'A');
    char randomSymbol2 = (char)(seed * randomSymbol1 % 26 + 'A');
    char randomSymbol3 = (char)(seed * randomSymbol2 % 26 + 'A');
    char randompass = (char)(seed * randomSymbol1 * randomSymbol2 - randomSymbol3 % 26 + 'A');

    while (*input != '\0')
    {
        if (*input >= 'a' && *input <= 'z')
        {
            *input = *input - 'a' + '0';
            *input = randomSymbol1;
        }
        else if (*input >= 'A' && *input <= 'Z')
        {
            *input = *input - 'A' + '0' + 38;
        }
        else if (*input >= '0' && *input <= '9')
        {
            *input = *input - '0' + 'a';
            *input = randomSymbol2;
        }

        input++;
        input++;

        // Add the random symbol
        *input = randompass;
        input++;
    }
}