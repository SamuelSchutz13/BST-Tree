#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

typedef struct tree {
    int id;
    char name[50];
    struct tree* left;
    struct tree* right;
} tree;

int nextId = 1;

int compareById(const void* a, const void* b) {
    return (*(tree**)a)->id - (*(tree**)b)->id;
}

int isValidName(char name[]) {
    for (int i = 0; name[i] != '\0'; i++) {
        if (isdigit(name[i])) {
            return 0;
        }
    }

    return 1;
}

int isNameInTree(tree* root, char name[]) {
    if (root == NULL) {
        return 0; 
    }

    int compare = strcmp(name, root->name);

    if (compare == 0) {
        return 1; 
    } else if (compare < 0) {
        return isNameInTree(root->left, name);
    } else {
        return isNameInTree(root->right, name);
    }
}

void reassignIds(tree* root) {
    if (root != NULL) {
        reassignIds(root->left);
        root->id = nextId++;
        reassignIds(root->right);
    }
}

void updateIds(tree* root) {
    nextId = 1;
    reassignIds(root);
}

void printElementsInOrder(tree** elements, int count) {
    for (int i = 0; i < count; i++) {
        printf("ID: %-4d| Nome: %s\n", elements[i]->id, elements[i]->name);
    }
}

void collectElementsInOrder(tree* root, tree** elements, int* count) {
    if (root != NULL) {
        collectElementsInOrder(root->left, elements, count);
        elements[(*count)++] = root;
        collectElementsInOrder(root->right, elements, count);
    }
}

tree* findMinElement(tree* root) {
    while (root->left != NULL) {
        root = root->left;
    }

    return root;
}

tree* createElementInTree(char name[]) {
    tree* newElement = (tree*)malloc(sizeof(tree));
    newElement->id = nextId++;
    strcpy(newElement->name, name);
    newElement->left = NULL;
    newElement->right = NULL;
    return newElement;
}

tree* insertElementInTree(tree* root, char name[]) {
    if (root == NULL) {
        return createElementInTree(name);
    }

    if (isNameInTree(root, name)) {
        printf("Nome ja existe na arvore, ignorando insercao\n");
        return root;
    }

    int compare = strcmp(name, root->name);
    if (compare < 0) {
        root->left = insertElementInTree(root->left, name);
    } else {
        root->right = insertElementInTree(root->right, name);
    }

    return root;
}

tree* searchElementByIdInTree(tree* root, int id) {
    if (root == NULL) {
        return NULL;
    }

    if (id == root->id) {
        return root;
    }

    tree* leftResult = searchElementByIdInTree(root->left, id);
    if (leftResult != NULL) {
        return leftResult;
    }

    tree* rightResult = searchElementByIdInTree(root->right, id);
    return rightResult;
}

tree* removeElementInTree(tree* root, int id) {
    if (root == NULL) {
        printf("Elemento com ID %d nao foi encontrado na arvore, Nenhuma remocao feita\n", id);
        return root; 
    }

    if (id < root->id) {
        root->left = removeElementInTree(root->left, id);
    } else if (id > root->id) {
        root->right = removeElementInTree(root->right, id);
    } else {
        if (root->id == 1) {
            tree* temp = root->right;
            free(root);
            nextId = 1;
            updateIds(temp);
            return temp;
        }

        if (root->left == NULL) {
            tree* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            tree* temp = root->left;
            free(root);
            return temp;
        }

        tree* temp = findMinElement(root->right);
        root->id = temp->id;
        strcpy(root->name, temp->name);
        root->right = removeElementInTree(root->right, temp->id);
    }

    updateIds(root); 
    return root;
}

tree* editElementInTree(tree* root, int id, char newName[]) {
    tree* element = searchElementByIdInTree(root, id);

    if (element != NULL) {
        strcpy(element->name, newName);
    } else {
        printf("Elemento com ID %d nao foi encontrado na arvore, nenhuma edicao feita\n", id);
    }

    return root;
}

