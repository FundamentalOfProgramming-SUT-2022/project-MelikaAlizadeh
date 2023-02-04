#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

char clipboard[2000];

// tree
void treecommand(int tmpdepth, char *address, int depth)
{
    struct dirent *d;
    DIR *dir = opendir(address);
    char newaddress[500];
    if (!dir)
        return;
    while ((d = readdir(dir)) != NULL)
    {
        if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0)
        {
            for (int i = 0; i < 2 * tmpdepth; i++)
            {
                if (i % 2 == 0)
                    printf("%c", '|');
                else
                    printf(" ");
            }
            printf("%c%c%c%s\n", '|', '-', '-', d->d_name);
            if (depth == -2 || tmpdepth <= depth)
            {
                int j;
                int n = strlen(address);
                for (j = 0; j < n; j++)
                    newaddress[j] = address[j];
                newaddress[j] = '/';
                j++;

                int z;
                int n1 = strlen(d->d_name);
                for (z = 0; z < n1; z++)
                    newaddress[j + z] = d->d_name[z];
                newaddress[j + z] = '\0';
                treecommand(tmpdepth + 1, newaddress, depth);
            }
        }
    }
    closedir(dir);
}

// existenceofdir checks if dir alredey exists or not
int existenceofdir(char array[])
{
    char *pch;
    pch = strrchr(array, '/');
    array[pch - array] = '\0';
    printf("%s", array);
    DIR *dir = opendir(array);
    if (dir)
    {
        // Directory exists
        closedir(dir);
        return 1;
    }
    else if (ENOENT == errno)
        return 0;
    // Directory does not exist

    else
        return 0;
    // opendir() failed for some other reason
}

// existenceoffiles checks if file alredey exists or not
int existenceoffile(char array[])
{
    FILE *file = fopen(array, "r");
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    else
        return 0;
}

// createallfiles makes every files + parent files in the given address ONLY FOR LINUX
int createallfiles(char pathname[])
{
    for (char *p = pathname; (p = strchr(p, '/')) != NULL; ++p)
    {
        char c = p[1];
        p[1] = '\0';
        errno = 0;
        if (mkdir(pathname, 0700) != 0 && errno != EEXIST)
        {
            perror("mkdir");
            FILE *f = fopen(pathname, "w+");
            return -1;
        }
        p[1] = c;
    }
    int fd = creat(pathname, 0600);
    if (fd < 0)
        perror("creat");
    return fd;
}

// add the string in the given pos
void addtofile(char address[], char secondfile[], int line, int character, char str[])
{
    FILE *f = fopen(address, "r");
    char fstr[1000][1000] = {'\0'};
    char str1[1024] = {'\0'};
    char str2[1024] = {'\0'};
    int i = 0, check = 0;
    if (existenceoffile(secondfile) == 0)
    {
        createallfiles(secondfile);
    }
    FILE *secfile = fopen(secondfile, "w+");
    char c2 = fgetc(f);
    while (c2 != EOF)
    {
        fputc(c2, secfile);
        c2 = fgetc(f);
    }
    fclose(secfile);
    fseek(f, 0, SEEK_SET);
    while (fgets(fstr[i], 200, f) || i == 0)
    {
        i++;
        if (i == line)
        {
            if (strlen(fstr[i - 1]) - 1 < character)
            {
                fclose(f);
                printf("wrong char or line number");
            }
            check = 1;
            strcpy(str1, fstr[i - 1]);
            str1[character] = '\0';
            strcpy(str2, fstr[i - 1]);
            char *p = str2;
            p = p + character;
            strcat(str1, str);
            strcat(str1, p);
            strcpy(fstr[i - 1], str1);
        }
    }
    fclose(f);
    if (check == 1)
    {
        FILE *fp = fopen(address, "w");
        for (int j = 0; j < i; j++)
        {
            fputs(fstr[j], fp);
        }
        fclose(fp);
    }
}

// show the contents of file
void show(char c[])
{
    FILE *ptr;
    char str[50];
    ptr = fopen(c, "r");
    if (NULL == ptr)
    {
        printf("file can't be opened \n");
    }
    while (fgets(str, 50, ptr) != NULL)
    {
        printf("%s", str);
    }

    fclose(ptr);
}

