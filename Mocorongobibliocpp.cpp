#include <algorithm>
#include <cctype>   
#include <ctime>    
#include <iostream> 
#include <limits>   
#include <string>   
using namespace std;


struct data {
    int dia;
    int mes;
    int ano;
};

// retorna a data de hoje
data dataLocal(time_t instante) {
    tm partes = *localtime(&instante);

    // converte os valores da biblioteca para o formato convencional
    return {partes.tm_mday, partes.tm_mon + 1, partes.tm_year + 1900};
}

// calcula a data de devolucao
data dataUmaSemanaDepois(time_t instante) {
    tm partes = *localtime(&instante);
 
    partes.tm_mday += 7;

    // ajusta automaticamente mes e ano caso necessario
    mktime(&partes);

    return {partes.tm_mday, partes.tm_mon + 1, partes.tm_year + 1900};
}

// apresenta uma data no formato dd/mm/aaaa
void apresentarData(const data& valor) {
    cout << (valor.dia < 10 ? "0" : "") << valor.dia << "/"
         << (valor.mes < 10 ? "0" : "") << valor.mes << "/" << valor.ano;
}

// verifica se uma data e anterior a outra
bool dataAnterior(const data& a, const data& b) {
    if (a.ano != b.ano) return a.ano < b.ano;
    if (a.mes != b.mes) return a.mes < b.mes;
    return a.dia < b.dia;
}

// descarta os caracteres restantes da entrada ate encontrar uma quebra de linha
void limparEntrada() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// realiza a leitura de um numero inteiro
int lerInteiro(const string& mensagem) {
    int valor;

    while (true) {
        cout << mensagem;

        if (cin >> valor) {
            limparEntrada();
            return valor;
        }

        cout << "Entrada invalida. Digite um numero inteiro.\n";

        // limpa o estado de erro do cin
        cin.clear();

        // descarta a entrada invalida
        limparEntrada();
    }
}

// converte todos os caracteres de uma string para letras minusculas
string minusculas(string texto) {
    transform(texto.begin(), texto.end(), texto.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });

    return texto;
}

// estrutura que representa uma pessoa cadastrada na biblioteca
struct Pessoa {
    int id;
    string nome;
    string telefone;
    string email;

    // apresenta os dados cadastrados da pessoa
    void apresentar() const {
        cout << "\nId: " << id << "\nNome: " << nome
             << "\nTelefone: " << telefone << "\nEmail: " << email << "\n";
    }
};

// declaracao antecipada da estrutura NoReserva
// necessaria porque Livro utiliza um ponteiro para NoReserva
struct NoReserva;

// estrutura que representa um livro do acervo
struct Livro {
    int id;
    string nome;
    string autor;
    string categoria;
    int anoLancamento;
    int numPaginas;

    int quantidade;

    int quantidadeTotal;

    bool emprestado;

    // ponteiros que controlam o inicio e o fim da fila de espera
    NoReserva* filaInicio;
    NoReserva* filaFim;

    void apresentar() const {
        cout << "\nId: " << id << "\nNome: " << nome
             << "\nAutor: " << autor << "\nCategoria: " << categoria
             << "\nAno de lancamento: " << anoLancamento
             << "\nNumero de paginas: " << numPaginas
             << "\nQuantidade de livros disponiveis: " << quantidade
             << " de " << quantidadeTotal
             << "\nStatus: " << (quantidade > 0 ? "Disponivel" : "Indisponivel")
             << "\n";
    }
};

// no da lista encadeada que armazena as pessoas na fila de espera
struct NoReserva {
    Pessoa pessoa;
    NoReserva* prox;
};

// estrutura que representa um emprestimo realizado
struct Emprestimo {
    int id;
    Pessoa pessoa;
    Livro livro;

    data dataEmprestimo;

    data dataLimite;

    bool espera;

    // construtor que inicializa automaticamente as datas
    Emprestimo() {
        time_t agora = time(nullptr);

        dataEmprestimo = dataLocal(agora);

        dataLimite = dataUmaSemanaDepois(agora);

        espera = false;
    }

