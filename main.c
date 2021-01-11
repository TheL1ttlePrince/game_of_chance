#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include "../../CPP/cpp headers/hacking.h"

#define DATAFILE "D:\Programming\Programs\Projects\gameHacking\game.data"

struct user {
    int credits;
    int highscore;
    char name[100];
    int (*current_game) ();
};

int get_player_data();
void update_player_data();
void show_highscore();
void jackpot();
void input_name();
void print_cards(char *, char *, int);
int take_wager(int, int);
void play_the_game();
int pick_a_number();
int dealer_no_match();
int find_the_ace();

struct user player;

int main() {
    setlocale(LC_ALL, "Rus");

    int choice, last_game;

    srand(time(0));
    printf("\n$$$$$$$[ ����� ���������� � ������ ]$$$$$$$\n");
    printf("\n");

    if (get_player_data() == -1)
        register_new_player();

    while (choice != 7) {
        printf("-=[ ���� ��� ]=-\n");
        printf("1 - ���� ������ �����\n");
        printf("2 - ���� ��� ���������� � ������\n");
        printf("3 - ���� ����� ����\n");
        printf("4 - ������� ������\n");
        printf("5 - ������� ���\n");
        printf("6 - ������� ������� ������ � 100 ��������\n");
        printf("7 - �����\n");
        printf("[���: %s]\n", player.name);
        printf("[� ��� %u �����] -> ",player.credits);
        scanf("%d", &choice);

        if ((choice < 1) || (choice > 7))
            printf("\n[!!] ����� %d �����������.\n\n", choice);
        else if (choice < 4) {
            if (choice != last_game) {
                if (choice == 1)
                    player.current_game = pick_a_number;
                else if (choice == 2)
                    player.current_game = dealer_no_match;
                else
                    player.current_game = find_the_ace;
                last_game = choice;
            }
            play_the_game();
        }
        else if (choice == 4)
            show_highscore();
        else if (choice == 5) {
            printf("\n����� �����\n");
            printf("������� ����� ���: ");
            input_name();
            printf("��� ������������ ��������.\n\n");
        }
        else if (choice == 6) {
            printf("\n��� ���� ��������� � 100 ��������.\n\n");
            player.credits = 100;
        }
    }
    update_player_data();
    printf("\n������� �� ����! ����.\n");

    return 0;
}

int get_player_data() {
    int fd, read_bytes;
    struct user entry;

    fd = open(DATAFILE, O_RDONLY);
    if (fd == -1)
            return -1;
    read_bytes = read(fd, &entry, sizeof(struct user));
    close(fd);
    if (read_bytes < sizeof(struct user))
        return -1;
    else
        player = entry;
    return 1;
}

void register_new_player() {
    int fd;
    printf(".=.={ ����������� ������ ������ }=-=-\n");
    printf("������� ��� ���: ");
    input_name();

    player.highscore = player.credits = 100;

    fd = open(DATAFILE, O_WRONLY|O_CREAT|O_APPEND);
    if (fd == -1)
        fatal("� ������� register_new_player() ��� �������� �����.");
    write(fd, &player, sizeof(struct user));
    close(fd);

    printf("\n����� ���������� � ���� %s.\n", player.name);
    printf("��� ������ %u ��������.\n", player.credits);
}

void update_player_data() {
    int fd, i;
    char burned_byte;

    fd = open(DATAFILE, O_RDWR);
    if (fd == -1)
        fatal("� ������� update_player_data() ��� �������� �����");
    write(fd, &(player.credits), 4);
    write(fd, &(player.highscore), 4);
    write(fd, &(player.name), 100);
    close(fd);
}

void show_highscore() {

    printf("\n====================| ������ |====================\n");

    if (player.credits < player.highscore)
        printf("�� ���������� ������ %u\n", player.highscore);
    else
        printf("������ � ��� ��������� %u ��������!\n", player.highscore);
    printf("\n");
}

void jackpot() {
    printf("*+*+*+*+*+* ������� *+*+*+*+*+*\n");
    printf("�� �������� ������� � 100 ��������!\n");
    player.credits += 100;
}

void input_name() {
    char *name_ptr, input_char = '\n';
    while (input_char == '\n')
        scanf("%c", &input_char);

    name_ptr = (char *) &(player.name);
    while (input_char != '\n') {
        *name_ptr = input_char;
        scanf("%c", &input_char);
        name_ptr++;
    }
    *name_ptr = 0;
}

void print_cards(char *message, char *cards, int user_pick) {
    printf("\n\t*** %s ***\n", message);
    printf("      \t._.\t._.\t._.\n");
    printf("�����:\t|%c|\t|%c|\t|%c|\n\t", cards[0], cards[1], cards[2]);
    if (user_pick == -1)
        printf(" 1 \t 2 \t 3\n");
    else {
        for (int i = 0; i < user_pick; i++)
            printf("\t");
        printf(" ^-- �� �������\n");
    }
}

int take_wager(int available_credits, int previous_wager) {
    int wager, total_wager;
    printf("������� �� ����� %d �������� �� ������ ���������?  ", available_credits);
    scanf("%d", &wager);
    if (wager < 1) {
        printf("������ ������ ���� ������������� ������!\n");
        return -1;
    }
    total_wager = previous_wager + wager;
    if (total_wager > available_credits) {
        printf("�� ��������� %d ������, ��� ������!\n", total_wager);
        printf("� ��� ����� %d ��������, ��������� �������.\n", available_credits);
        return -1;
    }
    return wager;
}