// find the given string with the given option(s) in the file
int findstring(char address[], char string[], int starpos, int character, int line, int at, int count, int byword, int all)
{
    int n = strlen(string);
    char c;
    FILE *file = fopen(address, "r");
    c = fgetc(file);
    int condition = (c != EOF) && (c != ' ');
    int counter = 0;
    int startchar = 0, startline = 1;
    int fchar = 0;
    int result = -1, result1 = 0, tmpresult = -1;
    int same = 0;
    int check1 = 0, check2 = 0, check3 = 0, check = 0;
    for (int i = 0; i < n;)
    {
        counter++;
        int j = i;
        if (!(starpos == i))
        {
            check1 = 0;
            int pos = -i;
            i = 0;
            character = 0;
            startchar = 0;
            if ((string[starpos] != 0) && (string[starpos] != ' '))
            {
                if (starpos >= 0)
                {
                    fseek(file, pos, SEEK_CUR);
                    counter = counter + pos;
                }
            }
            else
            {
                fseek(file, pos, SEEK_CUR);
                counter = counter + pos;
            }
        }
        else if ((startchar == 0) && (starpos == i))
        {
            if ((string[i] != ' ') && (string[i] != '\0'))
            {
                if (i == 0)
                    result = fchar;
                while (condition == 1)
                {
                    counter++;
                    character++;
                    if ((check3 == 1) && (string[j] == ' ') || (string[j] == '\0'))
                    {
                        check3++;
                        break;
                    }
                    if (c == string[j])
                    {
                        j++;
                        check3 = 1;
                    }
                    else if (c != string[j])
                    {
                        check3 = 0;
                        j = i;
                    }
                    c = fgetc(file);
                    condition = (c != EOF) && (c != ' ');
                }
                if (check3 == 0)
                {
                    check1 = 0;
                    i = 0;
                    character = 0;
                }
                else if (check3 == 2)
                {
                    while (condition == 1)
                    {
                        counter += 1;
                        character += 1;
                        c = fgetc(file);
                        condition = (c != EOF) && (c != ' ');
                    }
                }
                else if (check3 != 0)
                {
                    i = j;
                    check1 = 1;
                    startchar = 1;
                    if (string[i] == ' ')
                    {
                        fseek(file, -check1, SEEK_CUR);
                        counter = counter - 1;
                    }
                }
                if ((c == EOF) || (c == ' '))
                    fchar = counter;
            }
        }
        else if (c == string[i])
        {
            check1 = 1;
            if (i == 0)
            {
                int tmp = counter - 1;
                if (starpos == 0)
                    result = fchar;
                else
                    result = tmp;
                if (!(tmp == result))
                    character = tmp - fchar + character;
            }
            i++;
            character += 1;
        }
        if ((string[i] == ' ') || (string[i] == '\0'))
        {
            if (check == 0 && check1 == 1 && starpos == i)
            {
                character -= 1;
                check++;
                while (condition == 1)
                {
                    counter = counter + 1;
                    c = fgetc(file);
                    condition = (c != EOF) && (c != ' ');
                    character++;
                }
                if (c == ' ')
                    fchar = counter;
                counter = counter - 1;
                fseek(file, -check, SEEK_CUR);
            }
        }
        if (check1 == 1)
        {
            if (n <= i)
            {
                check1--;
                i = 0;
                check = 0;
                startchar = 1;
                same++;
                if (tmpresult == result)
                    continue;
                else
                {
                    tmpresult = result;
                    if (count != 0)
                        character = 0;
                    else
                    {
                        if (at == same)
                        {
                            line = startline;
                            if (byword == 0)
                            {
                                fclose(file);
                                return result;
                            }
                            else
                            {
                                check2 = 1;
                                fclose(file);
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (c == EOF)
            break;
        if (c == ' ')
            fchar = counter;
        if (c == '\n')
        {
            i = 0;
            counter = 0;
            startline += 1;
            fchar = 0;
            result = -1;
            tmpresult = -1;
            check = 0;
            check1 = 0;
            check2 = 0;
        }
        c = fgetc(file);
        condition = (c != EOF) && (c != ' ');
    }
    if (byword == 1)
    {
        if (check2 == 1)
        {
            check1 = 0;
            FILE *file = fopen(address, "r");
            for (int j = 0; j < result; j++)
            {
                c = fgetc(file);
                condition = (c != EOF) && (c != ' ');

                if (c == ' ')
                {
                    if (check1 == 0)
                    {
                        check1 = 1;
                        result1 += 1;
                    }
                }
                else
                    check1 = 0;
            }
            fclose(file);
            return result1;
        }
    }
    if (count == 1)
    {
        fclose(file);
        return same;
    }
    return -1;
}

// findstr inputs
void findstr()
{
    int at, count, byword, all;
    char chec;
    char tmpaddress;
    char address[1000] = {'\0'};
    int roww = 0;
    char caddress[1000] = "/home/melika";
    char slash[1000] = "/";
    char str1[1000] = {'\0'};
    char string[1000] = {'\0'};
    char fstring[1000] = {'\0'};
    char line[1000] = {'\0'};
    getchar();
    int row = 0, col = 0;
    char split[200][200] = {'\0'};
    int i = 0;
    int check, num, size = 0, numline = 1;
    char *firstoption;
    char *secondoption;
    char *thirdoption;
    while (1)
    {
        scanf("%s", str1);
        if (strcmp(str1, "--file") == 0)
            break;
        else if (i == 0)
        {
            strcat(string, str1);
            i = 1;
        }
        else
        {
            strcat(string, " ");
            strcat(string, str1);
        }
    }
    int n = strlen(string);
    if (string[0] == '\"')
    {
        char *p = string;
        p++;
        p[n - 2] = '\0';
        strcpy(string, p);
    }
    int n1 = strlen(string);
    int x = 0, y = 0, tmp = 0;
    while (x < n1)
    {
        if ((string[x] == '\\') && (string[x + 1] == 'n'))
        {
            fstring[y] = '\n';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string[x] == '\\') && (string[x + 1] == '\"'))
        {
            fstring[y] = '\"';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string[x] == '\\') && (string[x + 1] == '\\'))
        {
            fstring[y] = '\\';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string[x] == '*') && (string[x - 1] != '\\'))
        {
            tmp = y;
            x++;
        }
        else if ((string[x] == '\\') && (string[x + 1] == '*'))
        {
            fstring[y] = '*';
            y++;
            x = x + 2;
        }

        fstring[y] = string[x];
        y++;
        x++;
    }

    getchar();
    scanf("%c", &chec);
    if (chec == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            address[roww] = tmpaddress;
            roww++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
    }
    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            address[roww] = tmpaddress;
            roww++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, address);
        strcat(caddress, slash);
    }
    if (existenceoffile(caddress) == 0)
    {
        if (existenceofdir(caddress) == 0)
        {
            printf(" wrong address\n");
            return;
        }
        else
        {
            printf(" wrong file\n");
            return;
        }
    }
    char ch = getchar();
    if (ch == ' ')
    {
        fgets(line, 1000, stdin);
        for (int i = 0; i <= strlen(line); i++)
        {
            if (line[i] == ' ' || line[i] == '\0' || line[i] == '\n')
            {
                split[row][col] = '\0';
                row++;
                col = 0;
            }
            else
            {
                split[row][col] = line[i];
                col++;
            }
        }
        firstoption = split[0];
        secondoption = split[1];
        thirdoption = split[2];
        if (strcmp(firstoption, "-count") == 0)
        {
            count = 1;
            if ((strcmp(secondoption, "-at") == 0) || (strcmp(secondoption, "-byword") == 0) || (strcmp(secondoption, "-all") == 0))
            {
                printf("invalid options\n");
                return;
            }
            else
            {
                at = 1;
                byword = 0;
                all = 0;
                check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
            }
        }
        else if (strcmp(firstoption, "-at") == 0)
        {
            all = 0;
            at = atoi(secondoption);
            if ((strcmp(thirdoption, "-count") == 0) || (strcmp(thirdoption, "-all") == 0))
            {
                printf("invalid options\n");
                return;
            }
            else if (strcmp(thirdoption, "-byword") == 0)
            {
                count = 0;
                byword = 1;
                check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
                check++;
            }
            else
            {
                count = 0;
                byword = 0;
                check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
            }
        }
        else if (strcmp(firstoption, "-byword") == 0)
        {
            byword = 1;
            if ((strcmp(secondoption, "-count") == 0))
            {
                printf("invalid options\n");
                return;
            }
            else if ((strcmp(secondoption, "-at") == 0))
            {
                count = 0;
                at = atoi(thirdoption);
                all = 0;
                check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
            }
            else if ((strcmp(secondoption, "-all") == 0))
            {
                all = 1;
                numline = 1;
                at = 1;
                count = 0;
                check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
                if (check != -1)
                {
                    while (check != -1)
                    {
                        printf("%d  ", check + 1);
                        at += 1;
                        check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
                    }
                    printf("\n");
                }
            }
            else
            {
                all = 0;
                at = 1;
                count = 0;
                check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
                check += 1;
            }
        }
        else if (strcmp(firstoption, "-all") == 0)
        {
            all = 1;
            if ((strcmp(secondoption, "-at") == 0) || (strcmp(secondoption, "-count") == 0))
            {
                printf("invalid options\n");
                return;
            }
            else if ((strcmp(secondoption, "-byword") == 0))
            {
                numline = 1;
                at = 1;
                count = 0;
                byword = 1;
                check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
                if (check != -1)
                {
                    while (check != -1)
                    {
                        printf("%d  ", check + 1);
                        at += 1;
                        check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
                    }
                    printf("\n");
                }
            }
            else
            {
                numline = 1;
                at = 1;
                count = 0;
                byword = 0;
                check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
                if (check != -1)
                {
                    while (check != -1)
                    {
                        printf("%d  ", check + 1);
                        at += 1;
                        count = 0;
                        byword = 0;
                        check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
                    }
                    printf("\n");
                }
            }
        }
    }
    else
    {
        all = 0;
        at = 1;
        count = 0;
        byword = 0;
        check = findstring(caddress, fstring, tmp, size, numline, at, count, byword, all);
    }
    if (check != -1)
        printf("%d\n", check);
    else
        printf("this string does not exist.\n");
}

// find the string to replace
int replacefinder(char address[], char string[], int starpos, int *size, int *line, int at, int count, int byword, int all)
{
    int n = strlen(string);
    char c;
    FILE *file = fopen(address, "r");
    c = fgetc(file);
    int condition = (c != EOF) && (c != ' ');
    int counter = 0;
    int startchar = 0, startline = 1;
    int fchar = 0;
    int result = -1, result1 = 0, tmpresult = -1;
    int same = 0;
    int check1 = 0, check2 = 0, check3 = 0, check = 0;
    for (int i = 0; i < n;)
    {
        counter++;
        if (c == string[i])
        {
            check1 = 1;
            if (i == 0)
            {
                int tmp = counter - 1;
                if (starpos == 0)
                    result = fchar;
                else
                    result = tmp;
                if (!(tmp == result))
                    *size = tmp - fchar + *size;
            }
            i++;
            *size = *size + 1;
        }
        else if ((startchar == 0) && (starpos == i))
        {
            int j = i;
            if ((string[i] != ' ') && (string[i] != '\0'))
            {
                if (i == 0)
                    result = fchar;
                while (condition == 1)
                {
                    counter++;
                    *size = *size + 1;
                    if ((string[j] == ' ') || (string[j] == '\0'))
                    {
                        if (check3 == 1)
                        {
                            check3 = 2;
                            break;
                        }
                    }
                    if (string[j] == c)
                    {
                        check3 = 1;
                        j++;
                    }
                    else
                    {
                        check3 = 0;
                        j = i;
                    }
                    c = fgetc(file);
                    condition = (c != EOF) && (c != ' ');
                }
                if (check3 == 2)
                {
                    while (condition == 1)
                    {
                        counter++;
                        *size = *size + 1;
                        c = fgetc(file);
                        condition = (c != EOF) && (c != ' ');
                    }
                }
                if (check3 == 0)
                {
                    *size = 0;
                    check1 = 0;
                    i = 0;
                }
                else
                {
                    i = j;
                    check1 = 1;
                    startchar = 1;
                    if (string[i] == ' ')
                    {
                        fseek(file, -check1, SEEK_CUR);
                        counter = counter - 1;
                    }
                }
                if ((c == EOF) || (c == ' '))
                    fchar = counter;
            }
        }
        else if (!(starpos == i))
        {
            check1 = 0;
            int pos = -i;
            i = 0;
            *size = 0;
            startchar = 0;
            if ((string[starpos] != 0) && (string[starpos] != ' '))
            {
                if (starpos >= 0)
                {
                    fseek(file, pos, SEEK_CUR);
                    counter = counter + pos;
                }
            }
            else
            {
                fseek(file, pos, SEEK_CUR);
                counter = counter + pos;
            }
        }
        if ((string[i] == ' ') || (string[i] == 0))
        {
            if (check == 0 && check1 == 1 && starpos == i)
            {
                *size = *size - 1;
                check = 1;
                while (condition == 1)
                {
                    counter = counter + 1;
                    c = fgetc(file);
                    condition = (c != EOF) && (c != ' ');
                    *size = *size + 1;
                }
                if (c == ' ')
                    fchar = counter;
                fseek(file, -check, SEEK_CUR);
                counter = counter - 1;
            }
        }
        if (check1 == 1)
        {
            if (n <= i)
            {
                check1 = 0;
                i = 0;
                check = 0;
                if (tmpresult == result)
                {
                    continue;
                }
                startchar = 1;
                same++;
                tmpresult = result;
                if (count != 0)
                    *size = 0;
                else
                {
                    if (at == same)
                    {
                        *line = startline;
                        if (byword == 0)
                        {
                            fclose(file);
                            return result;
                        }
                        else
                        {
                            check2 = 1;
                            fclose(file);
                            break;
                        }
                    }
                }
            }
        }
        if (c == EOF)
            break;
        else if (c == ' ')
            fchar = counter;
        if (c == '\n')
        {
            i = 0;
            counter = 0;
            startline += 1;
            fchar = 0;
            check = 0;
            check1 = 0;
            check2 = 0;
            result = -1;
            tmpresult = -1;
        }
        c = fgetc(file);
        condition = (c != EOF) && (c != ' ');
    }
    if (count == 1)
    {
        fclose(file);
        return same;
    }
    return -1;
}

// paste inputs
void pastestr()
{
    char check;
    char tmpaddress;
    char address[1000];
    int row = 0;
    char caddress[1000] = "/home/melika";
    char secondfile[2000] = "/home/melika/tempfiles";
    char slash[1000] = "/";
    char pos[10];
    int line, character;
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        strcat(secondfile, address);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            getchar();
            scanf("%s", pos);
            if (strcmp(pos, "--pos") == 0)
            {
                getchar();
                scanf("%d:%d", &line, &character);
                addtofile(caddress, secondfile, line, character, clipboard);
            }
        }
    }

    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, address);
        strcat(caddress, slash);
        strcat(secondfile, slash);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            scanf("%s", pos);
            if (strcmp(pos, "--pos") == 0)
            {
                getchar();
                scanf("%d:%d", &line, &character);
                addtofile(caddress, secondfile, line, character, clipboard);
            }
        }
    }
}