void saveTreeToFileOrdered(FILE* file, tree* node) {
    if (node != NULL) {
        saveTreeToFileOrdered(file, node->left);
        fprintf(file, "%d | %s\n", node->id, node->name);
        saveTreeToFileOrdered(file, node->right);
    }
}

void saveTreeToFile(tree* root) {
    FILE* file = fopen("database/tree.txt", "w");

    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    saveTreeToFileOrdered(file, root);
    fclose(file);
}

void loadTreeFromFile(tree** root) {
    FILE* file = fopen("database/tree.txt", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        int id;
        char name[50];
        if (sscanf(line, "%d | %[^\n]", &id, name) == 2) {
            *root = insertElementInTree(*root, name);
            (*root)->id = id;
        }
    }

    fclose(file);
}

void exitSystem(tree** root) {
    saveTreeToFile(*root); 
    puts("Saindo do sistema, salvando informacoes...");
    exit(1);
}

void menu() {
    puts("------------------------------------");
    puts("[1] => Inserir elemento");
    puts("[2] => Buscar elemento");
    puts("[3] => Excluir elemento");
    puts("[4] => Percorrer Arvore");
    puts("[5] => Editar elemento");
    puts("[6] => Sair");
    puts("------------------------------------");
    puts("Escolha uma opcao: ");
}

void process_menu(tree** root, int *command) {
    char name[50];
    int id;
    tree *result;

    switch (*command) {
        case 1:
            puts("Informe o nome a ser inserido: ");
            scanf("%s", name);

            if (!isValidName(name)) {
                printf("Nome invalido, nao deve conter numeros\n");
                break; 
            }

            *root = insertElementInTree(*root, name);
            break;
        case 2:
            puts("Informe o ID a ser buscado: ");
            if (scanf("%d", &id) != 1) {
                puts("Por favor, insira um numero\n");
                while (getchar() != '\n'); 
                break;
            }
            result = searchElementByIdInTree(*root, id);
            if (result != NULL) {
                printf("Listando o nome encontrado: \n");
                printf("ID: %-4d| Nome: %s\n", result->id, result->name);
            }

            break;
        case 3:
            puts("Informe o ID a ser removido da arvore: ");
            if (scanf("%d", &id) != 1) {
                puts("Por favor, insira um numero\n");
                while (getchar() != '\n'); 
                break;
            }

            *root = removeElementInTree(*root, id);
            break;
        case 4:
            if (*root != NULL) {
                int count = 0;
                tree** elements = (tree**)malloc(sizeof(tree*) * nextId);
                collectElementsInOrder(*root, elements, &count);
                qsort(elements, count, sizeof(tree*), compareById);
                printElementsInOrder(elements, count);
                free(elements);
            } else {
                puts("A arvore esta vazia");
            }

            break;
        case 5:
            puts("Informe o ID do elemento a ser editado: ");
            if (scanf("%d", &id) != 1) {
                puts("Por favor, insira um numero valido\n");
                while (getchar() != '\n'); 
                break;
            }

            while (getchar() != '\n'); 

            char newName[50];
            printf("Informe o novo nome: \n");
            scanf("%s", newName);

            if (!isValidName(newName)) {
                puts("Nome invalido, nao deve conter numeros\n");
                break;
            }

            *root = editElementInTree(*root, id, newName);
            break;
        case 6:
            exitSystem(root); 
            break;
        default:
            puts("Opcao invalida, tente novamente!\n");
    }
}

int main() {
    int command;
    setlocale(LC_ALL, "Portuguese");  
    tree* root = NULL;
    loadTreeFromFile(&root);

    do {
        menu();
        scanf("%d", &command);
        printf("\033[H\033[J");
        
        while (getchar() != '\n');
        process_menu(&root, &command);
    } while (command != 6);

    return 0;
}