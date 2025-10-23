#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>
#include<windows.h>
#include"strlowcmp.h"
#include"bookmark.h"

#ifndef max
#define max(a, b) ((a > b) ? (a) : (b))
#endif

typedef struct User {
    char username[50];
    char password[50];
    bool isAdmin;
} User;


typedef struct Node{
	char *indonesia;
	char *english;
	int height;
	struct Node *left, *right;
} Node;

typedef struct Hashmap{
	struct Node **table;
	int size;
} Hashmap;

typedef enum Dictionary{ // Memberi penjelasan kamus hash map yang mana (indo -> inggris) atau (inggris -> indo)
	IndoToEng,
	EngToIndo
} Dictionary;

void welcome(){
	printf("--------------------------------------\n");
	printf("| Kamus bahasa Indonesia dan Inggris |\n");
	printf("--------------------------------------\n");
}

int hash(const char *word){ // Fungsi hash berdasarkan abjad (a - z)
	return (tolower(word[0]) - 'a') % 26;
}

Hashmap *initializeMap(){ // Inisialisasi hash map
	Hashmap *newMap = (Hashmap *)malloc(sizeof(Hashmap));
	newMap->size = 0;
	newMap->table = (Node **)malloc(26 * sizeof(Node *));

	int i;
	for(i = 0; i < 26; i++){
		newMap->table[i] = NULL;
	}

	return newMap;
}

int getHeight(struct Node* n) // Mendapatkan ketinggian node
{
    if (n == NULL) return 0;
    return n->height;
}

int getBalanceFactor(struct Node* n) // Mencari balance factor node (ketinggian kanan - ketinggian kiri)
{
    if (n == NULL) return 0;
    return getHeight(n->left) - getHeight(n->right);
}