// cut chars from file and into the clipboard
void cuttoclip(char address[], char secondfile[], int fob, int chartoremove, int line, int character)
{
    char result[1000] = "\0";
    int start = 0, end = 0;
    int startline = 1, startchar = 0;
    int check = 0, checkpos = 0;
    char b[1000] = "\0", f[1000] = "\0";
    FILE *file = fopen(address, "r");
    char arr[1000];
    char coparr[1000] = "\0";

    if (existenceoffile(secondfile) == 0)
    {
        createallfiles(secondfile);
    }
    FILE *secfile = fopen(secondfile, "w+");
    char c2 = fgetc(file);
    while (c2 != EOF)
    {
        fputc(c2, secfile);
        c2 = fgetc(file);
    }
    fclose(secfile);
    fseek(file, 0, SEEK_SET);
    while (fgets(arr, 1000, file))
        strcat(coparr, arr);
    fclose(file);
    int n = strlen(coparr);
    int i = 0;
    while (i < n)
    {
        if (startline == line)
        {
            if (startchar == character)
            {
                checkpos = i;
                check = 1;
                break;
            }
        }
        if (coparr[i] == '\n')
        {
            startline++;
            startchar = 0;
        }
        else
            startchar++;
        i++;
    }

    if (fob == 1)
    {
        end = checkpos + chartoremove - 1;
        start = checkpos;
    }
    else if (fob == 0)
    {
        end = checkpos - 1;
        start = checkpos - chartoremove;
    }
    if (start < 0)
        start = 0;
    if (check == 0)
        return;
    if (end >= n)
        end = n - 1;
    int x = 0, y = 0, z = 0;
    for (int i = 0; i < n; i++)
    {
        if (i < start)
        {
            b[y] = coparr[i];
            y++;
            b[y] = '\0';
        }
        else if (end < i)
        {
            f[x] = coparr[i];
            x++;
            f[x] = '\0';
        }
        else
        {
            clipboard[z] = coparr[i];
            z++;
            clipboard[z] = '\0';
        }
    }
    strcat(result, b);
    strcat(result, f);
    FILE *ffile = fopen(address, "w");
    fputs(result, ffile);
    fclose(ffile);
}

