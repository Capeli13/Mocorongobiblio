#include <iostream>
#include <limits>
#include <string>
#include <chrono>
#include <ctime>

using namespace std;

struct data {
    int dia;
    int mes;
    int ano;
};

data dataLocal(time_t instante) {
    tm* partes = localtime(&instante);
    return {partes->tm_mday, partes->tm_mon + 1, partes->tm_year + 1900};
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

using namespace std;

struct Livro {
    int id;
    string nome;
    string autor;
    string categoria;
    int anoLancamento;
    int numPaginas;
    bool emprestado;

    void apresentar() const {
        cout << "\nId: " << id
             << "\nNome: " << nome
             << "\nAutor: " << autor
             << "\nCategoria: " << categoria
             << "\nAno de lancamento: " << anoLancamento
             << "\nNumero de paginas: " << numPaginas
             << "\nStatus: " << (emprestado ? "Indisponivel" : "Disponivel")
             << "\n";
    }
};

struct Pessoa {
    int id;
    string nome;
    string telefone;
    string email;

    void apresentar() const {
        cout << "\nId: " << id
             << "\nNome: " << nome
             << "\nTelefone: " << telefone
             << "\nEmail: " << email
             << "\n";
    }
};

struct Emprestimo {
    int id;
    Pessoa pessoa;
    Livro livro ;
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
        cout << "\nId: " << id
             << "\nPessoa: " << pessoa.nome
             << "\nLivro: " << livro.nome
             << "\nData de emprestimo: ";
        apresentarData(dataEmprestimo);
        cout << "\nData limite: ";
        apresentarData(dataLimite);
        cout
             << "\nNa lista de espera: " << espera
             << "\n";
    }
};

struct NoEmprestimo {
    Emprestimo emprestimo;
    NoEmprestimo* prox;
};

struct NoLivro {
    Livro livro;
    NoLivro* prox;
};

struct NoPessoa {
    Pessoa pessoa;
    NoPessoa* prox;
};

void limparEntrada() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

NoLivro* buscarLivro(NoLivro* iniciol, const string& nome) {
    NoLivro* atual = iniciol;

    while (atual != nullptr) {
        if (atual->livro.nome == nome) {
            return atual;
        }
        atual = atual->prox;
    }

    return nullptr;
}

NoLivro* buscarLivro(NoLivro* iniciol, int id) {
    NoLivro* atual = iniciol;
    while (atual != nullptr) {
        if (atual->livro.id == id) return atual;
        atual = atual->prox;
    }
    return nullptr;
}

NoPessoa* buscarPessoa(NoPessoa* iniciol, const string& nome) {
    NoPessoa* atual = iniciol;

    while (atual != nullptr) {
        if (atual->pessoa.nome == nome) {
            return atual;
        }
        atual = atual->prox;
    }

    return nullptr;
}

NoPessoa* buscarPessoa(NoPessoa* iniciol, int id) {
    NoPessoa* atual = iniciol;
    while (atual != nullptr) {
        if (atual->pessoa.id == id) return atual;
        atual = atual->prox;
    }
    return nullptr;
}