    void apresentar() const {
        cout << "\nId do emprestimo: " << id
             << "\nPessoa: " << pessoa.nome << " (id " << pessoa.id << ")"
             << "\nLivro: " << livro.nome << " (id " << livro.id << ")"
             << "\nData de emprestimo: ";

        apresentarData(dataEmprestimo);

        cout << "\nData limite: ";

        apresentarData(dataLimite);

        cout << "\nNa lista de espera: " << espera << "\n";
    }
};

struct NoLivro {
    Livro livro;
    NoLivro* prox;
};

struct NoPessoa {
    Pessoa pessoa;
    NoPessoa* prox;
};

struct NoEmprestimo {
    Emprestimo emprestimo;
    NoEmprestimo* prox;
};

// pesquisa um livro pelo nome ou por parte dele
NoLivro* buscarLivro(NoLivro* inicio, const string& nome) {

    string termo = minusculas(nome);

    // percorre toda a lista de livros
    for (NoLivro* atual = inicio; atual != nullptr; atual = atual->prox) {

        // verifica se o nome do livro contem o termo pesquisado
        if (minusculas(atual->livro.nome).find(termo) != string::npos)
            return atual;
    }
    
    return nullptr;
}

// pesquisa um livro utilizando seu identificador
NoLivro* buscarLivro(NoLivro* inicio, int id) {
    for (NoLivro* atual = inicio; atual != nullptr; atual = atual->prox) {

        if (atual->livro.id == id) return atual;
    }

    return nullptr;
}

// pesquisa uma pessoa pelo nome
NoPessoa* buscarPessoa(NoPessoa* inicio, const string& nome) {
    for (NoPessoa* atual = inicio; atual != nullptr; atual = atual->prox) {
        if (atual->pessoa.nome == nome) return atual;
    }

    return nullptr;
}

// pesquisa uma pessoa utilizando seu identificador
NoPessoa* buscarPessoa(NoPessoa* inicio, int id) {
    for (NoPessoa* atual = inicio; atual != nullptr; atual = atual->prox) {
        if (atual->pessoa.id == id) return atual;
    }

    return nullptr;
}

// procura um emprestimo utilizando o ID da pessoa e o ID do livro
NoEmprestimo* buscarEmprestimo(NoEmprestimo* inicio, int idPessoa, int idLivro) {
    for (NoEmprestimo* atual = inicio; atual != nullptr; atual = atual->prox) {

        if (atual->emprestimo.pessoa.id == idPessoa &&
            atual->emprestimo.livro.id == idLivro)
            return atual;
    }

    return nullptr;
}

// verifica se uma pessoa ja esta na fila de espera de determinado livro
bool pessoaNaFila(const Livro& livro, int idPessoa) {
    for (NoReserva* atual = livro.filaInicio; atual != nullptr; atual = atual->prox) {

        if (atual->pessoa.id == idPessoa) return true;
    }

    return false;
}

// adiciona uma pessoa ao final da fila de espera de um livro
void adicionarNaFila(Livro& livro, const Pessoa& pessoa) {

    NoReserva* novo = new NoReserva;

    novo->pessoa = pessoa;
    novo->prox = nullptr;

    if (livro.filaInicio == nullptr)
        livro.filaInicio = novo;
    else
        livro.filaFim->prox = novo;

    livro.filaFim = novo;

    cout << pessoa.nome << " foi adicionada ao fim da fila de reserva.\n";
}

// cria e registra um novo emprestimo
void criarEmprestimo(NoEmprestimo*& inicio, NoEmprestimo*& fim,
                     int& autoIncremento, const Pessoa& pessoa, Livro& livro) {

    NoEmprestimo* novo = new NoEmprestimo;

    novo->emprestimo.id = autoIncremento++;

    novo->emprestimo.pessoa = pessoa;
    novo->emprestimo.livro = livro;

    novo->prox = nullptr;

    // insere o emprestimo no final da lista
    if (inicio == nullptr)
        inicio = novo;
    else
        fim->prox = novo;

    fim = novo;

    --livro.quantidade;

    livro.emprestado = (livro.quantidade == 0);

    cout << "Emprestimo registrado.\n";

    novo->emprestimo.apresentar();
}

