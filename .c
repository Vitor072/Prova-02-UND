#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PRODUTOS 100
#define MAX_USUARIOS 10
#define MAX_CARRINHO 50

struct Produto {
    char nome[50];
    float preco;
    char categoria[20];
};

struct Usuario {
    char nome[50];
    char senha[50];
    char email[50];
};

struct Carrinho {
    struct Produto produtos[MAX_CARRINHO];
    int qtdProdutos;
};

// Função de login
int login(struct Usuario* usuarios, int qtdUsuarios) {
    char nome[50];
    char senha[50];
    char email[50];
    int tentativas = 3;

    while (tentativas > 0) {
        printf("Login de Usuário:\n");
        scanf("%s", nome);
        printf("Senha:\n");
        scanf("%s", senha);
        printf("Email:\n");
        scanf("%s", email);

        for (int i = 0; i < qtdUsuarios; i++) {
            if (strcmp(usuarios[i].nome, nome) == 0 && 
                strcmp(usuarios[i].senha, senha) == 0 && 
                strcmp(usuarios[i].email, email) == 0) {
                printf("Login Efetuado Com Sucesso!\n");
                return 1;
            }
        }

        tentativas--;
        printf("Usuário, Senha ou Email INCORRETO!\nTente Novamente!\n\nTentativas Restantes: %d\n\n\n", tentativas);
    }

    return 0;
}

// Função de boas-vindas
void mensagemBoasVindas() {
    printf("------------------------------------\n\n\n");
    printf("Bem-vindo à AMERICAN IMPORTS\n\n\n");
    printf("------------------------------------\n");
}

// Função para cadastrar produto
void cadastrarProduto(struct Produto* produtos, int* qtdProdutos) {
    if (*qtdProdutos < MAX_PRODUTOS) {
        printf("Informe o nome, categoria e o preço do produto:\n");
        scanf("%49s %19s %f", produtos[*qtdProdutos].nome, produtos[*qtdProdutos].categoria, &produtos[*qtdProdutos].preco);
        (*qtdProdutos)++;
        printf("Produto cadastrado com sucesso!\n");
    } else {
        printf("Estoque cheio!\n");
    }
}

// Função para listar produtos
void listarProdutos(struct Produto* produtos, int qtdProdutos) {
    printf("--- Lista de Produtos ---\n");

    for (int i = 0; i < qtdProdutos; i++) {
        printf("%d. %s (%s) - R$ %.2f\n", i + 1, produtos[i].nome, produtos[i].categoria, produtos[i].preco);
    }
    printf("=========================\n");
}

// Função para buscar produto por nome (para busca binária)
int buscaBinaria(struct Produto* produtos, int qtdProdutos, const char* nome) {
    int esquerda = 0, direita = qtdProdutos - 1;

    while (esquerda <= direita) {
        int meio = esquerda + (direita - esquerda) / 2;
        int cmp = strcmp(produtos[meio].nome, nome);

        if (cmp == 0) {
            return meio; // Produto encontrado
        }
        if (cmp < 0) {
            esquerda = meio + 1;
        } else {
            direita = meio - 1;
        }
    }

    return -1; // Produto não encontrado
}

// Função para ordenar os produtos usando QuickSort
int comparacao(const void* a, const void* b) {
    return strcmp(((struct Produto*)a)->nome, ((struct Produto*)b)->nome);
}

void quicksort(struct Produto* produtos, int baixo, int alto) {
    if (baixo < alto) {
        int pivo = particao(produtos, baixo, alto);
        quicksort(produtos, baixo, pivo - 1);
        quicksort(produtos, pivo + 1, alto);
    }
}

int particao(struct Produto* produtos, int baixo, int alto) {
    struct Produto pivo = produtos[alto];
    int i = baixo - 1;

    for (int j = baixo; j < alto; j++) {
        if (strcmp(produtos[j].nome, pivo.nome) <= 0) {
            i++;
            struct Produto temp = produtos[i];
            produtos[i] = produtos[j];
            produtos[j] = temp;
        }
    }

    struct Produto temp = produtos[i + 1];
    produtos[i + 1] = produtos[alto];
    produtos[alto] = temp;

    return i + 1;
}

