#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>
#include<ctype.h>

typedef struct Bookmark {
    char *word;
    struct Bookmark *next, *prev;
} Bookmark;

bool isDuplicate(Bookmark *head, const char *word) {
    if (!head) return 0;

    Bookmark *curr = head;
    do {
        if (strcmp(curr->word, word) == 0) {
            return true;
        }
        curr = curr->next;
    } while (curr != head);

    return false;
}

void insertBookmark(Bookmark **head, const char *word) {
    if (isDuplicate(*head, word)) {
        printf("Bookmark '%s' sudah ada!\n", word);
        return;
    }

    Bookmark *newNode = (Bookmark *)malloc(sizeof(Bookmark));
    if (!newNode) return;

    newNode->word = (char *)malloc(strlen(word) + 1);
    if (!newNode->word) {
        free(newNode);
        return;
    }
    strcpy(newNode->word, word);

    if (!*head) {
        newNode->next = newNode->prev = newNode;
        *head = newNode;
    } else {
        Bookmark *tail = (*head)->prev;
        newNode->next = *head;
        newNode->prev = tail;
        tail->next = newNode;
        (*head)->prev = newNode;
    }
}

Bookmark *loadBookmarks(const char *username) {
    char path[100];
    sprintf(path, "userBookmark/bookmark_%s.txt", username);

    FILE *file = fopen(path, "r");
    if (!file) return NULL;

    Bookmark *head = NULL;
    char word[200];

    while (fscanf(file, "%[^\n]\n", word) != EOF) {
        insertBookmark(&head, word);
    }

    fclose(file);
    return head;
}

void saveBookmarksToFile(const char *username, Bookmark *head) {
    char path[100];
    sprintf(path, "userBookmark/bookmark_%s.txt", username);

    FILE *file = fopen(path, "w");
    if (!file) {
        printf("Gagal menyimpan bookmark ke file!\n");
        return;
    }

    if (head) {
        Bookmark *curr = head;
        do {
            fprintf(file, "%s\n", curr->word);
            curr = curr->next;
        } while (curr != head);
    }

    fclose(file);
}

void addBookmark(Bookmark **head, const char *indonesia, const char *english) {
    char combined[200];
    sprintf(combined, "%s#%s", indonesia, english);
    insertBookmark(head, combined);
}

void displayBookmarks(Bookmark *head) {
    if (!head) {
        printf("\nBelum ada bookmark.\n");
        return;
    }

    printf("\n========== Daftar Bookmark ==========\n");
    printf("%-2s %-20s %s", "No.", "Indonesia", "Inggris");    
    printf("\n-------------------------------------\n");
    Bookmark *curr = head;
    int i = 1;
    do {
        char buffer[200];
        strcpy(buffer, curr->word);
        char *indonesia = strtok(buffer, "#");
        char *english = strtok(NULL, "#");

        if (indonesia && english) printf("%.2d. %-20s  %s\n", i++, indonesia, english);

        curr = curr->next;
    } while (curr != head);
    printf("=====================================\n");
}

void deleteBookmark(const char *username, Bookmark **head) {
    if (!*head) {
        printf("Tidak ada bookmark untuk dihapus.\n");
        return;
    }

    int total = 0;
    Bookmark *curr = *head;
    do {
        total++;
        curr = curr->next;
    } while (curr != *head);

    displayBookmarks(*head);

    printf("Masukkan nomor bookmark yang ingin dihapus (-1 untuk batal): ");
    int choice;
    scanf("%d", &choice);
    if (choice == -1) {
    	system("cls");
        printf("Penghapusan dibatalkan.\n");
        return;
    }

    if (choice < 1 || choice > total) {
    	system("cls");
        printf("Nomor bookmark tidak valid.\n");
        return;
    }

    curr = *head;
    int i;
    for (i = 1; i < choice; i++) {
        curr = curr->next;
    }

    printf("\nBookmark yang dipilih: %s\n", curr->word);
    printf("Yakin ingin menghapus bookmark ini? [y/n]: ");
    char confirm;
    scanf(" %c", &confirm);
    if (tolower(confirm) != 'y') {
    	system("cls");
        printf("Penghapusan dibatalkan.\n");
        return;
    }

    if (curr->next == curr && curr->prev == curr) {
        free(curr->word);
        free(curr);
        *head = NULL;
    } else {
        if (curr == *head) {
            *head = curr->next;
        }
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
        free(curr->word);
        free(curr);
    }

    saveBookmarksToFile(username, *head);
    system("cls");
    printf("Bookmark berhasil dihapus.\n");
}

void freeBookmarks(Bookmark *head) {
    if (!head) return;

    Bookmark *curr = head->next;
    while (curr != head) {
        Bookmark *temp = curr;
        curr = curr->next;
        free(temp->word);
        free(temp);
    }
    free(head->word);
    free(head);
}


