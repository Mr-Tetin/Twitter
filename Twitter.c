#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define maxlinhas 1000//define um numero bom de linhas que se pode ter(usuarios limites, para fins de trabalho 1000 usuarios é suficiente, mas vc pode mudar se quiser)

typedef struct usuario{//Atributos de usuario em uma struct
    char login[21];
    char senha[31];
    char nome[100];
}usuario;

typedef struct trending{//Struct para definir trending topics
    char palavra[20];
    int frequencia;
}trending;

usuario usuarios;//Definicao de variaveis do tipo usuario, um usuario geral
usuario usuario_atual;//E um usuario logado atualmente

void menu_incial(){//Menu de interação
    printf("\n+-------------------------------------------+\n");
    printf("|          +----------------------+         |\n");
    printf("|          |         MENU         |         |\n");
    printf("|          +----------------------+         |\n");
    printf("|          +----------------------+         |\n");
    printf("|          |  1-CADASTRAR USUARIO |         |\n");
    printf("|          |                      |         |\n");
    printf("|          |  2-EFETUAR LOGIN     |         |\n");
    printf("|          |                      |         |\n");
    printf("|          |  3-SAIR              |         |\n");
    printf("|          +----------------------+         |\n");
    printf("+-------------------------------------------+\n\n");
}
void cadastrar_post(){//Funcao que Cadastra os post na plataforma
    char post[280];//post com tamanho maximo de 280 caracteres(igual no Twitter/X real)
    char *hashtag; //usuario pode inserir um post com 280 caracteres sem espaco
    char delimitador[] = " ,.;/~[]{}";//Delimitadores para as hashtags, para evitar hastags como: #teste.teste,teste
    
    time_t tempo;//Definicao de tempo
    time(&tempo);
    struct tm *tempo2 = localtime(&tempo);

    FILE *arquivo_post;
    arquivo_post = fopen("Arquivo_post.txt", "a+");

    FILE *arquivo_trending;
    arquivo_trending = fopen("Arquivo_Trending.txt","a+");
    
    printf("Digite o post: ");
    getchar();
    fgets(post, 280, stdin);
    post[strcspn(post, "\n")] = 0;

    while(1){//Entra em um loop para verificar se o tamanho não excede o limite
        if(strlen(post) > 280){
            printf("Digite o post com ate 280 caracteres: ");
            fgets(post, 280, stdin);
            post[strcspn(post, "\n")] = 0;
        }else{
            break;
        }
    }

    fprintf(arquivo_post, "%s;%s;%02d:%02d:%02d;%s\n", usuario_atual.login, usuario_atual.nome,tempo2->tm_hour, tempo2->tm_min, tempo2->tm_sec,post);
    fflush(arquivo_post);

    hashtag = strtok(post,delimitador);

    while(hashtag != NULL){//separar as hashtags da string principal
        if(hashtag[0] == '#' && strlen(hashtag) != 1){
            fprintf(arquivo_trending, "%s\n", hashtag);
            fflush(arquivo_trending);
        }
        hashtag = strtok(NULL, delimitador);
    }
    fclose(arquivo_post);
    fclose(arquivo_trending);
}
void feed(){
    char linha[maxlinhas];
    char *login_post;
    char *nome_post;
    char *hora_post;
    char *post;

    FILE *arquivo_feed = fopen("Arquivo_post.txt", "a+");

    while(fgets(linha, maxlinhas, arquivo_feed)){//Com o auxilio do ponteiro arquivo_feed ele lê o arquivo_post
        login_post = strtok(linha, ";");
        nome_post = strtok(NULL, ";");
        hora_post = strtok(NULL, ";");
        post = strtok(NULL,";");

        printf("\n%s @%s - %s", nome_post, login_post, hora_post);
        printf("\n%s\n", post);
    }
}
void organizar_hashtag(int contador, trending *vetor_hashtag){//Ordena as hashtags mais usadas
    trending aux;
    int maior;
    for(int i = 0;i<contador;i++){
        maior = -1;
        for(int j=i; j<(contador);j++){
            if(vetor_hashtag[j].frequencia>=maior){
                aux=vetor_hashtag[i];
                vetor_hashtag[i]=vetor_hashtag[j];
                vetor_hashtag[j]=aux;
                maior = vetor_hashtag[i].frequencia;
            }
        }
    }
}
void trending_topics(){
    trending *vetor_hashtags;
    char quebra_linha;
    int linhas;
    char hashtag[maxlinhas];
    int contador = 0;
    int flag_presente = 0; //verifica se uma hashtag existe
    

    FILE *arquivo_trending = fopen("Arquivo_Trending.txt", "a+");

    //contador de linhas do arquivo 
    while((quebra_linha = fgetc(arquivo_trending)) != EOF){
        if(quebra_linha == '\n'){
            linhas++;        
        }
    }
    
    vetor_hashtags = (trending *) malloc(linhas * sizeof(trending));

    rewind(arquivo_trending);
    while(fgets(hashtag, maxlinhas,arquivo_trending)){//Lê as linhas do arquivo_trending
        flag_presente = 0;
        hashtag[strcspn(hashtag, "\n")] = 0;
        for(int i = 0; i <= contador; i++){
            if(strcmp(vetor_hashtags[i].palavra, hashtag) == 0){//se a hashtag indice i ja existir ele adiciona na frequencia
                vetor_hashtags[i].frequencia++;
                flag_presente = 1;
            } 
        }
        if(flag_presente == 0){//se a flag for 0 adiciona na lista de hashtags
            strcpy(vetor_hashtags[contador].palavra, hashtag);
            vetor_hashtags[contador].frequencia = 1;
            contador++;
        }    
    }
    organizar_hashtag(contador, vetor_hashtags);
    printf("\nTrending Topics:\n\n");
    if(contador > 10){//printa as 10 primeiras
        for(int i = 0; i < 10; i++){
            printf("%-20s%5d\n", vetor_hashtags[i].palavra, vetor_hashtags[i].frequencia);
        }
    }else{//printa todas caso nao tenha 10 ou mais
        for(int i = 0; i < contador; i++){
            printf("%-20s%5d\n", vetor_hashtags[i].palavra, vetor_hashtags[i].frequencia);
        }
    }
}
void pesquisar_user(){
    FILE *arquivo_usuario = fopen("Arquivos_dadosUsuarios.txt","r");
    FILE *arquivo_posts = fopen("Arquivo_post.txt","r");
    char *login_gabarito;
    char login_pesquisa[21];
    char linha[maxlinhas];
    char linha_post[maxlinhas];
    int flag_input = 0;
    char *login_post;
    char *nome_post;
    char *hora_post;
    char *post;

    while(1){
        flag_input = 0;

        printf("\nQual usuario deseja pesquisar(login): ");
        scanf("%20s", login_pesquisa);
        rewind(arquivo_usuario);
        if(arquivo_usuario != NULL){//verifica a existencia do usuario pesquisado
            while(fgets(linha, maxlinhas, arquivo_usuario)){
                linha[strcspn(linha, "\n")] = 0;//tirar quebra de linhas qnd for ler
                
                login_gabarito = strtok(linha, ";");
                if(strcmp(login_pesquisa, login_gabarito) == 0){
                    flag_input = 1;
                    break;
                }
            }
        }
        rewind(arquivo_posts);
        if(flag_input == 1){//se a flag for 1 o usuario existe
            strtok(NULL, ";");
            printf("\nNome do usuario pesquisado: %s \nLogin do usuario pesquisado: @%s\n",strtok(NULL, ";"),login_pesquisa);
            while(fgets(linha_post, maxlinhas, arquivo_posts)){//lê os posts
                linha_post[strcspn(linha_post, "\n")] = 0;
                login_post = strtok(linha_post, ";");
                if(strcmp(login_pesquisa,login_post)==0){//Se o post pertencer ao usuario pesquisado, imprime.
                    nome_post = strtok(NULL, ";");
                    hora_post = strtok(NULL, ";");
                    post = strtok(NULL,";");
            
                    printf("\n%s @%s - %s", nome_post, login_post, hora_post);
                    printf("\n%s\n", post);
                }
                
            }
            break;
        } else{
            printf("\nUsuario nao encontrado\n");
        }
    }
}
void mensagem_user(){
    FILE *arquivo_usuario = fopen("Arquivos_dadosUsuarios.txt","r");
    FILE *arquivo_mensagem = fopen("Arquivo_mensagem.txt", "a+");
    char *login_gabarito;
    char login_pesquisa[21];
    char linha[maxlinhas];
    char linha_mensagem[maxlinhas];
    char *login_post;
    char *destinatario;
    char *hora_post;
    char *post;
    char mensagem[] = {};
    int flag_input = 0;
    int escolha;

    time_t tempo; 
    time(&tempo);
    struct tm *tempo2 = localtime(&tempo);

    while(1){
        flag_input = 0;

        printf("\nQual usuario deseja enviar mensagem(login): ");
        scanf("%20s", login_pesquisa);
        rewind(arquivo_usuario);
        if(arquivo_usuario != NULL){//Verifica a existencia do usuario
            while(fgets(linha, maxlinhas, arquivo_usuario)){
                linha[strcspn(linha, "\n")] = 0;//tirar quebra de linhas qnd for ler
                
                login_gabarito = strtok(linha, ";");
                if(strcmp(login_pesquisa, login_gabarito) == 0){
                    flag_input = 1;
                    break;
                }
            }
        }
        while(1){
            if(flag_input == 1){//Se o usuario existir ele printa as mensagens do arquivo_mensagem pertencente aos dois
                rewind(arquivo_mensagem);
                while(fgets(linha_mensagem, maxlinhas, arquivo_mensagem)){
                    login_post = strtok(linha_mensagem, ";");
                    destinatario = strtok(NULL, ";");
                    hora_post = strtok(NULL, ";");
                    post = strtok(NULL,";");
                    //printf("login atual: %s login pesquisa: %s", usuario_atual.login, login_pesquisa);
                    //printf("\nlinha mensagem: %s", linha_mensagem);
                    if((strcmp(login_post, login_pesquisa) == 0 && (strcmp(destinatario, usuario_atual.login)) == 0) || (strcmp(login_post, usuario_atual.login) == 0 && (strcmp(destinatario, login_pesquisa)) == 0)){
                        printf("\n@%s - %s", login_post, hora_post);
                        printf("\n%s", post);
                    }
                }
            }else{
                printf("\nUsuario nao encontrado!\n");
                break;
            }

            printf("\n");
            printf("O que deseja fazer(1-Mensagem, 2-Voltar): ");
            scanf("%d", &escolha);
            
            if(escolha == 1){
                printf("Digite a mensagem: ");
                getchar();
                fgets(mensagem, 1000, stdin);
                mensagem[strcspn(mensagem, "\n")] = 0;
                while(1){
                    if(strlen(mensagem) > 1000){//Verifica se a mensagem não excede 1000 caracteres
                        printf("Digite o post com ate 1000 caracteres: ");
                        fgets(mensagem, 1000, stdin);
                        mensagem[strcspn(mensagem, "\n")] = 0;
                    }else{
                        break;
                        break;
                    }
                }
                fprintf(arquivo_mensagem, "%s;%s;%02d:%02d:%02d;%s\n", usuario_atual.login, login_pesquisa,tempo2->tm_hour, tempo2->tm_min, tempo2->tm_sec,mensagem);
                fflush(arquivo_mensagem);
            }else if(escolha == 2){
                return;
            }
        }
    }    
}
void menu_2(){//Chama o Menu de usuario logado, permitindo ele acessar suas funções
    int escolha;
    while(1){
        printf("\n+-------------------------------------------+\n");
        printf("|          +----------------------+         |\n");
        printf("|          |LOGIN: %15s|         |\n",usuario_atual.login);
        printf("|          +----------------------+         |\n");
        printf("|          +----------------------+         |\n");
        printf("|          |        MENU 2        |         |\n");
        printf("|          +----------------------+         |\n");
        printf("|          +----------------------+         |\n");
        printf("|          | 1-CADASTRAR POST     |         |\n");
        printf("|          |                      |         |\n");
        printf("|          | 2-VER FEED           |         |\n");
        printf("|          |                      |         |\n");
        printf("|          | 3-VER TRENDING TOPICS|         |\n");
        printf("|          |                      |         |\n");
        printf("|          | 4-PESQUISAR PERFIL   |         |\n");
        printf("|          |                      |         |\n");
        printf("|          | 5-MENSAGENS          |         |\n");
        printf("|          |                      |         |\n");
        printf("|          | 6-SAIR/RETORNAR      |         |\n");
        printf("|          +----------------------+         |\n");
        printf("+-------------------------------------------+\n\n");

        printf("O que deseja fazer: ");
        scanf("%d", &escolha);
        if(escolha == 1){
            cadastrar_post();
        }else if(escolha == 2){
            feed();
        }else if(escolha == 3){
            trending_topics();
        }else if(escolha == 4){
            pesquisar_user();
        }else if(escolha == 5){
            mensagem_user();
        }else if(escolha == 6){
            printf("\nVoce foi desconectado...\n");
            return;
        }
    }
}
void cadastrar_usuario(FILE *arq){
    char *login_gabarito;
    char linha[maxlinhas];
    int flag_input = 0;

    while(1){
        flag_input = 0;

        printf("Digite o login: ");
        scanf("%20s", usuarios.login);
        printf("Digite a senha: ");
        scanf("%30s", usuarios.senha);
        printf("Digite o nome de usuario: ");
        getchar();
        fgets(usuarios.nome, 100,stdin);
        usuarios.nome[strcspn(usuarios.nome, "\n")] = 0;
        
        if(strlen(usuarios.senha) < 8){//Verifica se a senha tem pelo menos 8 caracteres
            printf("Digite uma senha com pelo menos 8 caracteres\n");
            continue;
        }
        rewind(arq);
        if(arq != NULL){//
            while(fgets(linha, maxlinhas, arq)){//Inicia um loop lendo linha a linha do arquivo para fazer verificações
                linha[strcspn(linha, "\n")] = 0;//tirar quebra de linhas qnd for ler
    
                login_gabarito = strtok(linha, ";");
                if(strcmp(usuarios.login, login_gabarito) == 0){//verifica se o login que esta tentando ser cadastrado ja existe
                    printf("\nLogin ja existente!\n\n");
                    flag_input = 1;
                    break;
                }
            }
        }
        if(flag_input == 0){//Se o cadastro ocorrer corretamente as informações são tabuladas no arquivo txt e retorna ja entrando no menu_2(menu ja logado)
            fprintf(arq, "%s;%s;%s\n", usuarios.login, usuarios.senha, usuarios.nome);
            fflush(arq);
            printf("Usuario cadastrada com sucesso.\n\n");
            strcpy(usuario_atual.login, usuarios.login);
            strcpy(usuario_atual.senha, usuarios.senha);
            strcpy(usuario_atual.nome, usuarios.nome);
            printf("Cadastrado como: %s\n", usuario_atual.login);
            return;
        }
    }
}
void login(FILE *arq, int *n_tentativas){
    char login_input[20];
    char senha_input[30];
    char linha[maxlinhas];
    
    int login_flag = 0; //uma flag para fazer com que a verificacao de login continue mesmo se o login e senha nao estiverem na primeira linha
    char *login_gabarito;
    char *senha_gabarito;
    
    printf("Digite o Login: ");
    scanf("%s" , login_input);
    while(getchar() != '\n');
    printf("Digite a Senha: ");
    scanf("%s", senha_input);
    while(getchar() != '\n');

    rewind(arq);
    while(fgets(linha, maxlinhas, arq)){//passa por todas as linhas do arquivo e verifica se as credenciais estao corretas
        linha[strcspn(linha, "\n")] = 0;

        login_gabarito = strtok(linha, ";");
        senha_gabarito = strtok(NULL, ";");

        if((strcmp(login_input, login_gabarito) == 0) && (strcmp(senha_input, senha_gabarito) == 0)){
            printf("\nLogin efetuado com sucesso!\n");
            login_flag = 1;
            n_tentativas = 0;
            strcpy(usuario_atual.login, login_gabarito);
            strcpy(usuario_atual.senha, senha_gabarito);
            strcpy(usuario_atual.nome, strtok(NULL,";"));
            return;
        }
    }
    if(!login_flag){//se nao chama a mesma funcao recursivamente e com um limite de tentativas
        *n_tentativas += 1;
        if(*n_tentativas < 3){
            printf("\nLogin ou senha incorretos! Voce tem mais %d tentativas!\n\n", (3 - *n_tentativas));
            login(arq, &*n_tentativas);
        }
    }
}
int main(){
    int escolha;
    int n_tentativas = 0;

    FILE *arquivo_dadosUsuarios;
    arquivo_dadosUsuarios = fopen("Arquivos_dadosUsuarios.txt", "a+");
    if(arquivo_dadosUsuarios == NULL){
        printf("Erro ao abrir o arquivo");
        return 1;
    }

    while(1){
        menu_incial();
        n_tentativas = 0; //definimos n_tentativas para 0 toda vez que inicia o loop pq caso o usuario exceda o numero maximo de
        //tentativas ele volta para k e ai caso ele va tentar fazer o login e acerte dessa vez as tentativas precisam estar zeradas
        printf("O que deseja fazer: ");
        scanf("%d", &escolha);

        if(escolha == 1){
            cadastrar_usuario(arquivo_dadosUsuarios);
            menu_2();
        }else if(escolha == 2){
            login(arquivo_dadosUsuarios, &n_tentativas);
            if(n_tentativas == 3){
                printf("\nVoce excedeu o numero de tentativas e foi desconectado!!!\n");
                continue;
            }else{
                menu_2();
                n_tentativas = 0;
            }
        }else if(escolha == 3){
            printf("Saindo...");
            return 0;
        }
    }
    fclose(arquivo_dadosUsuarios);
}