void play_the_game() {
    int play_again = 1;
    int (*game) ();
    char selection;

    while (play_again) {
        printf("\n[DEBUG] ��������� current_game @ 0x%08x\n", player.current_game);
        if (player.current_game() != -1) {
            if (player.credits > player.highscore)
                player.highscore = player.credits;
            printf("������ � ��� %u ��������\n", player.credits);
            update_player_data();

            printf("������ ������� ��� ���? (y/n) ");
            selection = '\n';
            while (selection == '\n')
                scanf("%c", &selection);
            if (selection == 'n')
                play_again = 0;
        }
        else
            play_again = 0;
    }
}

int pick_a_number() {
    int pick, winning_number;

    printf("\n####### ������ ����� ######\n");
    printf("��� ���� ����� 10 ��������. ������ �������� �����\n");
    printf("�� 1 �� 20, � ���� �� ��������\n");
    printf("�� ��������� ������� � 100 ��������!\n\n");
    winning_number = (rand() % 20) + 1;
    if (player.credits < 10) {
        printf("� ��� ����� %d ��������. ����� ������������ ��� ����!\n\n", player.credits);
        return -1;
    }
    player.credits -= 10;
    printf("� ������ ����� ���� ������� 10 ��������.\n");
    printf("�������� ����� �� 1 �� 20: ");
    scanf("%d", &pick);

    printf("���������� ����� %d\n", winning_number);
    if (pick == winning_number)
        jackpot();
    else
        printf("� ���������, �� ���������.\n");
    return 0;
}

int dealer_no_match() {
    int i, j, numbers[16],wager = -1, match = -1;

    printf("\n::::::: ��� ���������� :::::::\n");
    printf("� ���� ���� ����� ��������� ��� ���� �������.\n");
    printf("������ �������� 16 ��������� ����� �� 0 �� 99.\n");
    printf("���� ��� ��� ����� �������, �� ��� �������� ��������� ������!\n\n");

    if (player.credits == 0) {
        printf("� ��� ��� ��������, ����� ������� ������!\n\n");
        return -1;
    }
    while (wager == -1)
        wager = take_wager(player.credits, 0);
    printf("\t\t::: �������� 16 ��������� ����� :::\n");
    for (i = 0; i < 16; i++) {
        numbers[i] = rand() % 100;
        printf("%2d\t", numbers[i]);
        if (i % 8 == 7)
            printf("\n");
    }
    for (i = 0; i < 15; i++) {
        j = i + 1;
        while (j < 16) {
            if (numbers[i] == numbers[j])
                match = numbers[i];
            j++;
        }
    }
    if (match != -1) {
        printf("������� ����� %d!\n", match);
        printf("�� �������� %d ��������.\n", wager);
        player.credits -= wager;
    } else {
        printf("���������� ���! �� �������� %d ��������!\n", wager);
        player.credits += wager;
    }
    return 0;
}

int find_the_ace() {
    int i, ace, total_wager;
    int invalid_choice, pick = -1, wager_one = -1, wager_two = -1;
    char choice_two, cards[3] = {'X', 'X', 'X'};
    ace = rand() % 3;
    printf("******* ����� ���� *******\n");
    printf("� ���� ���� ����� ��������� ��� ���� �������.\n");
    printf("������� ��� �����, ���� ��� � ������ ����.\n");
    printf("��������, ��� ��� � �� ��������� ������.\n");
    printf("����� ������ ����� ����������� ���� �� ���.\n");
    printf("����� ����� ����� ��� ������� ������ �����, ���\n");
    printf("��������� ������.\n\n");
    if (player.credits == 0) {
        printf("� ��� ��� ��������, ����� ������� ������!\n\n");
        return -1;
    }

    while (wager_one == -1)
        wager_one = take_wager(player.credits, 0);

    print_cards("������� �����", cards, -1);
    pick = -1;
    while ((pick < 1) || (pick > 3)) {
        printf("�������� �����: 1, 2 ��� 3: ");
        scanf("%d", &pick);
    }
    pick--;
    i = 0;
    while (i == ace || i == pick)
        i++;
    cards[i] = 'Q';
    print_cards("��������� ����", cards, pick);
    invalid_choice = 1;
    while (invalid_choice) {
        printf("������:\n[1]������� ������ �����\t    ���\t     [2]��������� ������?\n");
        printf("�������� � ��� �: ");
        choice_two = '\n';
        while (choice_two == '\n')
            scanf("%c", &choice_two);
            printf("%c\n", choice_two);
        if (choice_two == '2') {
            invalid_choice = 0;
            while (wager_two == -1)
                wager_two = take_wager(player.credits, wager_one);
        }
        if (choice_two == '1') {
            i = invalid_choice = 0;
            while (i == pick || cards[i] == 'Q')
                i++;
            pick = i;
            printf("�� �������� ���� ����� �� ����� %d\n", pick + 1);
        }
    }

    for (i = 0; i < 3; i++) {
        if (ace == i)
            cards[i] = 'A';
        else
            cards[i] = 'Q';
    }
    print_cards("���������", cards, pick);

    if (pick == ace) {
        printf("������ ������ �������� ��� ������� � %d ��������\n", wager_one);
        player.credits += wager_one;
        if (wager_two != -1) {
            printf("� ������ ������ �������������� �������� � %d ��������!\n", wager_two);
            player.credits += wager_two;
        }
    } else {
        printf("�� �������� %d �������� �� ����� ������ ������\n", wager_one);
        player.credits -= wager_one;
        if (wager_two != -1) {
            printf("� �������������� %d �������� �� ����� ������ ������!\n", wager_two);
            player.credits -= wager_two;
        }
    }
    return 0;
}
