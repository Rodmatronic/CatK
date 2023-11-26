void catsay(input)
{
    add_data_to_file(&rootfs, "logs.catk", "catsay: [ ok ] Started\n");
    console_init(COLOR_WHITE, COLOR_BLACK);
    printf(" ______________________________________________\n");
    printf("< %s\n", input);
    printf(" ----------------------------------------------\n");
    printf("                \\/\n");
    printf_white("             _      _\n");
    printf_white("            / \\    / \\\n");
    printf_brightcyan("           /   \\__/   \\\n");
    printf_brightcyan("          /            \\\n");
    printf_brightcyan("         |    |    |    |\n");
    printf_brightcyan("        =|      -       |=\n");
    printf_brightcyan("        =\\      v       /=\n");
    printf_brightcyan("          \\            /\n");
    printf_red("           =====\\/=====\n");
    printf_yellow("              (CatK)\n");
}