// cut inputs
void cutstr()
{
    char check;
    char tmpaddress;
    char address[2000];
    int row = 0;
    char caddress[2000] = "/home/melika";
    char secondfile[2000] = "/home/melika/tempfiles";
    char slash[2000] = "/";
    char pos[10];
    int line, character, chartoremove;
    char size[10];
    char forb[3];
    int fob;
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        strcat(secondfile, address);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            getchar();
            scanf("%s", pos);
            if (strcmp(pos, "--pos") == 0)
            {
                getchar();
                scanf("%d:%d", &line, &character);
                getchar();
                scanf("%s", size);
                if (strcmp(size, "-size") == 0)
                {
                    getchar;
                    scanf("%d", &chartoremove);
                    getchar();
                    scanf("%s", forb);
                    if (strcmp(forb, "-f") == 0)
                        fob = 1;
                    else
                        fob = 0;
                    cuttoclip(caddress, secondfile, fob, chartoremove, line, character);
                }
            }
        }
    }

    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, address);
        strcat(caddress, slash);
        strcat(secondfile, slash);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            scanf("%s", pos);
            if (strcmp(pos, "--pos") == 0)
            {
                getchar();
                scanf("%d:%d", &line, &character);
                getchar();
                scanf("%s", size);
                if (strcmp(size, "-size") == 0)
                {
                    getchar;
                    scanf("%d", &chartoremove);
                    getchar();
                    scanf("%s", forb);
                    if (strcmp(forb, "-f") == 0)
                        fob = 1;
                    else
                        fob = 0;
                    cuttoclip(caddress, secondfile, fob, chartoremove, line, character);
                }
            }
        }
    }
}

// copy chars in clipboard array
void coptoclip(char address[], int fob, int chartoremove, int line, int character)
{
    char result[1000] = {'\0'};
    int start = 0, end = 0, startline = 1, startchar = 0, check = 0, checkpos = 0;
    char b[1000] = {'\0'};
    char f[1000] = {'\0'};
    char arr[1000];
    char coparr[1000] = {'\0'};
    FILE *file = fopen(address, "r");
    while (fgets(arr, 1000, file))
        strcat(coparr, arr);
    fclose(file);
    int n = strlen(coparr);
    int i = 0;
    while (i < n)
    {
        if (startline == line)
        {
            if (startchar == character)
            {
                checkpos = i;
                check = 1;
                break;
            }
        }
        if (coparr[i] == '\n')
        {
            startline++;
            startchar = 0;
        }
        else
            startchar++;
        i++;
    }

    if (fob == 1)
    {
        end = checkpos + chartoremove - 1;
        start = checkpos;
    }
    else if (fob == 0)
    {
        end = checkpos - 1;
        start = checkpos - chartoremove;
    }
    if (check == 0)
        return;

    if (start < 0)
        start = 0;
    if (end >= n)
        end = n - 1;
    int x = 0;
    for (int i = start; i <= end; i++)
    {
        clipboard[x] = coparr[i];
        x++;
    }
}

// copy inputs
void copystr()
{
    char check;
    char tmpaddress;
    char address[100];
    int row = 0;
    char caddress[100] = "/home/melika";
    char slash[100] = "/";
    char pos[10];
    int line, character, chartoremove;
    char size[10];
    char forb[3];
    int fob;
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            getchar();
            scanf("%s", pos);
            if (strcmp(pos, "--pos") == 0)
            {
                getchar();
                scanf("%d:%d", &line, &character);
                getchar();
                scanf("%s", size);
                if (strcmp(size, "-size") == 0)
                {
                    getchar;
                    scanf("%d", &chartoremove);
                    getchar();
                    scanf("%s", forb);
                    if (strcmp(forb, "-f") == 0)
                        fob = 1;
                    else
                        fob = 0;
                    coptoclip(caddress, fob, chartoremove, line, character);
                }
            }
        }
    }

    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, address);
        strcat(caddress, slash);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            scanf("%s", pos);
            if (strcmp(pos, "--pos") == 0)
            {
                getchar();
                scanf("%d:%d", &line, &character);
                getchar();
                scanf("%s", size);
                if (strcmp(size, "-size") == 0)
                {
                    getchar;
                    scanf("%d", &chartoremove);
                    getchar();
                    scanf("%s", forb);
                    if (strcmp(forb, "-f") == 0)
                        fob = 1;
                    else
                        fob = 0;
                    coptoclip(caddress, fob, chartoremove, line, character);
                }
            }
        }
    }
}

// remove chars from file
void removechar(char address[], char secondfile[], int fob, int chartoremove, int line, int character)
{
    char result[1000] = "\0";
    int start = 0, end = 0, startline = 1, startchar = 0, check = 0, checkpos = 0;
    char b[1000] = "\0", f[1000] = "\0";
    FILE *file = fopen(address, "r");
    if (existenceoffile(secondfile) == 0)
    {
        createallfiles(secondfile);
    }
    FILE *secfile = fopen(secondfile, "w+");
    char c2 = fgetc(file);
    while (c2 != EOF)
    {
        fputc(c2, secfile);
        c2 = fgetc(file);
    }
    fclose(secfile);
    fseek(file, 0, SEEK_SET);
    char arr[1000];
    char coparr[1000] = "\0";
    while (fgets(arr, 1000, file))
        strcat(coparr, arr);
    fclose(file);
    int n = strlen(coparr);

    for (int i = 0; i < n; i++)
    {
        if (startchar == character && startline == line)
        {
            checkpos = i;
            check = 1;
            break;
        }
        if (coparr[i] == '\n')
        {
            startline++;
            startchar = 0;
        }
        else
            startchar++;
    }

    if (fob == 0)
    {
        end = checkpos - 1;
        start = checkpos - chartoremove;
    }
    if (!check)
        return;
    if (fob == 1)
    {
        end = checkpos + chartoremove - 1;
        start = checkpos;
    }

    if (start < 0)
        start = 0;
    if (end >= n)
        end = n - 1;
    int x = 0, y = 0;
    for (int i = 0; i < n; i++)
    {
        if (i < start)
        {
            b[x] = coparr[i];
            x++;
        }
        else if (end < i)
        {
            f[y] = coparr[i];
            y++;
        }
        b[x] = '\0';
        f[y] = '\0';
    }
    strcat(result, b);
    strcat(result, f);

    FILE *fp = fopen(address, "w");
    fputs(result, fp);
    fclose(fp);
}

