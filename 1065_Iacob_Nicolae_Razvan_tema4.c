#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Book {
    int id;
    char* title;
    char* author;
    int year;
    float price;
    char* publisher;
    int* pages;
} Book;

// Structura pentru arbore binar
typedef struct Node {
    Book info;
    struct Node* left;
    struct Node* right;
} Node;

// Structura Heap
typedef struct Heap {
    Book* elements;
    int size;
    int capacity;
} Heap;

// Functie pentru creare Book
Book createBook(int id, const char* title, const char* author, int year, float price, const char* publisher, int pages) {
    Book b;
    b.id = id;
    b.title = (char*)malloc(strlen(title) + 1);
    strcpy(b.title, title);
    b.author = (char*)malloc(strlen(author) + 1);
    strcpy(b.author, author);
    b.year = year;
    b.price = price;
    b.publisher = (char*)malloc(strlen(publisher) + 1);
    strcpy(b.publisher, publisher);
    b.pages = (int*)malloc(sizeof(int));
    *(b.pages) = pages;
    return b;
}

// Inserare in BST
Node* insertBST(Node* root, Book b) {
    if (root == NULL) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->info = b;
        newNode->left = newNode->right = NULL;
        return newNode;
    }
    if (b.id < root->info.id)
        root->left = insertBST(root->left, b);
    else
        root->right = insertBST(root->right, b);
    return root;
}

// Citire arbore din fisier
Node* readBSTFromFile(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Fisierul nu poate fi deschis.\n");
        return NULL;
    }
    Node* root = NULL;
    int id, year, pages;
    float price;
    char title[100], author[100], publisher[100];
    while (fscanf(f, "%d %s %s %d %f %s %d", &id, title, author, &year, &price, publisher, &pages) == 7) {
        Book b = createBook(id, title, author, year, price, publisher, pages);
        root = insertBST(root, b);
    }
    fclose(f);
    return root;
}

// Functie pentru numarat frunzele
int countLeaves(Node* root) {
    if (root == NULL)
        return 0;
    if (root->left == NULL && root->right == NULL)
        return 1;
    return countLeaves(root->left) + countLeaves(root->right);
}

Heap createHeap(int capacity) {
    Heap h;
    h.elements = (Book*)malloc(capacity * sizeof(Book));
    h.size = 0;
    h.capacity = capacity;
    return h;
}

// Inserare in heap 
void insertHeap(Heap* h, Book b) {
    if (h->size >= h->capacity) return;
    h->elements[h->size] = b;
    int i = h->size;
    h->size++;
    while (i > 0 && h->elements[(i - 1) / 2].id > h->elements[i].id) {
        Book temp = h->elements[i];
        h->elements[i] = h->elements[(i - 1) / 2];
        h->elements[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

// Conversie arbore -> heap
void treeToHeap(Node* root, Heap* h) {
    if (root) {
        insertHeap(h, root->info);
        treeToHeap(root->left, h);
        treeToHeap(root->right, h);
    }
}

// Filtrare heap
void filterHeap(Heap* h) {
    int i = 0;
    while (i < h->size) {
        if (*(h->elements[i].pages) <= 300) {
            free(h->elements[i].title);
            free(h->elements[i].author);
            free(h->elements[i].publisher);
            free(h->elements[i].pages);
            h->elements[i] = h->elements[h->size - 1];
            h->size--;
            i = 0;
        }
        else {
            i++;
        }
    }
}

// Afisare frunze heap 
void displayHeapLeaves(Heap h) {
    for (int i = (h.size / 2); i < h.size; i++) {
        printf("Frunza heap: %s (%d pagini)\n", h.elements[i].title, *(h.elements[i].pages));
    }
}


void traverseTree(Node* root) {
    if (root) {
        printf("Carte: %s, Autor: %s, An: %d\n", root->info.title, root->info.author, root->info.year);
        traverseTree(root->left);
        traverseTree(root->right);
    }
}


void traverseHeap(Heap h) {
    for (int i = 0; i < h.size; i++) {
        printf("Heap Carte: %s, Autor: %s, An: %d\n", h.elements[i].title, h.elements[i].author, h.elements[i].year);
    }
}

// Stergere din heap 
void deleteFromHeap(Heap* h) {
    int i = 0;
    while (i < h->size) {
        if (h->elements[i].year < 2000) {
            free(h->elements[i].title);
            free(h->elements[i].author);
            free(h->elements[i].publisher);
            free(h->elements[i].pages);
            h->elements[i] = h->elements[h->size - 1];
            h->size--;
            i = 0;
        }
        else {
            i++;
        }
    }
}

void freeTree(Node* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root->info.title);
        free(root->info.author);
        free(root->info.publisher);
        free(root->info.pages);
        free(root);
    }
}

void freeHeap(Heap* h) {
    for (int i = 0; i < h->size; i++) {
        free(h->elements[i].title);
        free(h->elements[i].author);
        free(h->elements[i].publisher);
        free(h->elements[i].pages);
    }
    free(h->elements);
}

int main() {
    Node* root = readBSTFromFile("carti.txt");

    // 3. Calculare frunze din subarbore drept
    if (root && root->right) {
        int leafCount = countLeaves(root->right);
        printf("Numar de frunze in subarbore drept: %d\n", leafCount);
    }

    // 4. Conversie arbore -> heap
    Heap h = createHeap(100);
    treeToHeap(root, &h);

    // 5. Filtrare heap
    filterHeap(&h);

    // 6. Afisare frunze heap
    displayHeapLeaves(h);

    // 7. Traversari
    printf("\nTraversare Arbore:\n");
    traverseTree(root);

    printf("\nTraversare Heap:\n");
    traverseHeap(h);

    // 8. Stergere din heap
    deleteFromHeap(&h);

    // 9. Eliberare memorie
    freeTree(root);
    freeHeap(&h);

    return 0;
}

/*
AUTOEVALUARE: 7/10
*/