#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

using namespace std;

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
        cerr << "Error: Unable to write to " << filename << "\n";
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
        cerr << "Error: Unable to write to " << filename << "\n";
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
        cerr << "Invalid match.\n";
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
    cout << "Match added successfully.\n";
}

void match_history() {
    vector<Match> matches = load_matches("matches.txt");
    if (matches.empty()) {
        cout << "No match history available.\n";
        return;
    }

    for (const auto& m : matches) {
        cout << m.player1 << " (" << m.score1 << ") vs "
             << m.player2 << " (" << m.score2 << ")\n";
    }
}

void list_players() {
    vector<Player> players = load_players("players.txt");
    if (players.empty()) {
        cout << "No players available.\n";
        return;
    }

    sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
        return a.wins > b.wins;
    });

    for (const auto& p : players) {
        cout << p.name << ": " << p.wins << " wins, " << p.losses << " losses\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./app_1.exe <command> [args]\n";
        return 1;
    }

    string cmd = argv[1];
    if (cmd == "add_match") {
        if (argc != 6) {
            cerr << "Usage: ./app_1.exe add_match <player1> <score1> <player2> <score2>\n";
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
        cerr << "Unknown command.\n";
        return 1;
    }

    return 0;
}