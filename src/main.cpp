#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <string>
#include <vector>
#include <random>

// Obter um número aleatório
int numAletatorio(int max) {
    std::random_device rd; // Seed
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<> distrib(0, max - 1); // Intervalor [0, max)

    return distrib(gen);
}

// Classe TicTacToe
class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo - matriz 3x3 de caracteres
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo - caractere indicando o vencedor ('X', 'O', ou 'D' para empate)

public:
    TicTacToe() {
        // Inicializar o tabuleiro e as variáveis do jogo
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                board[i][j] = ' ';
            }
        }
        game_over = false;
        winner = ' ';
    }

    void display_board() {
        // Exibir o tabuleiro no console
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                std::cout << board[i][j] << " | ";
            }
            std::cout << std::endl;
            std::cout << "===========";
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Jogador Atual: " << getJogadorAtual() << std:: endl;
    }

    bool movimentoOK(int row, int col){
        if(row >= 0 && row < 3 && col >= 0 && col < 3){
            if(board[row][col] == ' '){
                return true;
            }
            else{
                //std::cout << "Movimento inválido" << std::endl;
                return false;
            }
        }
        else{
            return false;
        }
    }

    void setJogadorAtual(char jogador){
        current_player = jogador;
        return;
    }

    char getJogadorAtual(){
        return current_player;
    }

    bool make_move(char player, int row, int col) {
        // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        // Utilizar variável de condição para alternância de turnos

        setJogadorAtual(player);

        if(movimentoOK(row, col)){
            board[row][col] = player;
            return true;
        }
        else{
            return false;
        }
    }

    bool check_win() {
        // Verificar se o jogador atual venceu o jogo
        for(int i = 0; i < 3; i++){
            // Verificar as linhas e colunas
            if((board[i][0]==current_player && board[i][1]==current_player && board[i][2]==current_player) ||
               (board[0][i]==current_player && board[1][i]==current_player && board[2][i]==current_player)){
                game_over = true;
                return true;
            }
            // Verificar as diagonais
            else if((board[0][0]==current_player && board[1][1]==current_player && board[2][2]==current_player) || 
                    (board[0][2]==current_player && board[1][1]==current_player && board[2][0]==current_player)){
                game_over = true;
                return true;
            }
            else{
                return false;
            }
        }
        return false;
    }

    bool check_draw() {
        // Verificar se houve um empate
        if(!game_over){
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 3; j++){
                    if(board[i][j]==' '){
                        return false;
                    }
                }
            }
        }
        game_over = true;
        return true;
    }

    bool is_game_over() {
        return game_over;
    }

    char get_winner() {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        if(check_win()){
            return current_player;
        }
        else if(check_draw()){
            return 'D';
        }
        else{
            return 'E'; // erro
        }
    }
};

// Classe Player
class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador ('sequencial' ou 'aleatório')

public:
    Player(TicTacToe& g, char s, std::string strat) 
        : game(g), symbol(s), strategy(strat) {}

    void play() {
        // Executar jogadas de acordo com a estratégia escolhida
        if(strategy == "sequencial"){
            play_sequential();
            //std::cout << "SEQ" << std::endl;
        }
        else if(strategy == "aleatorio"){
            play_random();
            //std::cout << "RAMD" << std::endl;
        }
        else{
            return;
        }
    }
    char getId(){
        return symbol;
    }

private:
    int proximo_movimento = 0;
    void play_sequential() {
        // Implementar a estratégia sequencial de jogadas
        int linha = proximo_movimento / 3;
        int coluna = proximo_movimento % 3;
        if(game.movimentoOK(linha, coluna)){
            game.make_move(symbol,linha, coluna);
            std::cout << std::endl;
            std::cout << "Linha " << linha << "| Coluna " << coluna << std::endl;
            proximo_movimento++;
            return;
        }
        return;
    }

    void play_random() {
        // Implementar a estratégia aleatória de jogadas
        int linha, coluna;
        do{
            linha = numAletatorio(3);
            coluna = numAletatorio(3);
        } while(!game.movimentoOK(linha, coluna));
        
        game.make_move(symbol,linha, coluna);
        
        return;
    }
};


std::mutex mutex;
std::condition_variable cv; 
char jogadorAtual = 'X'; // primeiro a jogar
bool fim_jogo = false;

void jogarJogoVelha(TicTacToe& jogo, Player& jogador){

    while(!jogo.is_game_over()){
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&]() { return jogadorAtual == jogador.getId() || jogo.is_game_over() || fim_jogo; });

        if(fim_jogo){
            return;
        }

        jogador.play();
        jogo.display_board();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system("clear");

        if(jogo.check_win()){
            std::cout << "O vencedor é: " << jogo.get_winner() << "!"  << std::endl;
            fim_jogo = true;
            return;
        }

        if(jogo.check_draw()){
            std::cout << "Resultado: " << jogo.get_winner() << "!"  << std::endl;
            fim_jogo = true;
            return;
        }

        // Trocar de Jogador
        jogadorAtual = (jogadorAtual == 'X') ? 'O' : 'X';
        //system("clear");
        cv.notify_all();
    }
    return;
}

// Função principal
int main() {
    // Inicializar o jogo e os jogadores
    std::cout << "Início JOGO DA VELHA" << std::endl;

    TicTacToe novoJogo = TicTacToe();
    
    Player jogador1(novoJogo, 'X', "aleatorio"); 
    Player jogador2(novoJogo, 'O', "sequencial");

    // Criar as threads para os jogadores
    std::thread J1(jogarJogoVelha, std::ref(novoJogo), std::ref(jogador1));
    std::thread J2(jogarJogoVelha, std::ref(novoJogo), std::ref(jogador2));

    // Aguardar o término das threads
    J1.join();
    J2.join();

    return 0;
}