// removestr for getting the inputs of removestr
void removestr()
{
    char check;
    char tmpaddress;
    char address[100];
    int row = 0;
    char caddress[100] = "/home/melika";
    char secondfile[2000] = "/home/melika/tempfiles";
    char slash[100] = "/";
    char pos[10];
    int line, character, chartoremove;
    char size[10];
    char forb[3];
    int fob;
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        strcat(secondfile, address);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            getchar();
            scanf("%s", pos);
            if (strcmp(pos, "--pos") == 0)
            {
                getchar();
                scanf("%d:%d", &line, &character);
                getchar();
                scanf("%s", size);
                if (strcmp(size, "-size") == 0)
                {
                    getchar;
                    scanf("%d", &chartoremove);
                    getchar();
                    scanf("%s", forb);
                    if (strcmp(forb, "-f") == 0)
                        fob = 1;
                    else
                        fob = 0;
                    removechar(caddress, secondfile, fob, chartoremove, line, character);
                }
            }
        }
    }

    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, address);
        strcat(caddress, slash);
        strcat(secondfile, slash);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            getchar();
            scanf("%s", pos);
            if (strcmp(pos, "--pos") == 0)
            {
                getchar();
                scanf("%d:%d", &line, &character);
                getchar();
                scanf("%s", size);
                if (strcmp(size, "-size") == 0)
                {
                    getchar;
                    scanf("%d", &chartoremove);
                    getchar();
                    scanf("%s", forb);
                    if (strcmp(forb, "-f") == 0)
                        fob = 1;
                    else
                        fob = 0;
                    removechar(caddress, secondfile, fob, chartoremove, line, character);
                }
            }
        }
    }
}

// replace str2 in file
void replaceline(char address[], char secondstring[], char string[], int startchar, int numline)
{
    FILE *file = fopen(address, "r");
    int done = 0, count = 0;
    char tmpaddress[1000] = "/home/melika/tmp";
    FILE *tmpfile = fopen(tmpaddress, "w");
    char c, c1;
    int tmp = 0, check = 1;
    if ((numline == 1) && (startchar == 0))
    {
        fputs(string, tmpfile);
        done = 1;
    }
    c = fgetc(file);
    int n = strlen(secondstring);
    while (c != EOF)
    {
        if (c == '\n' && n >= 0)
            count++;
        else
        {
            if (numline == 1)
            {
                if (startchar > 0)
                {
                    fputc(c, tmpfile);
                    startchar -= 1;
                }
            }
            else if (count > 0)
                fputs("\n", tmpfile);
            for (int i = 1; i <= startchar; i++)
            {
                if (check != 1)
                {
                    printf("lack of chars in line");
                    return;
                }
                else
                {
                    c = fgetc(file);
                    if (c == '\n' || c == EOF)
                        check = 0;
                    if (check == 1)
                        fputc(c, tmpfile);
                }
            }
            fputs(string, tmpfile);
            done = 1;
            if (tmp == 1)
                fputc(c1, tmpfile);
            if (check == 1)
            {
                c = fgetc(file);
                while (c != EOF)
                {
                    fputc(c, tmpfile);
                    if (c == '\n')
                        break;
                    c = fgetc(file);
                }
            }
            if (check == 0)
                fputs("\n", tmpfile);
        }
        int condition = ((count == 0) && (numline - 1 == 0) && (done == 0));
        if (condition == 0)
            fputc(c, tmpfile);
        c = fgetc(file);
    }
    if (done == 0)
    {
        printf("lack of chars");
        return;
    }
    fclose(tmpfile);
    fclose(file);

    FILE *ffile = fopen(address, "w");
    FILE *tmpffile = fopen(tmpaddress, "r");
    c = fgetc(tmpffile);
    while (c != EOF)
    {
        fputc(c, ffile);
        c = fgetc(tmpffile);
    }
    printf("the string was replaced\n");
    fclose(tmpffile);
    fclose(ffile);
}

// replace inputs
void replacestr()
{
    int at, count, byword, all;
    int check, num, size = 0, numline = 1;
    char chec;
    char tmpaddress;
    char caddress[1000] = "/home/melika";
    char secondfile[2000] = "/home/melika/tempfiles";
    char address[1000] = {'\0'};
    int roww = 0;
    char slash[1000] = "/";
    char str1[1000] = {'\0'};
    char str2[1000] = {'\0'};
    char string1[1000] = {'\0'};
    char string2[1000] = {'\0'};
    char fstring1[1000] = {'\0'};
    char fstring2[1000] = {'\0'};
    char line[1000] = {'\0'};
    int row = 0, col = 0;
    char split[200][200] = {'\0'};
    char *firstoption;
    char *secondoption;
    char *thirdoption;
    int i = 0;
    getchar();
    while (1)
    {
        scanf("%s", str1);
        if (strcmp(str1, "--str2") == 0)
            break;
        else if (i == 0)
        {
            strcat(string1, str1);
            i = 1;
        }
        else
        {
            strcat(string1, " ");
            strcat(string1, str1);
        }
    }
    int n1 = strlen(string1);
    if (string1[0] == '\"')
    {
        char *p = string1;
        p++;
        p[n1 - 2] = '\0';
        strcpy(string1, p);
    }
    n1 = strlen(string1);
    int x = 0, y = 0, tmp = 0;
    while (x < n1)
    {
        if ((string1[x] == '\\') && (string1[x + 1] == 'n'))
        {
            fstring1[y] = '\n';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string1[x] == '\\') && (string1[x + 1] == '\"'))
        {
            fstring1[y] = '\"';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string1[x] == '\\') && (string1[x + 1] == '\\'))
        {
            fstring1[y] = '\\';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string1[x] == '*') && (string1[x - 1] != '\\'))
        {
            tmp = y;
            x++;
        }
        else if ((string1[x] == '\\') && (string1[x + 1] == '*'))
        {
            fstring1[y] = '*';
            y++;
            x = x + 2;
        }

        fstring1[y] = string1[x];
        y++;
        x++;
    }
    getchar();
    int j = 0;
    while (1)
    {
        scanf("%s", str2);
        if (strcmp(str2, "--file") == 0)
            break;
        else if (j == 0)
        {
            strcat(string2, str2);
            j = 1;
        }
        else
        {
            strcat(string2, " ");
            strcat(string2, str2);
        }
    }
    int n2 = strlen(string2);
    if (string2[0] == '\"')
    {
        char *q = string2;
        q++;
        q[n2 - 2] = '\0';
        strcpy(string2, q);
    }
    n2 = strlen(string2);
    x = 0, y = 0, tmp = 0;
    while (x < n2)
    {
        if ((string2[x] == '\\') && (string2[x + 1] == 'n'))
        {
            fstring2[y] = '\n';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string2[x] == '\\') && (string2[x + 1] == '\"'))
        {
            fstring2[y] = '\"';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string2[x] == '\\') && (string2[x + 1] == '\\'))
        {
            fstring2[y] = '\\';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string2[x] == '*') && (string2[x - 1] != '\\'))
        {
            tmp = y;
            x++;
            printf("the 2nd string cannot contain wildcard.\n");
            return;
        }
        else if ((string2[x] == '\\') && (string2[x + 1] == '*'))
        {
            fstring2[y] = '*';
            y++;
            x = x + 2;
        }

        fstring2[y] = string2[x];
        y++;
        x++;
    }

    getchar();
    scanf("%c", &chec);
    if (chec == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            address[roww] = tmpaddress;
            roww++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        strcat(secondfile, address);
    }
    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            address[roww] = tmpaddress;
            roww++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, address);
        strcat(caddress, slash);
        strcat(secondfile, slash);
    }
    if (existenceoffile(caddress) == 0)
    {
        if (existenceofdir(caddress) == 0)
        {
            printf(" wrong address\n");
            return;
        }
        else
        {
            printf(" wrong file\n");
            return;
        }
    }
    char ch = getchar();
    if (ch == ' ')
    {
        fgets(line, 1000, stdin);
        for (int i = 0; i <= strlen(line); i++)
        {
            if (line[i] == ' ' || line[i] == '\0' || line[i] == '\n')
            {
                split[row][col] = '\0';
                row++;
                col = 0;
            }
            else
            {
                split[row][col] = line[i];
                col++;
            }
        }
        firstoption = split[0];
        secondoption = split[1];
        thirdoption = split[2];
        if (strcmp(firstoption, "-at") == 0)
        {
            at = atoi(secondoption);
            if ((strcmp(thirdoption, "\0") != 0))
            {
                printf("invalid options\n");
                return;
            }
            else
            {
                count = 0;
                byword = 0;
                all = 0;
                check = replacefinder(caddress, fstring1, tmp, &size, &numline, at, count, byword, all);
            }
        }
        else if (strcmp(firstoption, "-all") == 0)
        {
            all = 1;
            if ((strcmp(secondoption, "\0") != 0))
            {
                printf("invalid options\n");
                return;
            }
            else
            {
                at = 1;
                count = 1;
                byword = 0;
                int charac = 0, line = 1;
                int n = replacefinder(caddress, fstring1, tmp, &charac, &line, at, count, byword, all);
                if (n < 1)
                    printf("not found\n");
                else
                {
                    while (n >= 0)
                    {
                        count = 0;
                        byword = 0;
                        removechar(caddress, secondfile, 1, at, 1, charac);
                        replaceline(caddress, fstring1, fstring2, 1, at);
                        n = replacefinder(caddress, fstring1, tmp, &charac, &line, at, count, byword, all);
                    }
                }
            }
        }
        else
        {
            printf("invalid options\n");
            return;
        }
    }
    else
    {
        at = 1;
        count = 0;
        byword = 0;
        all = 0;
        check = replacefinder(caddress, fstring1, tmp, &size, &numline, at, count, byword, all);
    }
    if (check != -1)
    {
        removechar(caddress, secondfile, 1, size, 1, check);
        replaceline(caddress, fstring1, fstring2, check, 1);
    }
    else
        printf("this string does not exist.\n");
}

