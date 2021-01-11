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
    printf("\n$$$$$$$[ ДОБРО ПОЖАЛОВАТЬ В КАЗИНО ]$$$$$$$\n");
    printf("\n");

    if (get_player_data() == -1)
        register_new_player();

    while (choice != 7) {
        printf("-=[ Меню игр ]=-\n");
        printf("1 - Игра Угадай число\n");
        printf("2 - Игра Без совпадений с крупье\n");
        printf("3 - Игра Найди туза\n");
        printf("4 - Текущий рекорд\n");
        printf("5 - Сменить имя\n");
        printf("6 - Вернуть учетную запись к 100 кредитам\n");
        printf("7 - Выход\n");
        printf("[Имя: %s]\n", player.name);
        printf("[У вас %u очков] -> ",player.credits);
        scanf("%d", &choice);

        if ((choice < 1) || (choice > 7))
            printf("\n[!!] Число %d недопустимо.\n\n", choice);
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
            printf("\nСмена имени\n");
            printf("Укажите новое имя: ");
            input_name();
            printf("Имя пользователя изменено.\n\n");
        }
        else if (choice == 6) {
            printf("\nВаш счет возвращен к 100 кредитам.\n\n");
            player.credits = 100;
        }
    }
    update_player_data();
    printf("\nСпасибо за игру! Пока.\n");

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
    printf(".=.={ Регистрация нового игрока }=-=-\n");
    printf("Введите своё имя: ");
    input_name();

    player.highscore = player.credits = 100;

    fd = open(DATAFILE, O_WRONLY|O_CREAT|O_APPEND);
    if (fd == -1)
        fatal("в функции register_new_player() при открытии файла.");
    write(fd, &player, sizeof(struct user));
    close(fd);

    printf("\nДобро пожаловать в игру %s.\n", player.name);
    printf("Вам выдано %u кредитов.\n", player.credits);
}

void update_player_data() {
    int fd, i;
    char burned_byte;

    fd = open(DATAFILE, O_RDWR);
    if (fd == -1)
        fatal("в функции update_player_data() при открытии файла");
    write(fd, &(player.credits), 4);
    write(fd, &(player.highscore), 4);
    write(fd, &(player.name), 100);
    close(fd);
}

void show_highscore() {

    printf("\n====================| РЕКОРД |====================\n");

    if (player.credits < player.highscore)
        printf("Вы установили рекорд %u\n", player.highscore);
    else
        printf("Сейчас у вас рекордные %u кредитов!\n", player.highscore);
    printf("\n");
}

void jackpot() {
    printf("*+*+*+*+*+* ДЖЕКПОТ *+*+*+*+*+*\n");
    printf("Вы выиграли джекпот в 100 кредитов!\n");
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
    printf("Карты:\t|%c|\t|%c|\t|%c|\n\t", cards[0], cards[1], cards[2]);
    if (user_pick == -1)
        printf(" 1 \t 2 \t 3\n");
    else {
        for (int i = 0; i < user_pick; i++)
            printf("\t");
        printf(" ^-- вы выбрали\n");
    }
}

int take_wager(int available_credits, int previous_wager) {
    int wager, total_wager;
    printf("Сколько из ваших %d кредитов вы хотите поставить?  ", available_credits);
    scanf("%d", &wager);
    if (wager < 1) {
        printf("Ставка должна быть положительным числом!\n");
        return -1;
    }
    total_wager = previous_wager + wager;
    if (total_wager > available_credits) {
        printf("Вы поставили %d больше, чем имеете!\n", total_wager);
        printf("У вас всего %d кредитов, повторите попытку.\n", available_credits);
        return -1;
    }
    return wager;
}

