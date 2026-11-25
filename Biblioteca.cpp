#include <algorithm>  // Algoritmos prontos, como ordenacao e busca.
#include <cctype>     // Funcoes para testar ou converter caracteres, como isdigit e tolower.
#include <ctime>      // Funcoes relacionadas a data e hora do sistema.
#include <fstream>    // Leitura e gravacao de arquivos, como ifstream e ofstream.
#include <iostream>   // Entrada e saida padrao, como cin e cout.
#include <limits>     // Limites de tipos numericos; por exemplo, numeric_limits<streamsize>::max().
#include <sstream>    // Leitura e montagem de dados usando fluxos de texto.
#include <string>     // Tipo string e operacoes com textos.
#include <vector>     // Contem o vector, usado para armazenar uma sequencia de elementos.

using namespace std;

struct data {
    int dia;
    int mes;
    int ano;
};

// Retorna a data de hoje
data dataLocal(time_t instante) {
    tm partes = *localtime(&instante);

    return {
        partes.tm_mday,
        partes.tm_mon + 1,
        partes.tm_year + 1900
    };
}

// Retorna a data de hoje mas com uma semana a mais, usado para a data final de devolucao
data dataUmaSemanaDepois(time_t instante) {
    tm partes = *localtime(&instante);

    partes.tm_mday += 7;

    mktime(&partes);

    return {
        partes.tm_mday,
        partes.tm_mon + 1,
        partes.tm_year + 1900
    };
}

// Imprime a data
void apresentarData(const data& valor) {
    cout << (valor.dia < 10 ? "0" : "")
         << valor.dia << "/"
         << (valor.mes < 10 ? "0" : "")
         << valor.mes << "/"
         << valor.ano;
}

// Verifica qual da data veio primeiro
bool dataAnterior(const data& a, const data& b) {
    if (a.ano != b.ano) return a.ano < b.ano;
    if (a.mes != b.mes) return a.mes < b.mes;

    return a.dia < b.dia;
}

// Permite calcular a quantidadede dias entre duas datas
int diasDesdeInicio(const data& valor) {
    int ano = valor.ano;
    int mes = valor.mes;
    ano -= mes <= 2;
    const int era = (ano >= 0 ? ano : ano - 399) / 400;
    const unsigned anoEra = static_cast<unsigned>(ano - era * 400);
    const unsigned mesAjustado = static_cast<unsigned>(mes + (mes > 2 ? -3 : 9));
    const unsigned diaAno = (153 * mesAjustado + 2) / 5 + valor.dia - 1;
    const unsigned diaEra = anoEra * 365 + anoEra / 4 - anoEra / 100 + diaAno;
    return era * 146097 + static_cast<int>(diaEra);
}

// Utiliza o comando acima
int diasEntre(const data& inicio, const data& fim) {
    return diasDesdeInicio(fim) - diasDesdeInicio(inicio);
}

// Ignora a quebra de linha após digitar um numero
void limparEntrada() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Verifica se o ususario digitou um numero
int lerInteiro(const string& mensagem) {
    int valor;

    while (true) {
        cout << mensagem;

        if (cin >> valor) {
            limparEntrada();
            return valor;
        }

        cout << "entrada invalida. digite um numero inteiro.\n";

        cin.clear();
        limparEntrada();
    }
}

// Converte um texto para letra minuscula
string minusculas(string texto) {
    transform(
        texto.begin(),
        texto.end(),
        texto.begin(),
        [](unsigned char c) {
            return static_cast<char>(tolower(c));
        }
    );

    return texto;
}

struct Pessoa {
    int id;
    string nome;
    string telefone;
    string email;