// Função para salvar os produtos em um arquivo binário
void salvarProdutos(struct Produto* produtos, int qtdProdutos) {
    FILE* arquivo = fopen("produtos.bin", "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    fwrite(&qtdProdutos, sizeof(int), 1, arquivo);
    fwrite(produtos, sizeof(struct Produto), qtdProdutos, arquivo);
    fclose(arquivo);
    printf("Produtos salvos com sucesso!\n");
}

// Função para carregar os produtos de um arquivo binário
void carregarProdutos(struct Produto* produtos, int* qtdProdutos) {
    FILE* arquivo = fopen("produtos.bin", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return;
    }

    fread(qtdProdutos, sizeof(int), 1, arquivo);
    fread(produtos, sizeof(struct Produto), *qtdProdutos, arquivo);
    fclose(arquivo);
    printf("Produtos carregados com sucesso!\n");
}

// Função para editar produto
void editarProduto(struct Produto* produtos, int qtdProdutos) {
    char nome[50];
    printf("Digite o nome do produto a ser editado: ");
    scanf("%49s", nome);

    int index = buscaBinaria(produtos, qtdProdutos, nome);
    if (index != -1) {
        printf("Informe o novo nome, categoria e o novo preço do produto:\n");
        scanf("%49s %19s %f", produtos[index].nome, produtos[index].categoria, &produtos[index].preco);
        printf("Produto editado com sucesso!\n");
    } else {
        printf("Produto não encontrado!\n");
    }
}

// Função para excluir produto
void excluirProduto(struct Produto* produtos, int* qtdProdutos) {
    char nome[50];
    printf("Digite o nome do produto a ser excluído: ");
    scanf("%49s", nome);

    int index = buscaBinaria(produtos, *qtdProdutos, nome);
    if (index != -1) {
        for (int i = index; i < *qtdProdutos - 1; i++) {
            produtos[i] = produtos[i + 1];
        }
        (*qtdProdutos)--;
        printf("Produto excluído com sucesso!\n");
    } else {
        printf("Produto não encontrado!\n");
    }
}

// Função para comprar produto e adicionar ao carrinho
void comprarProduto(struct Produto* produtos, int qtdProdutos, struct Carrinho* carrinho) {
    int escolha;
    listarProdutos(produtos, qtdProdutos);
    printf("Digite o número do produto que deseja adicionar ao carrinho: ");
    scanf("%d", &escolha);

    if (escolha > 0 && escolha <= qtdProdutos) {
        struct Produto produtoEscolhido = produtos[escolha - 1];
        if (carrinho->qtdProdutos < MAX_CARRINHO) {
            carrinho->produtos[carrinho->qtdProdutos] = produtoEscolhido;
            carrinho->qtdProdutos++;
            printf("Produto '%s' adicionado ao carrinho.\n", produtoEscolhido.nome);
        } else {
            printf("Carrinho cheio!\n");
        }
    } else {
        printf("Número do produto inválido!\n");
    }
}

// Função para exibir catálogo de produtos por categoria
void catalogoProdutos(struct Produto* produtos, int qtdProdutos) {
    printf("--- Catálogo de Produtos ---\n");

    const char* categorias[] = {"Camisas", "Calças", "Shorts", "Tênis"};
    int numCategorias = sizeof(categorias) / sizeof(categorias[0]);

    for (int c = 0; c < numCategorias; c++) {
        printf("Categoria: %s\n", categorias[c]);
        for (int i = 0; i < qtdProdutos; i++) {
            if (strcmp(produtos[i].categoria, categorias[c]) == 0) {
                printf("  %d. %s - R$ %.2f\n", i + 1, produtos[i].nome, produtos[i].preco);
            }
        }
        printf("\n");
    }
    printf("=========================\n");
}

// Função para exibir os produtos no carrinho
void mostrarCarrinho(struct Carrinho* carrinho) {
    printf("--- Carrinho de Compras ---\n");

    if (carrinho->qtdProdutos == 0) {
        printf("O carrinho está vazio.\n");
    } else {
        for (int i = 0; i < carrinho->qtdProdutos; i++) {
            printf("%d. %s - R$ %.2f\n", i + 1, carrinho->produtos[i].nome, carrinho->produtos[i].preco);
        }
    }
    printf("=========================\n");
}

// Função para remover produto do carrinho
void removerDoCarrinho(struct Carrinho* carrinho) {
    int escolha;
    mostrarCarrinho(carrinho);
    printf("Digite o número do produto que deseja remover do carrinho: ");
    scanf("%d", &escolha);

    if (escolha > 0 && escolha <= carrinho->qtdProdutos) {
        for (int i = escolha - 1; i < carrinho->qtdProdutos - 1; i++) {
            carrinho->produtos[i] = carrinho->produtos[i + 1];
        }
        carrinho->qtdProdutos--;
        printf("Produto removido do carrinho.\n");
    } else {
        printf("Número do produto inválido!\n");
    }
}

// Função para calcular o total do carrinho
float calcularTotalCarrinho(struct Carrinho* carrinho) {
    float total = 0.0;
    for (int i = 0; i < carrinho->qtdProdutos; i++) {
        total += carrinho->produtos[i].preco;
    }
    return total;
}

// Função para finalizar compra
void finalizarCompra(struct Carrinho* carrinho) {
    if (carrinho->qtdProdutos == 0) {
        printf("O carrinho está vazio! Não é possível finalizar a compra.\n");
        return;
    }

    float total = calcularTotalCarrinho(carrinho);
    printf("=== Finalizar Compra ===\n");
    printf("O total da sua compra é: R$ %.2f\n", total);
    printf("Compra realizada com sucesso!\n");

    // Esvaziar o carrinho após finalizar a compra
    carrinho->qtdProdutos = 0;
    printf("O carrinho foi esvaziado.\n");
}

// Função para exibir o menu
void menu() {
    printf("=== Escolha uma opção ===\n");
    printf("1 - Cadastrar produto\n");
    printf("2 - Ver Carrinho\n");
    printf("3 - Editar produto\n");
    printf("4 - Remover do carrinho\n");
    printf("5 - Comprar produto\n");
    printf("6 - Ver catálogo de produtos\n");
    printf("7 - Finalizar compra\n");
    printf("8 - Salvar produtos\n");
    printf("9 - Carregar produtos\n");
    printf("10 - Sair\n");
    printf("=========================\n");
}

// Função principal
int main() {
    // Produtos e usuários pré-cadastrados
    struct Produto produtos[MAX_PRODUTOS] = {
        {"Camisa Polo Azul Claro", 79.90, "Camisas"},
        {"Camisa Social Branca Slim Fit", 99.00, "Camisas"},
        {"Camisa de Linho Bege", 120.00, "Camisas"},
        {"Camisa Xadrez Vermelha", 89.90, "Camisas"},
        {"Camisa Jeans Manga Longa", 110.00, "Camisas"},
        {"Calça Jeans Skinny Azul", 129.90, "Calças"},
        {"Calça Chino Caqui", 139.00, "Calças"},
        {"Calça Moletom Cinza Mescla", 99.00, "Calças"},
        {"Calça Social Preta Slim Fit", 149.90, "Calças"},
        {"Calça Cargo Verde Militar", 119.00, "Calças"},
        {"Bermuda Jeans Rasgada", 89.90, "Shorts"},
        {"Short Moletom Preto", 59.90, "Shorts"},
        {"Bermuda Sarja Bege", 79.00, "Shorts"},
        {"Short Esportivo Cinza", 49.90, "Shorts"},
        {"Bermuda Estampada Floral", 69.90, "Shorts"},
        {"Tênis Branco Casual", 199.90, "Tênis"},
        {"Tênis Esportivo Preto", 249.90, "Tênis"},
        {"Tênis Slip On Azul Marinho", 179.00, "Tênis"},
        {"Tênis de Corrida Cinza", 299.90, "Tênis"},
        {"Tênis Chunky Branco", 269.90, "Tênis"}
    };

    struct Usuario usuarios[MAX_USUARIOS] = {
        {"vitor", "vitor123", "vitor@example.com"},
        {"daniel", "daniel321", "daniel@example.com"},
        {"vdaniel", "vitord2", "vdaniel@example.com"},
        {"daniel3", "vitor3", "daniel3@example.com"},
        {"vitor55", "daniel1234", "vitor55@example.com"}
    };

    struct Carrinho carrinho = { .qtdProdutos = 0 };
    int qtdProdutos = 20;
    int qtdUsuarios = 5;

    // Ordenar produtos antes do login
    quicksort(produtos, 0, qtdProdutos - 1);

    // Login do usuário
    if (!login(usuarios, qtdUsuarios)) {
        printf("Número de tentativas excedido. Programa encerrado.\n");
        return 1;
    }

    mensagemBoasVindas();

    int op;
    do {
        menu();
        scanf("%d", &op);
        switch (op) {
            case 1:
                cadastrarProduto(produtos, &qtdProdutos);
                // Ordenar produtos após cadastro
                quicksort(produtos, 0, qtdProdutos - 1);
                break;
            case 2:
                mostrarCarrinho(&carrinho);
                break;
            case 3:
                editarProduto(produtos, qtdProdutos);
                // Ordenar produtos após edição
                quicksort(produtos, 0, qtdProdutos - 1);
                break;
            case 4:
                removerDoCarrinho(&carrinho);
                break;
            case 5:
                comprarProduto(produtos, qtdProdutos, &carrinho);
                break;
            case 6:
                catalogoProdutos(produtos, qtdProdutos);
                break;
            case 7:
                finalizarCompra(&carrinho);
                break;
            case 8:
                salvarProdutos(produtos, qtdProdutos);
                break;
            case 9:
                carregarProdutos(produtos, &qtdProdutos);
                // Ordenar produtos após carregar do arquivo
                quicksort(produtos, 0, qtdProdutos - 1);
                break;
            case 10:
                printf("Saindo do programa...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (op != 10);

    return 0;
}
