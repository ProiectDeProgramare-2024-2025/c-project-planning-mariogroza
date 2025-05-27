#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

using namespace std;

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"
#define BRIGHT_RED "\033[91m"
#define BRIGHT_GREEN "\033[92m"
#define BRIGHT_YELLOW "\033[93m"
#define BRIGHT_BLUE "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN "\033[96m"

struct Player {
    string name;
    int wins = 0;
    int losses = 0;
};

struct Match {
    string player1, player2;
    int score1, score2;
};

vector<Player> load_players(const string& filename) {
    vector<Player> players;
    ifstream fin(filename);
    if (!fin || fin.peek() == ifstream::traits_type::eof()) return players;

    int count;
    fin >> count;

    if (!fin || count < 0) return players;

    for (int i = 0; i < count; ++i) {
        Player p;
        if (!(fin >> p.name >> p.wins >> p.losses)) break;
        players.push_back(p);
    }

    return players;
}

void save_players(const string& filename, const vector<Player>& players) {
    ofstream fout(filename, ios::trunc);
    if (!fout) {
        cerr << BRIGHT_RED << "Error: Unable to write to " << filename << RESET << "\n";
        return;
    }

    fout << players.size() << '\n';
    for (const auto& p : players) {
        fout << p.name << ' ' << p.wins << ' ' << p.losses << '\n';
    }
}

vector<Match> load_matches(const string& filename) {
    vector<Match> matches;
    ifstream fin(filename);
    if (!fin || fin.peek() == ifstream::traits_type::eof()) return matches;

    int count;
    fin >> count;

    if (!fin || count < 0) return matches;

    for (int i = 0; i < count; ++i) {
        Match m;
        if (!(fin >> m.player1 >> m.score1 >> m.player2 >> m.score2)) break;
        matches.push_back(m);
    }

    return matches;
}

void save_matches(const string& filename, const vector<Match>& matches) {
    ofstream fout(filename, ios::trunc);
    if (!fout) {
        cerr << BRIGHT_RED << "Error: Unable to write to " << filename << RESET << "\n";
        return;
    }

    fout << matches.size() << '\n';
    for (const auto& m : matches) {
        fout << m.player1 << ' ' << m.score1 << ' '
             << m.player2 << ' ' << m.score2 << '\n';
    }
}

void add_match(const string& p1, int s1, const string& p2, int s2) {
    if (p1 == p2 || s1 == s2) {
        cerr << BRIGHT_RED << "Invalid match." << RESET << "\n";
        return;
    }

    vector<Player> players = load_players("players.txt");
    vector<Match> matches = load_matches("matches.txt");

    int player1_idx = -1, player2_idx = -1;

    for (int i = 0; i < players.size(); ++i) {
        if (players[i].name == p1) player1_idx = i;
        if (players[i].name == p2) player2_idx = i;
    }

    if (player1_idx == -1) {
        Player newPlayer1;
        newPlayer1.name = p1;
        newPlayer1.wins = 0;
        newPlayer1.losses = 0;
        players.push_back(newPlayer1);
        player1_idx = players.size() - 1;
    }

    if (player2_idx == -1) {
        Player newPlayer2;
        newPlayer2.name = p2;
        newPlayer2.wins = 0;
        newPlayer2.losses = 0;
        players.push_back(newPlayer2);
        player2_idx = players.size() - 1;
    }

    if (s1 > s2) {
        players[player1_idx].wins++;
        players[player2_idx].losses++;
    } else {
        players[player2_idx].wins++;
        players[player1_idx].losses++;
    }

    matches.push_back({p1, p2, s1, s2});

    save_players("players.txt", players);
    save_matches("matches.txt", matches);
    cout << BRIGHT_GREEN << "Match added successfully." << RESET << "\n";
}

void match_history() {
    vector<Match> matches = load_matches("matches.txt");
    if (matches.empty()) {
        cout << YELLOW << "No match history available." << RESET << "\n";
        return;
    }

    cout << BOLD << BRIGHT_CYAN << "=== MATCH HISTORY ===" << RESET << "\n";
    for (const auto& m : matches) {
        string winner_color = (m.score1 > m.score2) ? BRIGHT_GREEN : BRIGHT_RED;
        string loser_color = (m.score1 > m.score2) ? BRIGHT_RED : BRIGHT_GREEN;

        if (m.score1 > m.score2) {
            cout << winner_color << m.player1 << RESET << " ("
                 << BOLD << BRIGHT_YELLOW << m.score1 << RESET << ") vs "
                 << loser_color << m.player2 << RESET << " ("
                 << BRIGHT_YELLOW << m.score2 << RESET << ")\n";
        } else {
            cout << loser_color << m.player1 << RESET << " ("
                 << BRIGHT_YELLOW << m.score1 << RESET << ") vs "
                 << winner_color << m.player2 << RESET << " ("
                 << BOLD << BRIGHT_YELLOW << m.score2 << RESET << ")\n";
        }
    }
}

void list_players() {
    vector<Player> players = load_players("players.txt");
    if (players.empty()) {
        cout << YELLOW << "No players available." << RESET << "\n";
        return;
    }

    sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
        return a.wins > b.wins;
    });

    cout << BOLD << BRIGHT_MAGENTA << "=== PLAYER RANKINGS ===" << RESET << "\n";
    for (int i = 0; i < players.size(); ++i) {
        string rank_color;
        if (i == 0) rank_color = BRIGHT_YELLOW;
        else if (i == 1) rank_color = WHITE;
        else if (i == 2) rank_color = YELLOW;
        else rank_color = CYAN;

        cout << rank_color << "#" << (i + 1) << " " << BOLD << players[i].name << RESET
             << ": " << BRIGHT_GREEN << players[i].wins << " wins" << RESET
             << ", " << BRIGHT_RED << players[i].losses << " losses" << RESET << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << BRIGHT_RED << "Usage: ./app_1.exe <command> [args]" << RESET << "\n";
        return 1;
    }

    string cmd = argv[1];
    if (cmd == "add_match") {
        if (argc != 6) {
            cerr << BRIGHT_RED << "Usage: ./app_1.exe add_match <player1> <score1> <player2> <score2>" << RESET << "\n";
            return 1;
        }
        string p1 = argv[2];
        int s1 = atoi(argv[3]);
        string p2 = argv[4];
        int s2 = atoi(argv[5]);
        add_match(p1, s1, p2, s2);
    } else if (cmd == "match_history") {
        match_history();
    } else if (cmd == "list_players") {
        list_players();
    } else {
        cerr << BRIGHT_RED << "Unknown command." << RESET << "\n";
        return 1;
    }

    return 0;
}