struct Node* rightRotate(struct Node* y) // Kondisi berat ke kiri (balance factor > 1)
{
    struct Node* x = y->left;
    struct Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

struct Node* leftRotate(struct Node* x) // Kondisi berat ke kanan (balance factor < -1)
{
    struct Node* y = x->right;
    struct Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

Node *createNode(const char *indonesia, const char *english){ // Buat node saat ingin menambahkan data
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->indonesia = strdup(indonesia);
	newNode->english = strdup(english);
	newNode->left = newNode->right = NULL;
	newNode->height = 1;

	return newNode;
}


Node *searchWord(Node *root, const char *word, Dictionary dictionary){ // Mencari dictionary dari pohon biner sesuai abjad (sudah dihash) || searchWord(IndoEng->table[hash(word)], word, IndoToEng); ||
	Node *curr = root;

	while(curr != NULL){
		int compare = (dictionary == IndoToEng) ? strlowcmp(curr->indonesia, word) : strlowcmp(curr->english, word);

		if(compare == 0) return curr;
		else if(compare < 0) curr = curr->right;
		else curr = curr->left;
	}
	return NULL;
}

void search(Hashmap *IndoEng, Hashmap *EngIndo, Bookmark **bookmark){ // Tampilan pencarian
	int opt;
	char searchAgain;
	do{
		char word[100];
		printf("\n1. Cari terjemahan dari Indonesia\n");
		printf("2. Cari terjemahan dari Inggris\n");
		printf("3. Keluar pencarian\n");
		printf(">> ");
		scanf("%d", &opt); getchar();

		if(opt == 1){
			printf("\nMasukkan kosakata yang ingin dicari: ");
			scanf("%[^\n]", word); getchar();
			Node *translation = searchWord(IndoEng->table[hash(word)], word, IndoToEng);
			if(translation == NULL){
				printf("Terjemahan tidak ditemukan!\n");
			} else{
				printf("Terjemahan Inggris dari { \033[0;32m%s\033[0m } adalah { \033[0;32m%s\033[0m }\n\n", translation->indonesia, translation->english);

				char saveBookmark;
				printf("Tambah kata ini ke bookmark? [y/n]: ");
				scanf(" %c", &saveBookmark); getchar();

				if(tolower(saveBookmark) == 'y'){
					addBookmark(bookmark, translation->indonesia, translation->english);
					printf("Kata berhasil ditambahkan ke bookmark!\n");
				}
			}
		} else if(opt == 2){
			printf("\nMasukkan kosakata yang ingin dicari: ");
			scanf("%[^\n]", word); getchar();
			Node *translation = searchWord(EngIndo->table[hash(word)], word, EngToIndo);
			if(translation == NULL){
				printf("Terjemahan tidak ditemukan!\n");
			} else{
				printf("Terjemahan Indonesia dari { \033[0;32m%s\033[0m } adalah { \033[0;32m%s\033[0m }\n\n", translation->english, translation->indonesia);

				char saveBookmark;
				printf("Tambah kata ini ke bookmark? [y/n]: ");
				scanf(" %c", &saveBookmark); getchar();

				if(tolower(saveBookmark) == 'y'){
					addBookmark(bookmark, translation->indonesia, translation->english);
					printf("Kata berhasil ditambahkan ke bookmark!\n");
				}
			}
		} else if(opt == 3){
			system("cls");
			return;
		} else{
			system("cls");
			printf("Tolong masukkan input dengan benar [1 - 3]\n\n");
			continue;
		}

		printf("Apakah masih ingin mencari? [y/n]: ");
		scanf("%c", &searchAgain);
		system("cls");

	} while(tolower(searchAgain) == 'y');
}

Node *insertAVL(Node *root, const char *indonesia, const char *english, Dictionary dictionary){ // Insertion data ke avl tree (balanced binary tree)
    if (root == NULL) return createNode(indonesia, english);

    int cmp = (dictionary == IndoToEng) ? strlowcmp(indonesia, root->indonesia) : strlowcmp(english, root->english);

    if (cmp < 0)
        root->left = insertAVL(root->left, indonesia, english, dictionary);
    else if (cmp > 0)
        root->right = insertAVL(root->right, indonesia, english, dictionary);
    else
        return root;

    root->height = 1 + max(getHeight(root->left), getHeight(root->right));

    int balance = getBalanceFactor(root);

    if(balance > 1 && ((dictionary == IndoToEng ? strlowcmp(indonesia, root->left->indonesia) : strlowcmp(english, root->left->english)) < 0)) // Kondisi left-left
        return rightRotate(root);

    if(balance < -1 && ((dictionary == IndoToEng ? strlowcmp(indonesia, root->right->indonesia) : strlowcmp(english, root->right->english)) > 0)) // Kondisi right-right
        return leftRotate(root);

    if(balance > 1 && ((dictionary == IndoToEng ? strlowcmp(indonesia, root->left->indonesia) : strlowcmp(english, root->left->english)) > 0)){ // Kondisi left-right
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if(balance < -1 && ((dictionary == IndoToEng ? strlowcmp(indonesia, root->right->indonesia) : strlowcmp(english, root->right->english)) < 0)){// Kondisi right-left
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

void insertWord(Hashmap *map, const char *indonesia, const char *english, Dictionary dictionary) { // fungsi insert dictionary (beda fungsi supaya avl dapat melakukan secara rekursif)
    int index = (dictionary == IndoToEng) ? hash(indonesia) : hash(english);
    map->table[index] = insertAVL(map->table[index], indonesia, english, dictionary);
    map->size++;
}


void insert(Hashmap *IndoEng, Hashmap *EngIndo, const char *indonesia, const char *english){ // Memasukkan dictionary berdasarkan kategori hash map nya (Indo -> inggris) || (Inggris -> indo)
	insertWord(IndoEng, indonesia, english, IndoToEng);
	insertWord(EngIndo, indonesia, english, EngToIndo);
}

//void view
void appendTranslation(Hashmap *IndoEng, Hashmap *EngIndo){ // Tampilan menu saat meng-insert dictionary

	char indonesia[100];
	printf("Masukkan kata Indonesia: ");
	scanf("%[^\n]", indonesia); getchar();

	char english[100];
	printf("Masukkan kata Inggris: ");
	scanf("%[^\n]", english); getchar();

	char confirm;
	printf("Apakah yakin ingin menambahkan terjemahan? [y/n]: ");
	scanf("%c", &confirm);
	if(tolower(confirm) != 'y'){
		printf("\nBatal menambahkan kata!\n\n");
		return;
	}

	FILE *file = fopen("translation.txt", "a");

	if(file == NULL){
		printf("Ada masalah pada file\n");
		exit(1);
	}

	fprintf(file, "%s#%s\n", indonesia, english);
	insert(IndoEng, EngIndo, indonesia, english);
	printf("\nTerjemahan ditambahkan!\n\n");

	fclose(file);

	return;
}

void readFile(Hashmap *IndoEng, Hashmap *EngIndo){ // Fungsi untuk mengambil data dictionary dari file ke struktur data avl nya
	FILE *file = fopen("translation.txt", "r");

	if(file == NULL){
		printf("Ada masalah pada file\n");
		exit(1);
	}

	char indonesia[100], english[100];
	while(fscanf(file, "%[^#]#%[^\n]\n", indonesia, english) != EOF){
		insert(IndoEng, EngIndo, indonesia, english);
	}

	fclose(file);
}

void inorderTraversal(Node *root, char alpha, int *counter){ // Menampilkan dictionary secara inorder (Sorted)
	if(root == NULL) return;
	inorderTraversal(root->left, alpha, counter);
	printf("%-4d %-25s %-25s   %c   \n", *counter, root->indonesia, root->english, alpha);
	(*counter)++;
	inorderTraversal(root->right, alpha, counter);
}

void viewAll(Hashmap *IndoEng){ // Membuat tampilan dan definisi kategori untuk menampilkan dictionary
	printf("---------------------------------------------------------------\n");
	printf("%-4s %-25s %-25s %s\n", "No", "Indonesia", "Inggris", "Abjad");
	printf("---------------------------------------------------------------\n");
	int i = 0, counter = 1;
	for(i = 0; i < 26; i++){
		inorderTraversal(IndoEng->table[i], i + 'A', &counter);
	}
	printf("---------------------------------------------------------------\n\n");
	printf("Tekan enter untuk kembali ke menu >> "); getchar();
	system("cls");
}

Node *findSmallestElement(Node *root) { // Mencari elemen terkecil (Untuk menggantikkan successor di deletion nanti)
    if(root == NULL || root->left == NULL){
        return root;
    }

    return findSmallestElement(root->left);
}

Node *deleteNode(Node *root, const char *word, Dictionary dictionary){ // Menghapus dictionary dari avl tree
	if(root == NULL) return NULL;

	int compare = (dictionary == IndoToEng) ? strlowcmp(word, root->indonesia) : strlowcmp(word, root->english);

	if(compare < 0) root->left = deleteNode(root->left, word, dictionary);
	else if(compare > 0) root->right = deleteNode(root->right, word, dictionary);
	else{
		if(root->left == NULL){
			Node *temp = root->right;
            free(root->indonesia);
            free(root->english);
            free(root);
            return temp;
		} else if(root->right == NULL){
			Node *temp = root->left;
            free(root->indonesia);
            free(root->english);
            free(root);
            return temp;
		} else{
			Node *temp = findSmallestElement(root->right);
			free(root->indonesia);
			free(root->english);
			root->indonesia = strdup(temp->indonesia);
	        root->english = strdup(temp->english);
	        root->right = deleteNode(root->right, temp->indonesia, dictionary);
    	}
	}

	root->height = 1 + max(getHeight(root->left), getHeight(root->right));

    int balance = getBalanceFactor(root);

    if (balance > 1 && getBalanceFactor(root->left) >= 0) // Kondisi Left Left
        return rightRotate(root);

    if (balance < -1 && getBalanceFactor(root->right) <= 0) // Kondisi Right Right
        return leftRotate(root);

    if (balance > 1 && getBalanceFactor(root->left) < 0){ // Kondisi Left-Right
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalanceFactor(root->right) > 0){ // Kondisi Right-Left
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

	return root;
}

void writePreorder(Node *root, FILE *file){ // Menulis dictionary dari struktur data ke file
	if(root == NULL) return;
	fprintf(file, "%s#%s\n", root->indonesia, root->english);
	writePreorder(root->left, file);
	writePreorder(root->right, file);
}

void deleteTranslation(Hashmap *IndoEng, Hashmap *EngIndo){ // Tampilan pada menu deletion

	char target[100];
	printf("Masukkan kata yang ingin dihapus (Indonesia/Inggris): ");
	scanf("%[^\n]", target); getchar();

	Node *translation = searchWord(IndoEng->table[hash(target)], target, IndoToEng);
	if(translation == NULL){
		translation = searchWord(EngIndo->table[hash(target)], target, EngToIndo);
		if(translation == NULL){
			printf("Kata tidak ditemukan!\n");
			return;
		}
	}

	char confirm;
	printf("\n{ Indonesia }, { English } -> { \033[0;32m%s\033[0m }, { \033[0;32m%s\033[0m }\n", translation->indonesia, translation->english);
	printf("Apakah yaking ingin menghapus?[y/n]: ");
	scanf("%c", &confirm);

	if(tolower(confirm) != 'y'){
		printf("\nBatal menghapus kata!\n\n");
		return;
	}
	char *deleteIndo = strdup(translation->indonesia);
	char *deleteEng = strdup(translation->english);

	IndoEng->table[hash(deleteIndo)] = deleteNode(IndoEng->table[hash(deleteIndo)], deleteIndo, IndoToEng);
	EngIndo->table[hash(deleteEng)] =  deleteNode(EngIndo->table[hash(deleteEng)], deleteEng, EngToIndo);

	FILE *file = fopen("translation.txt", "w");
	if(file == NULL){
	    printf("Ada masalah pada file\n");
	    exit(1);
	}

	int i;
	for(i = 0; i < 26; i++){
	    writePreorder(IndoEng->table[i], file); // Menulis ulang dari struktur data ke file (dari array 0 - 25 || pohon biner dari setiap abjad)
	}
	fclose(file);

	printf("\nKata berhasil dihapus!\n\n");

}

void changeTranslation(Hashmap *IndoEng, Hashmap *EngIndo){ // Tampilan saat ingin mengubah suatu kalimat pada dictionary

	char target[100];
	printf("Masukkan kata yang ingin diubah (Indonesia/Inggris): ");
	scanf("%[^\n]", target); getchar();

	Node *translation = searchWord(IndoEng->table[hash(target)], target, IndoToEng);
	if(translation == NULL){
		translation = searchWord(EngIndo->table[hash(target)], target, EngToIndo);
		if(translation == NULL){
			printf("Kata tidak ditemukan!\n");
			return;
		}
	}

	char newIndo[100], newEng[100];
	printf("\nKata awal: { Indonesia }, { English } -> { \033[0;32m%s\033[0m }, { \033[0;32m%s\033[0m }\n", translation->indonesia, translation->english);
	printf("Kata Indonesia baru: ");
	scanf("%[^\n]", newIndo); getchar();

	printf("Kata Inggris baru: ");
	scanf("%[^\n]", newEng); getchar();

	char confirm;
	printf("Yakin ingin mengubah? [y/n]: ");
	scanf("%c", &confirm);
	if(tolower(confirm) != 'y') {
		printf("\nBatal mengubah data!\n\n");
		return;
	}

	char *oldIndo = strdup(translation->indonesia);
	char *oldEng = strdup(translation->english);

	IndoEng->table[hash(oldIndo)] = deleteNode(IndoEng->table[hash(oldIndo)], oldIndo, IndoToEng);
	EngIndo->table[hash(oldEng)] = deleteNode(EngIndo->table[hash(oldEng)], oldEng, EngToIndo);

	insert(IndoEng, EngIndo, newIndo, newEng);

	FILE *file = fopen("translation.txt", "w");

	if(file == NULL){
		printf("Ada masalah pada file\n");
		exit(1);
	}

	int i;
	for(i = 0; i < 26; i++){
		writePreorder(IndoEng->table[i], file);
	}

	fclose(file);

	printf("\nKata berhasil diubah!\n\n");
}

void getPassword(char *password){ // Input password secara hidden ( *** )
    int index = 0;
    char ch;

    while(1){
        ch = getch();

        if (ch == 13){ // Jika enter (13) selesai input
            password[index] = '\0';
            printf("\n");
            break;
        }

        else if(ch == 8){ // Jika backspace (8) backspace 1 kata dan kurangi index
            if (index > 0){
                index--;
                printf("\b \b");
            }
        }

        else if(index < 49 && ch >= 32 && ch <= 126){
            password[index++] = ch;
            printf("*");
        }
    }
}

bool login(User *user, Bookmark **bookmark){ // login user
    char inputUser[50], inputPass[50];
    printf("Username: ");
    scanf("%s", inputUser);
    printf("Password: ");
    getPassword(inputPass);

    FILE *file = fopen("users.bin", "rb");
    if(file == NULL){
        printf("Masalah saat membuka file\n");
        return false;
    }

    User temp;
    while(fread(&temp, sizeof(User), 1, file)){
        if(strcmp(temp.username, inputUser) == 0 && strcmp(temp.password, inputPass) == 0){
            *user = temp;
            *bookmark = loadBookmarks(user->username);
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

bool usernameExist(const char *username) { // Mencari apakah username sudah ada di file
    FILE *file = fopen("users.bin", "rb");
    if (!file) return false;

    User temp;
    while (fread(&temp, sizeof(User), 1, file)) {
        if (strcmp(temp.username, username) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

void signUp() { // Daftar pengguna
    User newUser;

    printf("Buat username: ");
    scanf("%s", newUser.username);

    if(usernameExist(newUser.username)){
        printf("Username sudah digunakan.\n");
        return;
    }

    printf("Buat password: ");
    getPassword(newUser.password);

    printf("Daftar sebagai Admin? [1 = ya, 0 = tidak]: ");
    scanf("%d", &newUser.isAdmin);

    FILE *file = fopen("users.bin", "ab");
    if (!file){
        printf("Masalah saat membuka file\n");
        return;
    }

    fwrite(&newUser, sizeof(User), 1, file);
    fclose(file);

	system("cls");
    printf("Registrasi berhasil.\n");
}

void userPage(User user, Bookmark *bookmark){ // Tampilan utama pengguna biasa
	Hashmap *IndoEng = initializeMap();
	Hashmap *EngIndo = initializeMap();
	bookmark = loadBookmarks(user.username);

	readFile(IndoEng, EngIndo);

	int opt;
	do{
		welcome();
        printf("1. Cari terjemahan\n");
        printf("2. Lihat semua terjemahan\n");
        printf("3. Lihat bookmark\n");
        printf("4. Hapus bookmark\n");
        printf("5. Logout\n");
        printf("6. Keluar\n");
        printf("Pilih opsi >> ");
        scanf("%d", &opt); getchar();
        switch(opt){
            case 1:
                system("cls");
                search(IndoEng, EngIndo, &bookmark); // read
                break;
            case 2:
                system("cls");
                viewAll(IndoEng); // read
                break;
            case 3:
                system("cls");
                displayBookmarks(bookmark); // read
                printf("Tekan enter untuk kembali >> "); getchar();
                system("cls");
                break;
            case 4:
                system("cls");
                deleteBookmark(user.username, &bookmark); // delete
                break;
            case 5:
                system("cls");
                saveBookmarksToFile(user.username, bookmark); // update
                main();
            case 6:
                saveBookmarksToFile(user.username, bookmark); // update
                exit(0);
            default:
                system("cls");
                printf("Tolong masukkan input dengan benar [1 - 6]\n\n");
        }
	} while(true);
}

void adminPage(User user, Bookmark *bookmark){ // Tampilan utama pengguna admin
	Hashmap *IndoEng = initializeMap();
	Hashmap *EngIndo = initializeMap();
	bookmark = loadBookmarks(user.username);

	readFile(IndoEng, EngIndo);

	int opt;
	do{
		welcome();
		printf("1. Cari terjemahan\n");
		printf("2. Lihat semua terjemahan\n");
		printf("3. Buat terjemahan\n");
		printf("4. Ubah terjemahan\n");
		printf("5. Hapus terjemahan\n");
		printf("6. Logout\n");
		printf("7. Keluar\n");
		printf("Pilih opsi >> ");
		scanf("%d", &opt); getchar();
		switch(opt){
			case 1:
				system("cls");
				search(IndoEng, EngIndo, &bookmark); // Read
				break;
			case 2:
				system("cls");
				viewAll(IndoEng); // Read
				break;
			case 3:
				system("cls");
				appendTranslation(IndoEng, EngIndo); // Create
				break;
			case 4:
				system("cls");
				changeTranslation(IndoEng, EngIndo); // Update
				break;
			case 5:
				system("cls");
				deleteTranslation(IndoEng, EngIndo); // Delete
				break;
			case 6:
                system("cls");
                main();
			case 7:
				exit(0);
			default:
			    system("cls");
				printf("Tolong masukkan input dengan benar [1 - 7]\n\n");
		}
	} while(true);
}

int main(){
	User user;
	Bookmark *headBookmark = NULL;

	do{
		printf("1. Login\n");
		printf("2. Sign Up\n");
		printf("3. Keluar\n");
		printf(">> ");

		int opt;
		scanf("%d", &opt);
		switch(opt){
			case 1:
			    system("cls");
				while(true){
					if(login(&user, &headBookmark)){
						system("cls");
				        printf("Login berhasil sebagai %s (%s)\n\n", user.username, (user.isAdmin ? "Admin" : "User"));
				        break;
				    } else{
				        do{
                            printf("Kredential salah!\n");
                            printf("1. Coba lagi\n");
                            printf("2. Kembali ke menu utama\n");
                            printf(">> ");
                            int choice;
                            scanf("%d", &choice);
                            switch(choice){
                                case 1:
                                    system("cls");
                                    login(&user, &headBookmark);
                                    break;
                                case 2:
                                    system("cls");
                                    main();
                                    break;
                                default:
                                    system("cls");
                                    printf("Tolong masukkan input dengan benar [1 - 2]\n\n");
                            }

                        } while(true);
				    }

				}
				if(user.isAdmin) adminPage(user, headBookmark);
				else userPage(user, headBookmark);
				break;
			case 2:
				signUp();
				break;
            case 3:
                exit(0);
            default:
                system("cls");
                printf("Tolong masukkan input dengan benar [1 - 3]\n\n");
                main();
			}
	} while(true);


	return 0;
}