// apresenta todas as pessoas que estao aguardando determinado livro
void apresentarFila(const Livro& livro) {

    cout << "Fila de espera: ";

    if (livro.filaInicio == nullptr) {
        cout << "vazia\n";
        return;
    }

    for (NoReserva* atual = livro.filaInicio;
         atual != nullptr;
         atual = atual->prox) {

        cout << atual->pessoa.nome << " (ID " << atual->pessoa.id << ")";

        if (atual->prox != nullptr) cout << " -> ";
    }

    cout << "\n";
}

int main() {

    // ponteiros para controlar o inicio e o final das listas
    NoLivro* iniciol = nullptr;
    NoLivro* fiml = nullptr;

    NoPessoa* iniciop = nullptr;
    NoPessoa* fimp = nullptr;

    NoEmprestimo* inicioe = nullptr;
    NoEmprestimo* fime = nullptr;

    int escolha;

    // contadores automaticos para gerar os identificadores
    int autoIncrementl = 1;
    int autoIncrementp = 1;
    int autoIncremente = 1;

    do {

        cout << "\nEscolha uma opcao:\n"
             << "1 - Listar livros\n"
             << "2 - Cadastrar novo livro\n"
             << "3 - Registrar retirada ou reserva de livro\n"
             << "4 - Registrar devolucao\n"
             << "5 - Listar pessoas\n"
             << "6 - Cadastrar nova pessoa\n"
             << "7 - Pesquisar titulos\n"
             << "8 - Relatorio de emprestimos ativos e atrasados\n"
             << "9 - Relatorio de filas de espera\n"
             << "0 - Sair\n";

        escolha = lerInteiro("Opcao: ");

        switch (escolha) {

            // opcao 1: apresenta todos os livros cadastrados
            case 1: {

                if (iniciol == nullptr)
                    cout << "Nenhum livro cadastrado.\n";

                // percorre a lista de livros
                for (NoLivro* atual = iniciol;
                     atual != nullptr;
                     atual = atual->prox) {

                    atual->livro.apresentar();

                    apresentarFila(atual->livro);
                }

                break;
            }

            // opcao 2: cadastra um novo livro
            case 2: {

                NoLivro* novo = new NoLivro;

                novo->livro.id = autoIncrementl++;

                // inicializa os ponteiros da fila de espera
                novo->livro.filaInicio = nullptr;
                novo->livro.filaFim = nullptr;

                novo->prox = nullptr;

                // solicita as informacoes do livro
                cout << "Insira o nome do livro: ";
                getline(cin, novo->livro.nome);

                cout << "Insira o autor: ";
                getline(cin, novo->livro.autor);

                cout << "Insira a categoria: ";
                getline(cin, novo->livro.categoria);

                novo->livro.anoLancamento =
                    lerInteiro("Insira o ano de lancamento: ");

                novo->livro.numPaginas =
                    lerInteiro("Insira o numero de paginas: ");

                do {
                    novo->livro.quantidadeTotal =
                        lerInteiro("Quantidade de exemplares: ");

                    if (novo->livro.quantidadeTotal <= 0)
                        cout << "A quantidade deve ser maior que zero.\n";

                } while (novo->livro.quantidadeTotal <= 0);

                novo->livro.quantidade = novo->livro.quantidadeTotal;
                novo->livro.emprestado = false;

                // insere o livro no final da lista encadeada
                if (iniciol == nullptr)
                    iniciol = novo;
                else
                    fiml->prox = novo;

                fiml = novo;

                cout << "Livro cadastrado com sucesso. ID: "
                     << novo->livro.id << "\n";

                break;
            }

            // opcao 3: registra um emprestimo ou adiciona uma pessoa na fila
            case 3: {

                int idPessoa = lerInteiro("Insira o id da pessoa: ");

                NoPessoa* pessoa = buscarPessoa(iniciop, idPessoa);

                if (pessoa == nullptr) {
                    cout << "Pessoa nao encontrada.\n";
                    break;
                }

                int idLivro = lerInteiro("Insira o id do livro: ");

                NoLivro* livro = buscarLivro(iniciol, idLivro);

                if (livro == nullptr) {
                    cout << "Livro nao encontrado.\n";
                    break;
                }

                if (buscarEmprestimo(inicioe, idPessoa, idLivro) != nullptr) {
                    cout << "Essa pessoa ja tem um exemplar deste livro emprestado.\n";
                    break;
                }

                if (pessoaNaFila(livro->livro, idPessoa)) {
                    cout << "Essa pessoa ja esta na fila de espera deste livro.\n";
                    break;
                }

                // a pessoa sera adicionada ao final da fila de espera se não houver exemplares
                if (livro->livro.quantidade == 0 ||
                    livro->livro.filaInicio != nullptr) {

                    adicionarNaFila(livro->livro, pessoa->pessoa);

                } else {

                    // cria um emprestimo do livro para o primeiro da fila
                    criarEmprestimo(inicioe, fime, autoIncremente,
                                    pessoa->pessoa, livro->livro);
                }

                break;
            }

            // opcao 4: registra a devolucao de um livro
            case 4: {

                int idLivro = lerInteiro("Insira o id do livro devolvido: ");

                NoLivro* livro = buscarLivro(iniciol, idLivro);

                if (livro == nullptr) {
                    cout << "Livro nao encontrado.\n";
                    break;
                }

                bool encontrouEmprestimo = false;

                cout << "\nEmprestimos ativos deste livro:\n";

                // apresenta todos os emprestimos ativos do livro selecionado
                for (NoEmprestimo* atual = inicioe;
                     atual != nullptr;
                     atual = atual->prox) {

                    if (atual->emprestimo.livro.id == idLivro) {
                        atual->emprestimo.apresentar();
                        encontrouEmprestimo = true;
                    }
                }

                if (!encontrouEmprestimo) {
                    cout << "Nao ha emprestimo ativo desse livro.\n";
                    break;
                }

                int idEmprestimo =
                    lerInteiro("Digite o id do emprestimo que sera devolvido: ");

                NoEmprestimo* anterior = nullptr;
                NoEmprestimo* emprestimo = inicioe;

                // procura o emprestimo utilizando o ID do livro e do emprestimo
                while (emprestimo != nullptr &&
                       !(emprestimo->emprestimo.livro.id == idLivro &&
                         emprestimo->emprestimo.id == idEmprestimo)) {

                    anterior = emprestimo;
                    emprestimo = emprestimo->prox;
                }

                if (emprestimo == nullptr) {
                    cout << "ID de emprestimo invalido para esse livro.\n";
                    break;
                }

                // remove o emprestimo da lista encadeada
                if (anterior == nullptr)
                    inicioe = emprestimo->prox;
                else
                    anterior->prox = emprestimo->prox;

                // atualiza o ponteiro final caso o ultimo elemento seja removido
                if (fime == emprestimo)
                    fime = anterior;

                delete emprestimo;

                ++livro->livro.quantidade;

                livro->livro.emprestado = (livro->livro.quantidade == 0);

                cout << "Devolucao registrada.\n";

                // verifica se existem pessoas aguardando o livro
                if (livro->livro.filaInicio != nullptr) {

                    NoReserva* proximaReserva = livro->livro.filaInicio;

                    livro->livro.filaInicio = proximaReserva->prox;

                    // se a fila ficar vazia, atualiza tambem o ponteiro final
                    if (livro->livro.filaInicio == nullptr)
                        livro->livro.filaFim = nullptr;

                    // guarda os dados da pessoa antes de liberar o no
                    Pessoa proximaPessoa = proximaReserva->pessoa;

                    delete proximaReserva;

                    cout << "Chamando a proxima pessoa da fila.\n";

                    // registra automaticamente o emprestimo para a proxima pessoa
                    criarEmprestimo(inicioe, fime, autoIncremente,
                                    proximaPessoa, livro->livro);
                }

                break;
            }

            // opcao 5: apresenta todas as pessoas cadastradas
            case 5: {

                if (iniciop == nullptr)
                    cout << "Nenhuma pessoa cadastrada.\n";

                // percorre a lista de pessoas
                for (NoPessoa* atual = iniciop;
                     atual != nullptr;
                     atual = atual->prox) {

                    atual->pessoa.apresentar();
                }

                break;
            }

            // opcao 6: cadastra uma nova pessoa
            case 6: {

                // aloca memoria para um novo cadastro
                NoPessoa* novo = new NoPessoa;

                novo->pessoa.id = autoIncrementp++;

                novo->prox = nullptr;

                // solicita os dados pessoais
                cout << "Insira o nome da pessoa: ";
                getline(cin, novo->pessoa.nome);

                cout << "Insira o telefone: ";
                getline(cin, novo->pessoa.telefone);

                cout << "Insira o email: ";
                getline(cin, novo->pessoa.email);

                // adiciona a pessoa no final da lista
                if (iniciop == nullptr)
                    iniciop = novo;
                else
                    fimp->prox = novo;

                fimp = novo;

                cout << "Pessoa cadastrada com sucesso. ID: "
                     << novo->pessoa.id << "\n";

                break;
            }

            // opcao 7: pesquisa livros pelo titulo ou parte dele
            case 7: {

                string termo;

                cout << "Digite o titulo ou parte dele: ";
                getline(cin, termo);

                // converte o termo pesquisado para minusculas
                termo = minusculas(termo);

                bool encontrou = false;

                // percorre todos os livros cadastrados
                for (NoLivro* atual = iniciol;
                     atual != nullptr;
                     atual = atual->prox) {

                    // procura o termo dentro do nome de cada livro
                    if (minusculas(atual->livro.nome).find(termo) != string::npos) {

                        atual->livro.apresentar();
                        apresentarFila(atual->livro);

                        encontrou = true;
                    }
                }

                // informa caso nenhum resultado seja encontrado
                if (!encontrou)
                    cout << "Nenhum titulo correspondente encontrado.\n";

                break;
            }

            // opcao 8: gera relatorio de emprestimos ativos e atrasados
            case 8: {

                if (inicioe == nullptr) {
                    cout << "Nao ha emprestimos ativos.\n";
                    break;
                }

                // obtém a data atual para comparar com os prazos de devolucao
                data hoje = dataLocal(time(nullptr));

                cout << "\nRelatorio de emprestimos ativos\n";

                for (NoEmprestimo* atual = inicioe;
                     atual != nullptr;
                     atual = atual->prox) {

                    atual->emprestimo.apresentar();

                    // compara a data limite com a data atual
                    if (dataAnterior(atual->emprestimo.dataLimite, hoje))
                        cout << "Situacao: ATRASADO\n";
                    else
                        cout << "Situacao: dentro do prazo\n";
                }

                break;
            }

            // opcao 9: gera relatorio das filas de espera
            case 9: {

                bool encontrou = false;

                cout << "\nRelatorio de filas de espera\n";

                // percorre todos os livros cadastrados
                for (NoLivro* atual = iniciol;
                     atual != nullptr;
                     atual = atual->prox) {

                    // verifica quais livros possuem pessoas aguardando
                    if (atual->livro.filaInicio != nullptr) {

                        cout << "Livro: " << atual->livro.nome << " (ID "
                             << atual->livro.id << ")\n";

                        // apresenta as pessoas que estao aguardando
                        apresentarFila(atual->livro);

                        encontrou = true;
                    }
                }

                // informa caso nao existam filas de espera
                if (!encontrou)
                    cout << "Nao ha reservas pendentes.\n";

                break;
            }

            // opcao 0: encerra o programa
            case 0:
                cout << "Encerrando o programa.\n";
                break;
                
            default:
                cout << "Opcao invalida.\n";
        }

    } while (escolha != 0);

    // percorre e libera todos os livros cadastrados
    while (iniciol != nullptr) {

        NoLivro* proximoLivro = iniciol->prox;

        // antes de excluir o livro, libera todos os nos da sua fila de espera
        NoReserva* reserva = iniciol->livro.filaInicio;

        while (reserva != nullptr) {

            NoReserva* proximaReserva = reserva->prox;

            delete reserva;

            reserva = proximaReserva;
        }

        // libera o no do livro atual
        delete iniciol;

        iniciol = proximoLivro;
    }

    // libera todos os nos da lista de pessoas
    while (iniciop != nullptr) {

        NoPessoa* proximaPessoa = iniciop->prox;

        delete iniciop;

        iniciop = proximaPessoa;
    }

    // libera todos os nos da lista de emprestimos
    while (inicioe != nullptr) {

        NoEmprestimo* proximoEmprestimo = inicioe->prox;

        delete inicioe;

        inicioe = proximoEmprestimo;
    }

    return 0;
}