    void apresentar() const {
        cout << "\nid: " << id
             << "\nnome: " << nome
             << "\ntelefone: " << telefone
             << "\nemail: " << email
             << "\n";
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
        cout << "\nid: " << id
             << "\nnome: " << nome
             << "\nautor: " << autor
             << "\ncategoria: " << categoria
             << "\nano de lancamento: " << anoLancamento
             << "\nnumero de paginas: " << numPaginas
             << "\nexemplares disponiveis: " << quantidade
             << " de " << quantidadeTotal
             << "\nstatus: "
             << (quantidade > 0 ? "disponivel" : "indisponivel")
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
        cout << "\nid do emprestimo: " << id
             << "\npessoa: " << pessoa.nome
             << " (id " << pessoa.id << ")"
             << "\nlivro: " << livro.nome
             << " (id " << livro.id << ")"
             << "\ndata de emprestimo: ";

        apresentarData(dataEmprestimo);

        cout << "\ndata limite: ";

        apresentarData(dataLimite);

        cout << "\nna lista de espera: "
             << (espera ? "sim" : "nao")
             << "\n";
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

// busca livro pelo nome

NoLivro* buscarLivro(NoLivro* inicio, const string& nome) {
    string termo = minusculas(nome);

    for (NoLivro* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        if (minusculas(atual->livro.nome).find(termo)
            != string::npos) {

            return atual;
        }
    }

    return nullptr;
}

// busca livro pelo id

NoLivro* buscarLivro(NoLivro* inicio, int id) {
    for (NoLivro* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        if (atual->livro.id == id)
            return atual;
    }

    return nullptr;
}

// busca pessoa pelo nome

NoPessoa* buscarPessoa(NoPessoa* inicio, const string& nome) {
    for (NoPessoa* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        if (atual->pessoa.nome == nome)
            return atual;
    }

    return nullptr;
}

// busca pessoa pelo id

NoPessoa* buscarPessoa(NoPessoa* inicio, int id) {
    for (NoPessoa* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        if (atual->pessoa.id == id)
            return atual;
    }

    return nullptr;
}

// busca emprestimo pelo id da pessoa e do livro

NoEmprestimo* buscarEmprestimo(
    NoEmprestimo* inicio,
    int idPessoa,
    int idLivro
) {
    for (NoEmprestimo* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        if (atual->emprestimo.pessoa.id == idPessoa &&
            atual->emprestimo.livro.id == idLivro) {

            return atual;
        }
    }

    return nullptr;
}

// Coloca aspas em volta do valor para gravá-lo como um campo CSV
string campoCSV(const string& valor) {
    string resultado = "\"";
    for (char caractere : valor) {
        if (caractere == '\"') resultado += "\"\"";
        else if (caractere != '\r' && caractere != '\n') resultado += caractere;
    }
    resultado += "\"";
    return resultado;
}

// Divide uma linha CSV em seus campos
vector<string> separarCSV(const string& linha) {
    vector<string> campos;
    string campo;
    bool entreAspas = false;

    for (size_t i = 0; i < linha.size(); ++i) {
        char caractere = linha[i];
        if (caractere == '\"') {
            if (entreAspas && i + 1 < linha.size() && linha[i + 1] == '\"') {
                campo += '\"';
                ++i;
            } else {
                entreAspas = !entreAspas;
            }
        } else if (caractere == ',' && !entreAspas) {
            campos.push_back(campo);
            campo.clear();
        } else {
            campo += caractere;
        }
    }
    campos.push_back(campo);
    return campos;
}

// Grava livros, pessoas, emprestimos e reservas em arquivos CSV
void salvarDados(NoLivro* livros, NoPessoa* pessoas, NoEmprestimo* emprestimos) {
    ofstream arquivoLivros("livros.csv");
    ofstream arquivoPessoas("pessoas.csv");
    ofstream arquivoEmprestimos("emprestimos.csv");
    ofstream arquivoReservas("reservas.csv");

    if (!arquivoLivros || !arquivoPessoas || !arquivoEmprestimos || !arquivoReservas) {
        cout << "erro ao salvar os arquivos CSV.\n";
        return;
    }

    arquivoLivros << "id,nome,autor,categoria,ano,paginas,disponiveis,total\n";
    for (NoLivro* atual = livros; atual != nullptr; atual = atual->prox) {
        const Livro& livro = atual->livro;
        arquivoLivros << livro.id << ',' << campoCSV(livro.nome) << ','
                      << campoCSV(livro.autor) << ',' << campoCSV(livro.categoria) << ','
                      << livro.anoLancamento << ',' << livro.numPaginas << ','
                      << livro.quantidade << ',' << livro.quantidadeTotal << '\n';
    }

    arquivoPessoas << "id,nome,telefone,email\n";
    for (NoPessoa* atual = pessoas; atual != nullptr; atual = atual->prox) {
        const Pessoa& pessoa = atual->pessoa;
        arquivoPessoas << pessoa.id << ',' << campoCSV(pessoa.nome) << ','
                       << campoCSV(pessoa.telefone) << ',' << campoCSV(pessoa.email) << '\n';
    }

    arquivoEmprestimos << "id,pessoa,livro,dia,mes,ano,dia_limite,mes_limite,ano_limite\n";
    for (NoEmprestimo* atual = emprestimos; atual != nullptr; atual = atual->prox) {
        const Emprestimo& emprestimo = atual->emprestimo;
        arquivoEmprestimos << emprestimo.id << ',' << emprestimo.pessoa.id << ','
                           << emprestimo.livro.id << ','
                           << emprestimo.dataEmprestimo.dia << ','
                           << emprestimo.dataEmprestimo.mes << ','
                           << emprestimo.dataEmprestimo.ano << ','
                           << emprestimo.dataLimite.dia << ','
                           << emprestimo.dataLimite.mes << ','
                           << emprestimo.dataLimite.ano << '\n';
    }

    arquivoReservas << "livro,pessoa\n";
    for (NoLivro* atual = livros; atual != nullptr; atual = atual->prox) {
        for (NoReserva* reserva = atual->livro.filaInicio;
             reserva != nullptr;
             reserva = reserva->prox) {
            arquivoReservas << atual->livro.id << ',' << reserva->pessoa.id << '\n';
        }
    }
}

// Lê os arquivos CSV e reconstrói as listas de livros, pessoas,
// empr3stimos e reservas. Tamb3m atualiza os proximos IDs disponiveis
void carregarDados(
    NoLivro*& inicioLivros, NoLivro*& fimLivros,
    NoPessoa*& inicioPessoas, NoPessoa*& fimPessoas,
    NoEmprestimo*& inicioEmprestimos, NoEmprestimo*& fimEmprestimos,
    int& proximoIdLivro, int& proximoIdPessoa, int& proximoIdEmprestimo
) {
    string linha;
    ifstream arquivoLivros("livros.csv");
    if (arquivoLivros && getline(arquivoLivros, linha)) {
        while (getline(arquivoLivros, linha)) {
            vector<string> campos = separarCSV(linha);
            if (campos.size() != 8) continue;
            NoLivro* novo = new NoLivro{};
            novo->livro.id = stoi(campos[0]);
            novo->livro.nome = campos[1];
            novo->livro.autor = campos[2];
            novo->livro.categoria = campos[3];
            novo->livro.anoLancamento = stoi(campos[4]);
            novo->livro.numPaginas = stoi(campos[5]);
            novo->livro.quantidade = stoi(campos[6]);
            novo->livro.quantidadeTotal = stoi(campos[7]);
            novo->livro.emprestado = novo->livro.quantidade == 0;
            if (inicioLivros == nullptr) inicioLivros = novo;
            else fimLivros->prox = novo;
            fimLivros = novo;
            if (novo->livro.id >= proximoIdLivro) proximoIdLivro = novo->livro.id + 1;
        }
    }

    ifstream arquivoPessoas("pessoas.csv");
    if (arquivoPessoas && getline(arquivoPessoas, linha)) {
        while (getline(arquivoPessoas, linha)) {
            vector<string> campos = separarCSV(linha);
            if (campos.size() != 4) continue;
            NoPessoa* novo = new NoPessoa{};
            novo->pessoa.id = stoi(campos[0]);
            novo->pessoa.nome = campos[1];
            novo->pessoa.telefone = campos[2];
            novo->pessoa.email = campos[3];
            if (inicioPessoas == nullptr) inicioPessoas = novo;
            else fimPessoas->prox = novo;
            fimPessoas = novo;
            if (novo->pessoa.id >= proximoIdPessoa) proximoIdPessoa = novo->pessoa.id + 1;
        }
    }

    ifstream arquivoEmprestimos("emprestimos.csv");
    if (arquivoEmprestimos && getline(arquivoEmprestimos, linha)) {
        while (getline(arquivoEmprestimos, linha)) {
            vector<string> campos = separarCSV(linha);
            if (campos.size() != 9) continue;
            NoPessoa* pessoa = buscarPessoa(inicioPessoas, stoi(campos[1]));
            NoLivro* livro = buscarLivro(inicioLivros, stoi(campos[2]));
            if (pessoa == nullptr || livro == nullptr) continue;
            NoEmprestimo* novo = new NoEmprestimo{};
            novo->emprestimo.id = stoi(campos[0]);
            novo->emprestimo.pessoa = pessoa->pessoa;
            novo->emprestimo.livro = livro->livro;
            novo->emprestimo.dataEmprestimo = {stoi(campos[3]), stoi(campos[4]), stoi(campos[5])};
            novo->emprestimo.dataLimite = {stoi(campos[6]), stoi(campos[7]), stoi(campos[8])};
            novo->emprestimo.espera = false;
            if (inicioEmprestimos == nullptr) inicioEmprestimos = novo;
            else fimEmprestimos->prox = novo;
            fimEmprestimos = novo;
            if (novo->emprestimo.id >= proximoIdEmprestimo)
                proximoIdEmprestimo = novo->emprestimo.id + 1;
        }
    }

    ifstream arquivoReservas("reservas.csv");
    if (arquivoReservas && getline(arquivoReservas, linha)) {
        while (getline(arquivoReservas, linha)) {
            vector<string> campos = separarCSV(linha);
            if (campos.size() != 2) continue;
            NoLivro* livro = buscarLivro(inicioLivros, stoi(campos[0]));
            NoPessoa* pessoa = buscarPessoa(inicioPessoas, stoi(campos[1]));
            if (livro == nullptr || pessoa == nullptr) continue;
            NoReserva* novo = new NoReserva{pessoa->pessoa, nullptr};
            if (livro->livro.filaInicio == nullptr) livro->livro.filaInicio = novo;
            else livro->livro.filaFim->prox = novo;
            livro->livro.filaFim = novo;
        }
    }
}

// verifica se a pessoa ja esta na fila

bool pessoaNaFila(const Livro& livro, int idPessoa) {
    for (NoReserva* atual = livro.filaInicio;
         atual != nullptr;
         atual = atual->prox) {

        if (atual->pessoa.id == idPessoa)
            return true;
    }

    return false;
}

// adiciona pessoa ao final da fila

void adicionarNaFila(Livro& livro, const Pessoa& pessoa) {
    NoReserva* novo = new NoReserva{pessoa, nullptr};

    if (livro.filaInicio == nullptr)
        livro.filaInicio = novo;
    else
        livro.filaFim->prox = novo;

    livro.filaFim = novo;

    cout << pessoa.nome
         << " foi adicionada ao fim da fila de reserva.\n";
}

// cria emprestimo

void criarEmprestimo(
    NoEmprestimo*& inicio,
    NoEmprestimo*& fim,
    int& autoIncremento,
    const Pessoa& pessoa,
    Livro& livro
) {
    NoEmprestimo* novo = new NoEmprestimo;

    novo->emprestimo.id = autoIncremento++;

    novo->emprestimo.pessoa = pessoa;
    novo->emprestimo.livro = livro;

    novo->prox = nullptr;

    if (inicio == nullptr)
        inicio = novo;
    else
        fim->prox = novo;

    fim = novo;

    --livro.quantidade;

    livro.emprestado = (livro.quantidade == 0);

    cout << "emprestimo registrado.\n";

    novo->emprestimo.apresentar();
}

// apresenta fila de espera

void apresentarFila(const Livro& livro) {
    cout << "fila de espera: ";

    if (livro.filaInicio == nullptr) {
        cout << "vazia\n";
        return;
    }

    for (NoReserva* atual = livro.filaInicio;
         atual != nullptr;
         atual = atual->prox) {

        cout << atual->pessoa.nome
             << " (id " << atual->pessoa.id << ")";

        if (atual->prox != nullptr)
            cout << " -> ";
    }

    cout << "\n";
}

// relatorio de livros

void relatorioLivros(NoLivro* inicio) {
    if (inicio == nullptr)
        cout << "nenhum livro cadastrado.\n";

    for (NoLivro* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        atual->livro.apresentar();
        apresentarFila(atual->livro);
    }
}

// relatorio de pessoas

void relatorioPessoas(NoPessoa* inicio) {
    if (inicio == nullptr)
        cout << "nenhuma pessoa cadastrada.\n";

    for (NoPessoa* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        atual->pessoa.apresentar();
    }
}

// relatorio de emprestimos

void relatorioEmprestimos(
    NoEmprestimo* inicio,
    bool somenteAtrasados
) {
    if (inicio == nullptr) {
        cout << "nao ha emprestimos ativos.\n";
        return;
    }

    data hoje = dataLocal(time(nullptr));

    bool encontrou = false;

    cout << (somenteAtrasados
             ? "\nemprestimos atrasados\n"
             : "\nemprestimos ativos\n");

    for (NoEmprestimo* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        bool atrasado = dataAnterior(
            atual->emprestimo.dataLimite,
            hoje
        );

        if (!somenteAtrasados || atrasado) {
            atual->emprestimo.apresentar();

            cout << "situacao: "
                 << (atrasado
                     ? "atrasado"
                     : "dentro do prazo")
                 << "\n";

            encontrou = true;
        }
    }

    if (!encontrou)
        cout << "nenhum emprestimo atrasado.\n";
}

// relatorio de filas

void relatorioFilas(NoLivro* inicio) {
    bool encontrou = false;

    cout << "\nrelatorio de filas de espera\n";

    for (NoLivro* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {

        if (atual->livro.filaInicio != nullptr) {
            cout << "livro: "
                 << atual->livro.nome
                 << " (id " << atual->livro.id << ")\n";

            apresentarFila(atual->livro);

            encontrou = true;
        }
    }

    if (!encontrou)
        cout << "nao ha reservas pendentes.\n";
}

int calcularMulta(const Emprestimo& emprestimo, const data& hoje) {
    int diasAtraso = diasEntre(emprestimo.dataLimite, hoje);
    return diasAtraso > 0 ? 5 + diasAtraso - 1 : 0;
}

void relatorioMultas(NoEmprestimo* inicio) {
    if (inicio == nullptr) {
        cout << "nao ha emprestimos ativos para verificar multas.\n";
        return;
    }

    data hoje = dataLocal(time(nullptr));
    bool encontrouAtraso = false;
    int totalMultas = 0;

    cout << "\nmultas por atraso\n";
    for (NoEmprestimo* atual = inicio;
         atual != nullptr;
         atual = atual->prox) {
        int diasAtraso = diasEntre(atual->emprestimo.dataLimite, hoje);
        if (diasAtraso > 0) {
            int multa = calcularMulta(atual->emprestimo, hoje);
            cout << "emprestimo " << atual->emprestimo.id
                 << " | pessoa: " << atual->emprestimo.pessoa.nome
                 << " | livro: " << atual->emprestimo.livro.nome
                 << " | dias em atraso: " << diasAtraso
                 << " | multa: R$ " << multa << ",00\n";
            totalMultas += multa;
            encontrouAtraso = true;
        }
    }

    if (encontrouAtraso)
        cout << "total de multas: R$ " << totalMultas << ",00\n";
    else
        cout << "nao ha emprestimos atrasados.\n";
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

    carregarDados(
        iniciol, fiml,
        iniciop, fimp,
        inicioe, fime,
        autoIncrementl, autoIncrementp, autoIncremente
    );
    cout << "dados carregados dos arquivos CSV, quando disponiveis.\n";

    do {
        cout << "\nescolha uma opcao:\n"
             << "1 - relatorios\n"
             << "2 - cadastrar novo livro\n"
             << "3 - cadastrar nova pessoa\n"
             << "4 - registrar retirada ou reserva de livro\n"
             << "5 - registrar devolucao\n"
             << "6 - pesquisar titulos\n"
             << "7 - consultar multas por atraso\n"
             << "0 - sair\n";

        escolha = lerInteiro("opcao: ");
        bool alterouDados = false;

        switch (escolha) {

            // relatorios

            case 1: {
                int opcaoRelatorio;

                cout << "\nrelatorios:\n"
                     << "1 - livros\n"
                     << "2 - pessoas\n"
                     << "3 - emprestimos\n"
                     << "4 - fila de espera\n";

                opcaoRelatorio = lerInteiro("opcao: ");

                switch (opcaoRelatorio) {
                    case 1:
                        relatorioLivros(iniciol);
                        break;

                    case 2:
                        relatorioPessoas(iniciop);
                        break;

                    case 3:
                        relatorioEmprestimos(inicioe, false);
                        break;

                    case 4:
                        relatorioFilas(iniciol);
                        break;

                    default:
                        cout << "opcao de relatorio invalida.\n";
                }

                break;
            }

            // cadastrar livro

            case 2: {
                NoLivro* novo = new NoLivro;

                novo->livro.id = autoIncrementl++;

                novo->livro.filaInicio = nullptr;
                novo->livro.filaFim = nullptr;

                novo->prox = nullptr;

                cout << "insira o nome do livro: ";
                getline(cin, novo->livro.nome);

                cout << "insira o autor: ";
                getline(cin, novo->livro.autor);

                cout << "insira a categoria: ";
                getline(cin, novo->livro.categoria);

                novo->livro.anoLancamento =
                    lerInteiro("insira o ano de lancamento: ");

                novo->livro.numPaginas =
                    lerInteiro("insira o numero de paginas: ");

                do {
                    novo->livro.quantidadeTotal =
                        lerInteiro("quantidade de exemplares: ");

                    if (novo->livro.quantidadeTotal <= 0)
                        cout << "a quantidade deve ser maior que zero.\n";

                } while (novo->livro.quantidadeTotal <= 0);

                novo->livro.quantidade =
                    novo->livro.quantidadeTotal;

                novo->livro.emprestado = false;

                if (iniciol == nullptr)
                    iniciol = novo;
                else
                    fiml->prox = novo;

                fiml = novo;

                cout << "livro cadastrado com sucesso. id: "
                     << novo->livro.id << "\n";
                alterouDados = true;

                break;
            }

			// cadastrar pessoa
            case 3: {
                NoPessoa* novo = new NoPessoa;

                novo->pessoa.id = autoIncrementp++;
                novo->prox = nullptr;

                cout << "insira o nome da pessoa: ";
                getline(cin, novo->pessoa.nome);

                cout << "insira o telefone: ";
                getline(cin, novo->pessoa.telefone);

                cout << "insira o email: ";
                getline(cin, novo->pessoa.email);

                if (iniciop == nullptr)
                    iniciop = novo;
                else
                    fimp->prox = novo;

                fimp = novo;

                cout << "pessoa cadastrada com sucesso. id: "
                     << novo->pessoa.id << "\n";
                alterouDados = true;

                break;
            }
            // emprestimo ou reserva

            case 4: {
                int idPessoa = lerInteiro("insira o id da pessoa: ");

                NoPessoa* pessoa = buscarPessoa(iniciop, idPessoa);

                if (pessoa == nullptr) {
                    cout << "pessoa nao encontrada.\n";
                    break;
                }

                int idLivro = lerInteiro("insira o id do livro: ");

                NoLivro* livro = buscarLivro(iniciol, idLivro);

                if (livro == nullptr) {
                    cout << "livro nao encontrado.\n";
                    break;
                }

                if (buscarEmprestimo(
                        inicioe,
                        idPessoa,
                        idLivro
                    ) != nullptr) {

                    cout << "essa pessoa ja tem um exemplar deste livro emprestado.\n";
                    break;
                }

                if (pessoaNaFila(livro->livro, idPessoa)) {
                    cout << "essa pessoa ja esta na fila de espera deste livro.\n";
                    break;
                }

                if (livro->livro.quantidade == 0 ||
                    livro->livro.filaInicio != nullptr) {

                    adicionarNaFila(
                        livro->livro,
                        pessoa->pessoa
                    );

                } else {
                    criarEmprestimo(
                        inicioe,
                        fime,
                        autoIncremente,
                        pessoa->pessoa,
                        livro->livro
                    );
                }
                alterouDados = true;

                break;
            }

            // devolucao

            case 5: {
                int idLivro = lerInteiro(
                    "insira o id do livro devolvido: "
                );

                NoLivro* livro = buscarLivro(iniciol, idLivro);

                if (livro == nullptr) {
                    cout << "livro nao encontrado.\n";
                    break;
                }

                bool encontrouEmprestimo = false;

                cout << "\nemprestimos ativos deste livro:\n";

                for (NoEmprestimo* atual = inicioe;
                     atual != nullptr;
                     atual = atual->prox) {

                    if (atual->emprestimo.livro.id == idLivro) {
                        atual->emprestimo.apresentar();
                        encontrouEmprestimo = true;
                    }
                }

                if (!encontrouEmprestimo) {
                    cout << "nao ha emprestimo ativo desse livro.\n";
                    break;
                }

                int idEmprestimo = lerInteiro(
                    "digite o id do emprestimo que sera devolvido: "
                );

                NoEmprestimo* anterior = nullptr;
                NoEmprestimo* emprestimo = inicioe;

                while (emprestimo != nullptr &&
                       !(emprestimo->emprestimo.livro.id == idLivro &&
                         emprestimo->emprestimo.id == idEmprestimo)) {

                    anterior = emprestimo;
                    emprestimo = emprestimo->prox;
                }

                if (emprestimo == nullptr) {
                    cout << "id de emprestimo invalido para esse livro.\n";
                    break;
                }

                data hoje = dataLocal(time(nullptr));
                int multa = calcularMulta(emprestimo->emprestimo, hoje);
                if (multa > 0)
                    cout << "multa por atraso cobrada: R$ " << multa << ",00\n";
                else
                    cout << "devolucao sem multa.\n";

                if (anterior == nullptr)
                    inicioe = emprestimo->prox;
                else
                    anterior->prox = emprestimo->prox;

                if (fime == emprestimo)
                    fime = anterior;

                delete emprestimo;

                ++livro->livro.quantidade;

                livro->livro.emprestado =
                    (livro->livro.quantidade == 0);

                cout << "devolucao registrada.\n";

                // chama a proxima pessoa da fila

                if (livro->livro.filaInicio != nullptr) {
                    NoReserva* proximaReserva =
                        livro->livro.filaInicio;

                    livro->livro.filaInicio =
                        proximaReserva->prox;

                    if (livro->livro.filaInicio == nullptr)
                        livro->livro.filaFim = nullptr;

                    Pessoa proximaPessoa =
                        proximaReserva->pessoa;

                    delete proximaReserva;

                    cout << "chamando a proxima pessoa da fila.\n";

                    criarEmprestimo(
                        inicioe,
                        fime,
                        autoIncremente,
                        proximaPessoa,
                        livro->livro
                    );
                }
                alterouDados = true;

                break;
            }

            // pesquisar titulo

            case 6: {
                string termo;

                cout << "digite o titulo ou parte dele: ";
                getline(cin, termo);

                termo = minusculas(termo);

                bool encontrou = false;

                for (NoLivro* atual = iniciol;
                     atual != nullptr;
                     atual = atual->prox) {

                    if (minusculas(atual->livro.nome).find(termo)
                        != string::npos) {

                        atual->livro.apresentar();
                        apresentarFila(atual->livro);

                        encontrou = true;
                    }
                }

                if (!encontrou)
                    cout << "nenhum titulo correspondente encontrado.\n";

                break;
            }

            case 7:
                relatorioMultas(inicioe);
                break;

            // sair

            case 0:
                cout << "encerrando o programa.\n";
                break;

            default:
                cout << "opcao invalida.\n";
        }

        if (alterouDados || escolha == 0)
            salvarDados(iniciol, iniciop, inicioe);

    } while (escolha != 0);

    // libera livros e filas

    while (iniciol != nullptr) {
        NoLivro* proximoLivro = iniciol->prox;

        NoReserva* reserva =
            iniciol->livro.filaInicio;

        while (reserva != nullptr) {
            NoReserva* proximaReserva = reserva->prox;

            delete reserva;

            reserva = proximaReserva;
        }

        delete iniciol;

        iniciol = proximoLivro;
    }

    // libera pessoas

    while (iniciop != nullptr) {
        NoPessoa* proximaPessoa = iniciop->prox;

        delete iniciop;

        iniciop = proximaPessoa;
    }

    // libera emprestimos

    while (inicioe != nullptr) {
        NoEmprestimo* proximoEmprestimo = inicioe->prox;

        delete inicioe;

        inicioe = proximoEmprestimo;
    }

    return 0;
}
