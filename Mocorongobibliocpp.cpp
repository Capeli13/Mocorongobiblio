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

data dataLocal(time_t instante) {
    tm partes = *localtime(&instante);
    return {partes.tm_mday, partes.tm_mon + 1, partes.tm_year + 1900};
}

data dataUmaSemanaDepois(time_t instante) {
    tm partes = *localtime(&instante);
    partes.tm_mday += 7;
    mktime(&partes);
    return {partes.tm_mday, partes.tm_mon + 1, partes.tm_year + 1900};
}

void apresentarData(const data& valor) {
    cout << (valor.dia < 10 ? "0" : "") << valor.dia << "/"
         << (valor.mes < 10 ? "0" : "") << valor.mes << "/" << valor.ano;
}

bool dataAnterior(const data& a, const data& b) {
    if (a.ano != b.ano) return a.ano < b.ano;
    if (a.mes != b.mes) return a.mes < b.mes;
    return a.dia < b.dia;
}

void limparEntrada() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int lerInteiro(const string& mensagem) {
    int valor;
    while (true) {
        cout << mensagem;
        if (cin >> valor) {
            limparEntrada();
            return valor;
        }
        cout << "Entrada invalida. Digite um numero inteiro.\n";
        cin.clear();
        limparEntrada();
    }
}