// comparing two files
void pathforcompare(char address[])
{
    char check;
    char tmpaddress;
    char faddress[1000] = {'\0'};
    int row = 0;
    char caddress[1000] = "/home/melika";
    char slash[1000] = "/";
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            faddress[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, faddress);
        strcpy(address, caddress);
    }
    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            faddress[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, faddress);
        strcat(caddress, slash);
        strcpy(address, caddress);
    }
}

void compare()
{
    char address1[1000] = {'\0'};
    pathforcompare(address1);
    if (existenceoffile(address1) == 0)
    {
        if (existenceofdir(address1) == 0)
        {
            printf("wrong address\n");
            return;
        }
        else
        {
            printf("wrong file\n");
            return;
        }
    }
    getchar();
    char address2[1000] = {'\0'};
    pathforcompare(address2);
    if (existenceoffile(address2) == 0)
    {
        if (existenceofdir(address2) == 0)
        {
            printf("wrong address\n");
            return;
        }
        else
        {
            printf("wrong file\n");
            return;
        }
    }

    FILE *f1 = fopen(address1, "r");
    FILE *f2 = fopen(address2, "r");
    int numlines1 = 0, numlines2 = 0, LINES = 0, count = 0;
    char nl1[1000], nl2[1000], L1[1000], L2[1000];
    while (fgets(nl1, 1000, f1) != NULL)
    {
        numlines1++;
    }
    fclose(f1);
    while (fgets(nl2, 1000, f2) != NULL)
    {
        numlines2++;
    }
    fclose(f2);

    FILE *file1 = fopen(address1, "r");
    FILE *file2 = fopen(address2, "r");
    while (1)
    {
        if (fgets(L1, 1000, file1) == NULL)
            break;
        else if (fgets(L2, 1000, file2) == NULL)
            break;

        LINES++;
        if (strcmp(L1, L2) != 0)
        {
            printf("============ #%d ============\n", LINES);
            printf("%s", L1);
            printf("%s", L2);
            count++;
        }
    }
    fclose(file1);
    fclose(file2);

    if (count == 0)
        puts("same contents in both files");
    else if (numlines1 < numlines2)
    {
        numlines1 += 1;
        printf(">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n", numlines1, numlines2);
        char nline[1000];
        int x = 0;
        FILE *ffile2 = fopen(address2, "r");
        while (fgets(nline, 1000, ffile2) != NULL)
        {
            x += 1;
            if (x >= numlines1)
                printf("%s", nline);
        }
        fclose(ffile2);
    }
    else if (numlines1 > numlines2)
    {
        numlines2 += 1;
        printf(">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n", numlines2, numlines1);
        FILE *ffile1 = fopen(address1, "r");
        int x = 0;
        char nline[1000];
        while (fgets(nline, 1000, ffile1) != NULL)
        {
            x += 1;
            if (x >= numlines2)
                printf("%s", nline);
        }
        fclose(ffile1);
    }
}
void getaddressgrep(char address[], char filename[])
{
    char chec;
    int roww = 0;
    char tmpaddress;
    char tempname[1000] = {'\0'};
    int row = 0;
    char list[1000][1000];
    char faddress[2000] = {'\0'};
    char caddress[2000] = "/home/melika";
    char slash[2000] = "/";
    scanf("%c", &chec);
    if (chec == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            faddress[roww] = tmpaddress;
            roww++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, faddress);
        strcpy(address, caddress);
        char *token = strtok(faddress, "/");
        while (token != NULL)
        {
            strcpy(list[row], token);
            row++;
            token = strtok(NULL, "/");
        }
        strcpy(filename, list[row - 1]);
    }
    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            faddress[roww] = tmpaddress;
            roww++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, faddress);
        strcat(caddress, slash);
        strcpy(address, caddress);
        char *token = strtok(faddress, "/");
        while (token != NULL)
        {
            strcpy(list[row], token);
            row++;
            token = strtok(NULL, "/");
        }
        strcpy(filename, list[row - 1]);
    }
}

int grep(char address[], char filename[], char string[], int option, int starpos)
{
    FILE *file = fopen(address, "r");
    char ch = fgetc(file);
    int character = 0;
    int count = 0;
    int n;
    int at = 1, coun = 0, byword = 0, all = 0;
    int line = -1;
    int linecount = 1;
    int tmplline = -1;
    n = replacefinder(address, string, starpos, &character, &line, at, coun, byword, all);
    for (int i = 0; n >= 0; i++)
    {
        if (line != tmplline)
        {
            count++;
            while (ch != EOF)
            {
                if (ch == '\n')
                    linecount++;
                if (option == 0)
                {
                    if (linecount == line)
                    {
                        printf("%s: ", filename);
                        if (ch == '\n')
                            ch = fgetc(file);
                        while (ch != '\n')
                        {
                            printf("%c", ch);
                            ch = fgetc(file);
                        }
                        printf("\n");
                        break;
                    }
                }
                char ch = fgetc(file);
            }
            if (option == 2)
            {
                fclose(file);
                return 1;
            }
        }
        at += 1;
        n = replacefinder(address, string, starpos, &character, &line, at, coun, byword, all);
        tmplline = line;
    }
    fclose(file);
    return count;
}

