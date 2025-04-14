#define _CRT_SECURE_NO_WARNINGS
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIM 101

typedef struct Persoana {
    int id;
    char* nume;
    int varsta;
    float salariu;
    char* functie;
    char* oras;
    struct Persoana* next;
} Persoana;

typedef struct Nod {
    Persoana* info;
    struct Nod* next;
} Nod;

typedef struct {
    Nod* front;
    Nod* rear;
} Coada;

typedef struct {
    Persoana* tabela[DIM];
} HashTable;

//CREARE SI AFISARE 

Persoana* crearePersoana(int id, const char* nume, int varsta, float salariu, const char* functie, const char* oras) {
    Persoana* p = (Persoana*)malloc(sizeof(Persoana));
    p->id = id;
    p->nume = strdup(nume);
    p->varsta = varsta;
    p->salariu = salariu;
    p->functie = strdup(functie);
    p->oras = strdup(oras);
    p->next = NULL;
    return p;
}

void enqueue(Coada* q, Persoana* p) {
    Nod* nou = (Nod*)malloc(sizeof(Nod));
    nou->info = p;
    nou->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = nou;
    } else {
        q->rear->next = nou;
        q->rear = nou;
    }
}

void afisareCoada(Coada q) {
    Nod* temp = q.front;
    printf("\n--- Coada ---\n");
    while (temp) {
        printf("ID: %d, Nume: %s, Varsta: %d, Salariu: %.2f, Functie: %s, Oras: %s\n",
               temp->info->id, temp->info->nume, temp->info->varsta, temp->info->salariu,
               temp->info->functie, temp->info->oras);
        temp = temp->next;
    }
}

//FISIER 

void citireFisier(const char* numeFisier, Coada* q) {
    FILE* f = fopen(numeFisier, "r");
    if (!f) {
        printf("Eroare la deschiderea fisierului.\n");
        return;
    }
    int id, varsta;
    float salariu;
    char nume[50], functie[50], oras[50];
    while (fscanf(f, "%d %s %d %f %s %s", &id, nume, &varsta, &salariu, functie, oras) == 6) {
        Persoana* p = crearePersoana(id, nume, varsta, salariu, functie, oras);
        enqueue(q, p);
    }
    fclose(f);
}

//CAUTARE IN COADA 

Persoana* ultimSalariuMare(Coada q, float prag) {
    Nod* temp = q.front;
    Persoana* rezultat = NULL;
    while (temp) {
        if (temp->info->salariu > prag) {
            rezultat = temp->info;
        }
        temp = temp->next;
    }
    return rezultat;
}

//HASH TABLE 

int hashCustom(const char* nume) {
    int suma = 0;
    for (int i = 0; nume[i]; i++)
        suma += nume[i];
    return suma % DIM;
}

void initHash(HashTable* ht) {
    for (int i = 0; i < DIM; i++)
        ht->tabela[i] = NULL;
}

void inserareHash(HashTable* ht, Persoana* p) {
    int index = hashCustom(p->nume);
    p->next = ht->tabela[index];
    ht->tabela[index] = p;
}

void coadaToHash(Coada q, HashTable* ht) {
    Nod* temp = q.front;
    while (temp) {
        Persoana* copie = crearePersoana(
            temp->info->id,
            temp->info->nume,
            temp->info->varsta,
            temp->info->salariu,
            temp->info->functie,
            temp->info->oras
        );
        inserareHash(ht, copie);
        temp = temp->next;
    }
}

void afisareHash(HashTable ht) {
    printf("\n--- Tabela de Dispersie ---\n");
    for (int i = 0; i < DIM; i++) {
        Persoana* temp = ht.tabela[i];
        if (temp) {
            printf("Index %d:\n", i);
            while (temp) {
                printf(" -> %s (%d)\n", temp->nume, temp->id);
                temp = temp->next;
            }
        }
    }
}

//VECTOR PE BAZA CHEII 

Persoana** cautaNume(HashTable ht, const char* nume, int* dim) {
    int index = hashCustom(nume);
    Persoana* temp = ht.tabela[index];
    Persoana** rezultat = malloc(100 * sizeof(Persoana*)); // presupunem max 100
    *dim = 0;
    while (temp) {
        if (strcmp(temp->nume, nume) == 0)
            rezultat[(*dim)++] = temp;
        temp = temp->next;
    }
    return rezultat;
}

void afisareVector(Persoana** v, int dim) {
    printf("\n--- Vector Persoane ---\n");
    for (int i = 0; i < dim; i++) {
        printf("VECTOR: %s (%d)\n", v[i]->nume, v[i]->id);
    }
}

//STERGERE 

void stergePersoaneVechi(HashTable* ht, int prag) {
    for (int i = 0; i < DIM; i++) {
        Persoana* temp = ht->tabela[i];
        Persoana* anterior = NULL;
        while (temp) {
            if (temp->varsta > prag) {
                Persoana* deSters = temp;
                if (anterior == NULL)
                    ht->tabela[i] = temp->next;
                else
                    anterior->next = temp->next;
                temp = temp->next;
                free(deSters->nume);
                free(deSters->functie);
                free(deSters->oras);
                free(deSters);
            } else {
                anterior = temp;
                temp = temp->next;
            }
        }
    }
}

//CLEANUP 
void elibereazaCoada(Coada* q) {
    while (q->front) {
        Nod* temp = q->front;
        q->front = q->front->next;
        free(temp->info->nume);
        free(temp->info->functie);
        free(temp->info->oras);
        free(temp->info);
        free(temp);
    }
    q->rear = NULL;
}

//MAIN 
int main() {
    Coada q = {NULL, NULL};
    HashTable ht;
    initHash(&ht);

    citireFisier("persoane.txt", &q);
    afisareCoada(q);

    Persoana* ultima = ultimSalariuMare(q, 4000);
    if (ultima)
        printf("\nUltimul cu salariu > 4000: %s (%d)\n", ultima->nume, ultima->id);

    coadaToHash(q, &ht);
    afisareHash(ht);

    int dim = 0;
    Persoana** vector = cautaNume(ht, "Popescu", &dim);
    afisareVector(vector, dim);
    free(vector);

    stergePersoaneVechi(&ht, 60);
    printf("\nDupa stergere:\n");
    afisareHash(ht);

    elibereazaCoada(&q);

    return 0;
}