void play_the_game() {
    int play_again = 1;
    int (*game) ();
    char selection;

    while (play_again) {
        printf("\n[DEBUG] указатель current_game @ 0x%08x\n", player.current_game);
        if (player.current_game() != -1) {
            if (player.credits > player.highscore)
                player.highscore = player.credits;
            printf("Теперь у вас %u кредитов\n", player.credits);
            update_player_data();

            printf("Хотите сыграть еще раз? (y/n) ");
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

    printf("\n####### Выбери число ######\n");
    printf("Эта игра стоит 10 кредитов. Просто выберите число\n");
    printf("от 1 до 20, и если вы угадаете\n");
    printf("то выиграете джекпот в 100 кредитов!\n\n");
    winning_number = (rand() % 20) + 1;
    if (player.credits < 10) {
        printf("У вас всего %d кредитов. Этого недостаточно для игры!\n\n", player.credits);
        return -1;
    }
    player.credits -= 10;
    printf("С ващешл счета были списаны 10 кредитов.\n");
    printf("Выберите число от 1 до 20: ");
    scanf("%d", &pick);

    printf("Выигрышное число %d\n", winning_number);
    if (pick == winning_number)
        jackpot();
    else
        printf("К сожалению, вы проиграли.\n");
    return 0;
}

int dealer_no_match() {
    int i, j, numbers[16],wager = -1, match = -1;

    printf("\n::::::: Без совпадений :::::::\n");
    printf("В этой игре можно поставить все свои кредиты.\n");
    printf("Крупье выбирает 16 случайных чисел от 0 до 99.\n");
    printf("Если все они будут разными, то вам вернется удвоенная ставка!\n\n");

    if (player.credits == 0) {
        printf("У вас нет кредитов, чтобы сделать ставку!\n\n");
        return -1;
    }
    while (wager == -1)
        wager = take_wager(player.credits, 0);
    printf("\t\t::: Выбираем 16 случайных чисел :::\n");
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
        printf("Совпало число %d!\n", match);
        printf("Вы потеряли %d кредитов.\n", wager);
        player.credits -= wager;
    } else {
        printf("Совпадений нет! Вы выиграли %d кредитов!\n", wager);
        player.credits += wager;
    }
    return 0;
}

int find_the_ace() {
    int i, ace, total_wager;
    int invalid_choice, pick = -1, wager_one = -1, wager_two = -1;
    char choice_two, cards[3] = {'X', 'X', 'X'};
    ace = rand() % 3;
    printf("******* Найди туза *******\n");
    printf("В этой игре можно поставить все свои кредиты.\n");
    printf("Выберем три карты, двух дам и одного туза.\n");
    printf("Угадайте, где туз и вы выиграете ставку.\n");
    printf("После выбора карты открывается одна из дам.\n");
    printf("После этого можно или выбрать другую карту, или\n");
    printf("увеличить ставку.\n\n");
    if (player.credits == 0) {
        printf("У вас нет кредитов, чтобы сделать ставку!\n\n");
        return -1;
    }

    while (wager_one == -1)
        wager_one = take_wager(player.credits, 0);

    print_cards("Раздаем карты", cards, -1);
    pick = -1;
    while ((pick < 1) || (pick > 3)) {
        printf("Выберите карту: 1, 2 или 3: ");
        scanf("%d", &pick);
    }
    pick--;
    i = 0;
    while (i == ace || i == pick)
        i++;
    cards[i] = 'Q';
    print_cards("Открываем даму", cards, pick);
    invalid_choice = 1;
    while (invalid_choice) {
        printf("Хотите:\n[1]выбрать другую карту\t    или\t     [2]увеличить ставку?\n");
        printf("Выберите в или у: ");
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
            printf("Вы поменяли свой выбор на карту %d\n", pick + 1);
        }
    }

    for (i = 0; i < 3; i++) {
        if (ace == i)
            cards[i] = 'A';
        else
            cards[i] = 'Q';
    }
    print_cards("Результат", cards, pick);

    if (pick == ace) {
        printf("Первая ставка принесла вам выигрыш в %d кредитов\n", wager_one);
        player.credits += wager_one;
        if (wager_two != -1) {
            printf("а вторая ставка дополнительный выигрышь в %d кредитов!\n", wager_two);
            player.credits += wager_two;
        }
    } else {
        printf("Вы потеряли %d кредитов на вашей первой ставке\n", wager_one);
        player.credits -= wager_one;
        if (wager_two != -1) {
            printf("и дополнительные %d кредитов на вашей второй ставке!\n", wager_two);
            player.credits -= wager_two;
        }
    }
    return 0;
}