// grep inputs
void grepstr()
{
    char option[100] = {'\0'};
    char str[100] = {'\0'};
    int op = 0, count = 0;
    char string2[2000] = {'\0'};
    char fstring2[2000] = {'\0'};
    char str2[2000] = {'\0'};
    getchar();
    scanf("%s", option);
    if (strcmp(option, "--str") == 0)
        op = 0;
    else if (strcmp(option, "-c") == 0)
        op = 1;
    else if (strcmp(option, "-l") == 0)
        op = 2;
    getchar();
    if (op == 1 || op == 2)
    {
        scanf("%s", str);
        getchar();
    }

    int j = 0;
    while (1)
    {
        scanf("%s", str2);
        if (strcmp(str2, "--files") == 0)
            break;
        else if (j == 0)
        {
            strcat(string2, str2);
            j = 1;
        }
        else
        {
            strcat(string2, " ");
            strcat(string2, str2);
        }
    }
    int n2 = strlen(string2);
    if (string2[0] == '\"')
    {
        char *q = string2;
        q++;
        q[n2 - 2] = '\0';
        strcpy(string2, q);
    }
    n2 = strlen(string2);
    int x = 0, y = 0, tmp = 0, z = 0;
    while (x < n2)
    {
        if ((string2[x] == '\\') && (string2[x + 1] == 'n'))
        {
            fstring2[y] = '\n';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string2[x] == '\\') && (string2[x + 1] == '\"'))
        {
            fstring2[y] = '\"';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string2[x] == '\\') && (string2[x + 1] == '\\'))
        {
            fstring2[y] = '\\';
            y++;
            x = x + 2;
            continue;
        }
        else if ((string2[x] == '*') && (string2[x - 1] != '\\'))
        {
            tmp = y;
            x++;
        }
        else if ((string2[x] == '\\') && (string2[x + 1] == '*'))
        {
            fstring2[y] = '*';
            y++;
            x = x + 2;
        }

        fstring2[y] = string2[x];
        y++;
        x++;
    }
    while (1)
    {

        getchar();
        char address[2000] = {'\0'};
        char filename[2000] = {'\0'};
        getaddressgrep(address, filename);
        // printf("%s", address);
        if (existenceoffile(address) == 0)
        {
            if (existenceofdir(address) == 0)
            {
                printf(" wrong address\n");
                return;
            }
            else
            {
                printf(" wrong file\n");
                return;
            }
        }
        else
        {
            int n = grep(address, filename, fstring2, op, tmp);
            if (n > 0)
                count++;
            if ((op == 2) && (n > 0))
                printf("%s", filename);
            if (n == -1)
                return;
        }
    }
    if ((op == 1) && (count != 0))
        printf("%d\n", count);
    else if (count == 0)
        printf("not found\n");
}

// CAT for showing the whole contents of file
void CAT()
{
    char check;
    char tmpaddress;
    char address[1000];
    int row = 0;
    char caddress[1000] = "/home/melika";
    char slash[1000] = "/";
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            show(caddress);
        }
    }

    else
    {
        fgets(address, 1000, stdin);
        strcat(slash, address);
        strcat(caddress, slash);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            show(caddress);
        }
    }
}

// checks file path for inserting string
void insert()
{
    char check;
    char tmpaddress;
    char address[1000] = {'\0'};
    int row = 0;
    char caddress[1000] = "/home/melika";
    char secondfile[2000] = "/home/melika/tempfiles";
    char slash[1000] = "/";
    char strcommand[100] = {'\0'};
    char string[2000] = {'\0'};
    char fstring[1000] = {'\0'};
    char str1[2000] = {'\0'};
    int line, character;
    char pos[100] = {'\0'};
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '\"')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        strcat(secondfile, address);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            scanf("%s", strcommand);
            if (strcmp(strcommand, "--str") == 0)
            {
                getchar();
                int i = 0;
                while (1)
                {
                    scanf("%s", str1);
                    if (strcmp(str1, "--pos") == 0)
                        break;
                    else if (i == 0)
                    {
                        strcat(string, str1);
                        i = 1;
                    }
                    else
                    {
                        strcat(string, " ");
                        strcat(string, str1);
                    }
                }

                getchar();
                scanf("%d:%d", &line, &character);

                int n = strlen(string);
                if (string[0] == '\"')
                {
                    char *p = string;
                    p++;
                    p[n - 2] = '\0';
                    strcpy(string, p);
                }
                int n1 = strlen(string);
                int x = 0, y = 0;
                while (x < n1)
                {
                    if ((string[x] == '\\') && (string[x + 1] == 'n'))
                    {
                        fstring[y] = '\n';
                        y++;
                        x = x + 2;
                        continue;
                    }
                    if ((string[x] == '\\') && (string[x + 1] == '\"'))
                    {
                        fstring[y] = '\"';
                        y++;
                        x = x + 2;
                        continue;
                    }
                    if ((string[x] == '\\') && (string[x + 1] == '\\'))
                    {
                        fstring[y] = '\\';
                        y++;
                        x = x + 2;
                        continue;
                    }
                    fstring[y] = string[x];
                    y++;
                    x++;
                }
                // printf("\n%s\n%d\n%d", string, line, character);
                addtofile(caddress, secondfile, line, character, fstring);
            }
        }
    }
    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, address);
        strcat(caddress, slash);
        strcat(secondfile, slash);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            scanf("%s", strcommand);
            if (strcmp(strcommand, "--str") == 0)
            {
                getchar();
                int i = 0;
                while (1)
                {
                    scanf("%s", str1);
                    if (strcmp(str1, "--pos") == 0)
                        break;
                    else if (i == 0)
                    {
                        strcat(string, str1);
                        i = 1;
                    }
                    else
                    {
                        strcat(string, " ");
                        strcat(string, str1);
                    }
                }

                getchar();
                scanf("%d:%d", &line, &character);

                int n = strlen(string);
                if (string[0] == '\"')
                {
                    char *p = string;
                    p++;
                    p[n - 2] = '\0';
                    strcpy(string, p);
                }
                int n1 = strlen(string);
                int x = 0, y = 0;
                while (x < n1)
                {
                    if ((string[x] == '\\') && (string[x + 1] == 'n'))
                    {
                        fstring[y] = '\n';
                        y++;
                        x = x + 2;
                        continue;
                    }
                    if ((string[x] == '\\') && (string[x + 1] == '\"'))
                    {
                        fstring[y] = '\"';
                        y++;
                        x = x + 2;
                        continue;
                    }
                    if ((string[x] == '\\') && (string[x + 1] == '\\'))
                    {
                        fstring[y] = '\\';
                        y++;
                        x = x + 2;
                        continue;
                    }
                    fstring[y] = string[x];
                    y++;
                    x++;
                }
                addtofile(caddress, secondfile, line, character, fstring);
            }
        }
    }
}

// path func inputs the address of file to create if not exists
void path()
{
    char check;
    char tmpaddress;
    char address[1000];
    int row = 0;
    char caddress[1000] = "/home/melika";
    char slash[1000] = "/";
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '\"')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        if (existenceoffile(caddress) == 0)
            createallfiles(caddress);
        else
            printf("File already exists\n");
    }
    else
    {
        fgets(address, 1000, stdin);
        strcat(slash, address);
        strcat(caddress, slash);
        if (existenceoffile(caddress) == 0)
            createallfiles(caddress);
        else
            printf("File already exists\n");
    }
}