string minusculas(string texto) {
    transform(texto.begin(), texto.end(), texto.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return texto;
}

struct Pessoa {
    int id;
    string nome;
    string telefone;
    string email;

    void apresentar() const {
        cout << "\nId: " << id << "\nNome: " << nome
             << "\nTelefone: " << telefone << "\nEmail: " << email << "\n";
    }
};

struct NoReserva;

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

struct NoReserva {
    Pessoa pessoa;
    NoReserva* prox;
};

struct Emprestimo {
    int id;
    Pessoa pessoa;
    Livro livro;
    data dataEmprestimo;
    data dataLimite;
    bool espera;

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

NoLivro* buscarLivro(NoLivro* inicio, const string& nome) {
    string termo = minusculas(nome);
    for (NoLivro* atual = inicio; atual != nullptr; atual = atual->prox) {
        if (minusculas(atual->livro.nome).find(termo) != string::npos) return atual;
    }
    return nullptr;
}

NoLivro* buscarLivro(NoLivro* inicio, int id) {
    for (NoLivro* atual = inicio; atual != nullptr; atual = atual->prox) {
        if (atual->livro.id == id) return atual;
    }
    return nullptr;
}

NoPessoa* buscarPessoa(NoPessoa* inicio, const string& nome) {
    for (NoPessoa* atual = inicio; atual != nullptr; atual = atual->prox) {
        if (atual->pessoa.nome == nome) return atual;
    }
    return nullptr;
}

NoPessoa* buscarPessoa(NoPessoa* inicio, int id) {
    for (NoPessoa* atual = inicio; atual != nullptr; atual = atual->prox) {
        if (atual->pessoa.id == id) return atual;
    }
    return nullptr;
}

NoEmprestimo* buscarEmprestimo(NoEmprestimo* inicio, int idPessoa, int idLivro) {
    for (NoEmprestimo* atual = inicio; atual != nullptr; atual = atual->prox) {
        if (atual->emprestimo.pessoa.id == idPessoa &&
            atual->emprestimo.livro.id == idLivro) return atual;
    }
    return nullptr;
}

bool pessoaNaFila(const Livro& livro, int idPessoa) {
    for (NoReserva* atual = livro.filaInicio; atual != nullptr; atual = atual->prox) {
        if (atual->pessoa.id == idPessoa) return true;
    }
    return false;
}

void adicionarNaFila(Livro& livro, const Pessoa& pessoa) {
    NoReserva* novo = new NoReserva;
    novo->pessoa = pessoa;
    novo->prox = nullptr;
    if (livro.filaInicio == nullptr) livro.filaInicio = novo;
    else livro.filaFim->prox = novo;
    livro.filaFim = novo;
    cout << pessoa.nome << " foi adicionada ao fim da fila de reserva.\n";
}

void criarEmprestimo(NoEmprestimo*& inicio, NoEmprestimo*& fim,
                     int& autoIncremento, const Pessoa& pessoa, Livro& livro) {
    NoEmprestimo* novo = new NoEmprestimo;
    novo->emprestimo.id = autoIncremento++;
    novo->emprestimo.pessoa = pessoa;
    novo->emprestimo.livro = livro;
    novo->prox = nullptr;
    if (inicio == nullptr) inicio = novo;
    else fim->prox = novo;
    fim = novo;
    --livro.quantidade;
    livro.emprestado = (livro.quantidade == 0);
    cout << "Emprestimo registrado.\n";
    novo->emprestimo.apresentar();
}

void apresentarFila(const Livro& livro) {
    cout << "Fila de espera: ";
    if (livro.filaInicio == nullptr) {
        cout << "vazia\n";
        return;
    }
    for (NoReserva* atual = livro.filaInicio; atual != nullptr; atual = atual->prox) {
        cout << atual->pessoa.nome << " (ID " << atual->pessoa.id << ")";
        if (atual->prox != nullptr) cout << " -> ";
    }
    cout << "\n";
}

int main() {
    NoLivro* iniciol = nullptr;
    NoLivro* fiml = nullptr;
    NoPessoa* iniciop = nullptr;
    NoPessoa* fimp = nullptr;
    NoEmprestimo* inicioe = nullptr;
    NoEmprestimo* fime = nullptr;

    int escolha;
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
            case 1: {
                if (iniciol == nullptr) cout << "Nenhum livro cadastrado.\n";
                for (NoLivro* atual = iniciol; atual != nullptr; atual = atual->prox) {
                    atual->livro.apresentar();
                    apresentarFila(atual->livro);
                }
                break;
            }
            case 2: {
                NoLivro* novo = new NoLivro;
                novo->livro.id = autoIncrementl++;
                novo->livro.filaInicio = nullptr;
                novo->livro.filaFim = nullptr;
                novo->prox = nullptr;

                cout << "Insira o nome do livro: ";
                getline(cin, novo->livro.nome);
                cout << "Insira o autor: ";
                getline(cin, novo->livro.autor);
                cout << "Insira a categoria: ";
                getline(cin, novo->livro.categoria);
                novo->livro.anoLancamento = lerInteiro("Insira o ano de lancamento: ");
                novo->livro.numPaginas = lerInteiro("Insira o numero de paginas: ");
                do {
                    novo->livro.quantidadeTotal = lerInteiro("Quantidade de exemplares: ");
                    if (novo->livro.quantidadeTotal <= 0)
                        cout << "A quantidade deve ser maior que zero.\n";
                } while (novo->livro.quantidadeTotal <= 0);
                novo->livro.quantidade = novo->livro.quantidadeTotal;
                novo->livro.emprestado = false;

                if (iniciol == nullptr) iniciol = novo;
                else fiml->prox = novo;
                fiml = novo;
                cout << "Livro cadastrado com sucesso. ID: " << novo->livro.id << "\n";
                break;
            }
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
                if (livro->livro.quantidade == 0 || livro->livro.filaInicio != nullptr) {
                    adicionarNaFila(livro->livro, pessoa->pessoa);
                } else {
                    criarEmprestimo(inicioe, fime, autoIncremente,
                                    pessoa->pessoa, livro->livro);
                }
                break;
            }
            case 4: {
                int idLivro = lerInteiro("Insira o id do livro devolvido: ");
                NoLivro* livro = buscarLivro(iniciol, idLivro);
                if (livro == nullptr) {
                    cout << "Livro nao encontrado.\n";
                    break;
                }

                bool encontrouEmprestimo = false;
                cout << "\nEmprestimos ativos deste livro:\n";
                for (NoEmprestimo* atual = inicioe; atual != nullptr; atual = atual->prox) {
                    if (atual->emprestimo.livro.id == idLivro) {
                        atual->emprestimo.apresentar();
                        encontrouEmprestimo = true;
                    }
                }
                if (!encontrouEmprestimo) {
                    cout << "Nao ha emprestimo ativo desse livro.\n";
                    break;
                }

                int idEmprestimo = lerInteiro("Digite o id do emprestimo que sera devolvido: ");
                NoEmprestimo* anterior = nullptr;
                NoEmprestimo* emprestimo = inicioe;
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

                if (anterior == nullptr) inicioe = emprestimo->prox;
                else anterior->prox = emprestimo->prox;
                if (fime == emprestimo) fime = anterior;
                delete emprestimo;
                ++livro->livro.quantidade;
                livro->livro.emprestado = (livro->livro.quantidade == 0);
                cout << "Devolucao registrada.\n";

                if (livro->livro.filaInicio != nullptr) {
                    NoReserva* proximaReserva = livro->livro.filaInicio;
                    livro->livro.filaInicio = proximaReserva->prox;
                    if (livro->livro.filaInicio == nullptr) livro->livro.filaFim = nullptr;
                    Pessoa proximaPessoa = proximaReserva->pessoa;
                    delete proximaReserva;
                    cout << "Chamando a proxima pessoa da fila.\n";
                    criarEmprestimo(inicioe, fime, autoIncremente,
                                    proximaPessoa, livro->livro);
                }
                break;
            }
            case 5: {
                if (iniciop == nullptr) cout << "Nenhuma pessoa cadastrada.\n";
                for (NoPessoa* atual = iniciop; atual != nullptr; atual = atual->prox)
                    atual->pessoa.apresentar();
                break;
            }
            case 6: {
                NoPessoa* novo = new NoPessoa;
                novo->pessoa.id = autoIncrementp++;
                novo->prox = nullptr;
                cout << "Insira o nome da pessoa: ";
                getline(cin, novo->pessoa.nome);
                cout << "Insira o telefone: ";
                getline(cin, novo->pessoa.telefone);
                cout << "Insira o email: ";
                getline(cin, novo->pessoa.email);
                if (iniciop == nullptr) iniciop = novo;
                else fimp->prox = novo;
                fimp = novo;
                cout << "Pessoa cadastrada com sucesso. ID: " << novo->pessoa.id << "\n";
                break;
            }
            case 7: {
                string termo;
                cout << "Digite o titulo ou parte dele: ";
                getline(cin, termo);
                termo = minusculas(termo);
                bool encontrou = false;
                for (NoLivro* atual = iniciol; atual != nullptr; atual = atual->prox) {
                    if (minusculas(atual->livro.nome).find(termo) != string::npos) {
                        atual->livro.apresentar();
                        apresentarFila(atual->livro);
                        encontrou = true;
                    }
                }
                if (!encontrou) cout << "Nenhum titulo correspondente encontrado.\n";
                break;
            }
            case 8: {
                if (inicioe == nullptr) {
                    cout << "Nao ha emprestimos ativos.\n";
                    break;
                }
                data hoje = dataLocal(time(nullptr));
                cout << "\nRelatorio de emprestimos ativos\n";
                for (NoEmprestimo* atual = inicioe; atual != nullptr; atual = atual->prox) {
                    atual->emprestimo.apresentar();
                    if (dataAnterior(atual->emprestimo.dataLimite, hoje))
                        cout << "Situacao: ATRASADO\n";
                    else
                        cout << "Situacao: dentro do prazo\n";
                }
                break;
            }
            case 9: {
                bool encontrou = false;
                cout << "\nRelatorio de filas de espera\n";
                for (NoLivro* atual = iniciol; atual != nullptr; atual = atual->prox) {
                    if (atual->livro.filaInicio != nullptr) {
                        cout << "Livro: " << atual->livro.nome << " (ID "
                             << atual->livro.id << ")\n";
                        apresentarFila(atual->livro);
                        encontrou = true;
                    }
                }
                if (!encontrou) cout << "Nao ha reservas pendentes.\n";
                break;
            }
            case 0:
                cout << "Encerrando o programa.\n";
                break;
            default:
                cout << "Opcao invalida.\n";
        }
    } while (escolha != 0);

    while (iniciol != nullptr) {
        NoLivro* proximoLivro = iniciol->prox;
        NoReserva* reserva = iniciol->livro.filaInicio;
        while (reserva != nullptr) {
            NoReserva* proximaReserva = reserva->prox;
            delete reserva;
            reserva = proximaReserva;
        }
        delete iniciol;
        iniciol = proximoLivro;
    }
    while (iniciop != nullptr) {
        NoPessoa* proximaPessoa = iniciop->prox;
        delete iniciop;
        iniciop = proximaPessoa;
    }
    while (inicioe != nullptr) {
        NoEmprestimo* proximoEmprestimo = inicioe->prox;
        delete inicioe;
        inicioe = proximoEmprestimo;
    }
    return 0;
}