NoEmprestimo* buscarEmprestimo(NoEmprestimo* inicioe, int idPessoa, int idLivro) {
    NoEmprestimo* atual = inicioe;

    while (atual != nullptr) {
        if (atual->emprestimo.pessoa.id == idPessoa &&
            atual->emprestimo.livro.id == idLivro) {
            return atual;
        }
        atual = atual->prox;
    }

    return nullptr;
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
    int autoIncremente = 1;
    int autoIncrementp = 1;

    do {
        cout << "\nEscolha uma opcao:\n"
             << "1 - Listar livros\n"
             << "2 - Cadastrar novo livro\n"
             << "3 - Registrar retirada de livro\n"
             << "4 - Registrar devolucao\n"
             << "5 - Listar pessoas\n"
             << "6 - Cadastrar nova pessoa\n"
             << "0 - Sair\n";

        if (!(cin >> escolha)) {
            cout << "Opcao invalida. Digite um numero.\n";
            cin.clear();
            limparEntrada();
            continue;
        }

        limparEntrada();

        switch (escolha) {
            case 1: {
                bool encontrou = false;
                NoLivro* atual = iniciol;

                while (atual != nullptr) {
                        atual->livro.apresentar();
                        encontrou = true;
                    atual = atual->prox;
                }

                if (!encontrou) {
                    cout << "Nenhum livro disponivel.\n";
                }
                break;
            }

            case 2: {
                NoLivro* novo = new NoLivro;

                novo->livro.id = autoIncrementl;
                novo->livro.emprestado = false;
                novo->prox = nullptr;

                cout << "Insira o nome do livro: ";
                getline(cin, novo->livro.nome);

                cout << "Insira o autor: ";
                getline(cin, novo->livro.autor);

                cout << "Insira a categoria: ";
                getline(cin, novo->livro.categoria);

                cout << "Insira o ano de lancamento: ";
                cin >> novo->livro.anoLancamento;

                cout << "Insira o numero de paginas: ";
                cin >> novo->livro.numPaginas;
                limparEntrada();

                if (iniciol == nullptr) {
                    iniciol = novo;
                } else {
                    fiml->prox = novo;
                }

                fiml = novo;
                autoIncrementl++;

                cout << "Livro cadastrado com sucesso. ID: "
                     << novo->livro.id << "\n";
                break;
            }

            case 3: {
                int idPessoa, idLivro;
                cout << "Insira o id da pessoa: ";
                cin >> idPessoa;
                limparEntrada();

                NoPessoa* pessoaEncontrada = buscarPessoa(iniciop, idPessoa);
                if (pessoaEncontrada == nullptr) {
                    cout << "Pessoa nao encontrada.\n";
                    break;
                }

                cout << "Insira o id do livro: ";
                cin >> idLivro;
                limparEntrada();

                NoLivro* livroEncontrado = buscarLivro(iniciol, idLivro);
                if (livroEncontrado == nullptr) {
                    cout << "Livro nao encontrado.\n";
                    break;
                }
                if (livroEncontrado->livro.emprestado) {
                    cout << "Esse livro ja esta emprestado.\n";
                    break;
                }

                NoEmprestimo* novo = new NoEmprestimo;
                novo->emprestimo.id = autoIncremente++;
                novo->emprestimo.pessoa = pessoaEncontrada->pessoa;
                novo->emprestimo.livro = livroEncontrado->livro;
                novo->prox = nullptr;
                if (inicioe == nullptr) {
                    inicioe = novo;
                } else {
                    fime->prox = novo;
                }
                fime = novo;
                livroEncontrado->livro.emprestado = true;

                cout << "Retirada registrada.\n";
                novo->emprestimo.apresentar();
                break;
            }

            case 4: {
                int idLivro;
                cout << "Insira o id do livro devolvido: ";
                cin >> idLivro;
                limparEntrada();

                NoEmprestimo* anterior = nullptr;
                NoEmprestimo* atualEmprestimo = inicioe;
                while (atualEmprestimo != nullptr &&
                       atualEmprestimo->emprestimo.livro.id != idLivro) {
                    anterior = atualEmprestimo;
                    atualEmprestimo = atualEmprestimo->prox;
                }

                if (atualEmprestimo == nullptr) {
                    cout << "Emprestimo nao encontrado.\n";
                    break;
                }

                NoLivro* livroEncontrado = buscarLivro(iniciol, idLivro);
                if (livroEncontrado != nullptr) {
                    livroEncontrado->livro.emprestado = false;
                }
                if (anterior == nullptr) {
                    inicioe = atualEmprestimo->prox;
                } else {
                    anterior->prox = atualEmprestimo->prox;
                }
                if (fime == atualEmprestimo) fime = anterior;
                delete atualEmprestimo;
                cout << "Devolucao registrada.\n";
                break;
            }
            
                case 5: {
                bool encontrou = false;
                NoPessoa* atual = iniciop;

                while (atual != nullptr) {
                        atual->pessoa.apresentar();
                        encontrou = true;
                    atual = atual->prox;
                }

                if (!encontrou) {
                    cout << "Nenhuma pessoa disponivel.\n";
                }
                break;
            }
            
                case 6: {
                NoPessoa* novo = new NoPessoa;

                novo->pessoa.id = autoIncrementp;
                novo->prox = nullptr;

                cout << "Insira o nome da pessoa: ";
                getline(cin, novo->pessoa.nome);

                cout << "Insira o telefone: ";
                getline(cin, novo->pessoa.telefone);

                cout << "Insira o email: ";
                getline(cin, novo->pessoa.email);

                limparEntrada();

                if (iniciop == nullptr) {
                    iniciop = novo;
                } else {
                    fimp->prox = novo;
                }

                fimp = novo;
                autoIncrementp++;

                cout << "Pessoa cadastrada com sucesso. ID: "
                     << novo->pessoa.id << "\n";
                break;
            }

            case 0:
                cout << "Encerrando o programa.\n";
                break;

            default:
                cout << "Opcao invalida.\n";
        }

    } while (escolha != 0);

    NoLivro* atual = iniciol;
    while (atual != nullptr) {
        NoLivro* proximo = atual->prox;
        delete atual;
        atual = proximo;
    }
    
    

    return 0;
}