// fix the indent in the given file
void fixindent(char address[], char secondfile[])
{
    char ch;
    int line = 1, character = -1, x = 0, checkindent = 0;
    FILE *file = fopen(address, "r");
    fseek(file, 0, SEEK_SET);
    char string[1000];
    fgets(string, 1000, file);
    char fstring[1000] = {'\0'};
    int n1 = strlen(string);
    int a = 0, b = 0, open = 0, tmpopen = 0, close = 0;
    if (existenceoffile(secondfile) == 0)
    {
        createallfiles(secondfile);
    }
    FILE *secfile = fopen(secondfile, "w+");
    char c2 = fgetc(file);
    while (c2 != EOF)
    {
        fputc(c2, secfile);
        c2 = fgetc(file);
    }
    fclose(secfile);
    fseek(file, 0, SEEK_SET);
    while (a < n1)
    {
        if ((string[a] == '{'))
        {
            fstring[b] = '{';
            b++;
            a++;
            fstring[b++] = '\n';
            if (string[a] == ' ')
            {
                a++;
            }
            if (checkindent > 0 || tmpopen == open)
            {
                for (int i = 0; i <= open; i++)
                {
                    fstring[b] = ' ';
                    b++;
                    fstring[b] = ' ';
                    b++;
                    fstring[b] = ' ';
                    b++;
                    fstring[b] = ' ';
                    b++;
                }
            }
            open++;
            checkindent++;
            tmpopen++;
            continue;
        }
        if ((string[a] == '}'))
        {
            a++;
            open--;
            checkindent--;
            for (int i = 0; i < checkindent; i++)
            {
                fstring[b] = ' ';
                b++;
                fstring[b] = ' ';
                b++;
                fstring[b] = ' ';
                b++;
                fstring[b] = ' ';
                b++;
            }
            fstring[b] = '}';
            b++;
            if (open != 0)
            {
                fstring[b] = '\n';
                b++;
                if (string[a] == ' ')
                {
                    a++;
                }
                continue;
            }
        }

        fstring[b] = string[a];
        b++;
        a++;
        if (string[a] == '}')
        {
            fstring[b] = '\n';
            b++;
        }
    }
    fclose(file);
    FILE *ffile = fopen(address, "w");
    fputs(fstring, ffile);
    fclose(ffile);
}

// closing pairs
void indent()
{
    char check;
    char tmpaddress;
    char address[100];
    int row = 0;
    char caddress[100] = "/home/melika";
    char secondfile[2000] = "/home/melika/tempfiles";
    char slash[100] = "/";
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, address);
        strcat(secondfile, address);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            fixindent(caddress, secondfile);
        }
    }

    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ')
        {
            address[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, address);
        strcat(caddress, slash);
        strcat(secondfile, slash);
        if (existenceoffile(caddress) == 0)
        {
            if (existenceofdir(caddress) == 0)
                printf("wrong address\n");
            else
                printf("wrong file\n");
        }
        else
        {
            fixindent(caddress, secondfile);
        }
    }
}

// undo inputs
void undo()
{
    char check;
    char tmpaddress;
    char secondfile[2000] = "/home/melika/tempfiles";
    char tempaddress[2000] = "/home/melika/tempfiles/root/g";
    char faddress[2000] = {'\0'};
    int row = 0;
    char caddress[2000] = "/home/melika";
    char slash[2000] = "/";
    scanf(" %c", &check);
    if (check == '\"')
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != '"')
        {
            faddress[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(caddress, faddress);
        strcat(secondfile, faddress);
    }
    else
    {
        scanf("%c", &tmpaddress);
        while (tmpaddress != ' ' && tmpaddress != '\n')
        {
            faddress[row] = tmpaddress;
            row++;
            scanf("%c", &tmpaddress);
        }
        strcat(slash, faddress);
        strcat(caddress, slash);
        strcat(secondfile, slash);
    }
    if (existenceoffile(caddress) == 0)
    {
        if (existenceofdir(caddress) == 0)
        {
            printf("wrong address\n");
            return;
        }
        else
        {
            printf("wrong file\n");
            return;
        }
    }
    else
    {
        // rename(secondfile, tempaddress);
        // rename(caddress, secondfile);
        // rename(tempaddress, caddress);
        FILE *ff = fopen(secondfile, "r");
        FILE *f = fopen(tempaddress, "w");
        fseek(ff, 0, SEEK_SET);
        char c = fgetc(ff);
        while (c != EOF)
        {
            fputc(c, f);
            c = fgetc(ff);
        }
        fclose(f);
        fclose(ff);
        FILE *file = fopen(caddress, "r");
        FILE *ffile = fopen(secondfile, "w");
        fseek(file, 0, SEEK_SET);
        c = fgetc(file);
        while (c != EOF)
        {
            fputc(c, file);
            c = fgetc(ffile);
        }
        fclose(file);
        fclose(ffile);
        FILE *cfile = fopen(caddress, "w");
        FILE *ccfile = fopen(tempaddress, "r");
        fseek(ccfile, 0, SEEK_SET);
        c = fgetc(ccfile);
        while (c != EOF)
        {
            fputc(c, cfile);
            c = fgetc(ccfile);
        }
        fclose(cfile);
        fclose(ccfile);
    }
}

// inputing the commands and passing to functions
int main()
{
    char first[100];
    char second[100];
    scanf("%s", first);
    while (strcmp(first, "exit") != 0)
    {
        if (strcmp(first, "createfile") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--file") == 0)
            {
                path();
            }
        }
        else if (strcmp(first, "insertstr") == 0)
        {
            // insertstr --file "/root/a 1/c.txt" --str salam --pos 2:1
            scanf("%s", second);
            if (strcmp(second, "--file") == 0)
            {
                insert();
            }
        }
        else if (strcmp(first, "cat") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--file") == 0)
            {
                CAT();
            }
        }
        else if (strcmp(first, "tree") == 0)
        {
            int depth;
            int x = 0;
            char address[] = "/home/melika/root";
            scanf("%d", &depth);
            if (depth < -1)
                printf("invalid depth\n");
            else
            {
                depth = depth - 1;
                treecommand(x, address, depth);
            }
        }
        else if (strcmp(first, "removestr") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--file") == 0)
            {
                removestr();
            }
        }
        else if (strcmp(first, "copystr") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--file") == 0)
            {
                copystr();
            }
        }
        else if (strcmp(first, "cutstr") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--file") == 0)
            {
                cutstr();
            }
        }
        else if (strcmp(first, "pastestr") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--file") == 0)
            {
                pastestr();
            }
        }
        else if (strcmp(first, "find") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--str") == 0)
            {
                findstr();
            }
        }
        else if (strcmp(first, "replace") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--str1") == 0)
            {
                replacestr();
            }
        }
        else if (strcmp(first, "grep") == 0)
        {
            grepstr();
        }
        else if (strcmp(first, "undo") == 0)
        {
            scanf("%s", second);
            if (strcmp(second, "--file") == 0)
            {
                undo();
            }
        }
        else if (strcmp(first, "auto-indent") == 0)
        {
            indent();
        }
        else if (strcmp(first, "compare") == 0)
        {
            getchar();
            compare();
        }
        else
        {
            printf("invalid command\n");
        }

        scanf("%s", first);
    }
}
