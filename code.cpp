#include <bits/stdc++.h>
using namespace std;

// ---------------- USER ----------------
struct User {
    string username;
    string password;
    unordered_set<string> skills;
};

// ---------------- JOB ----------------
struct Job {
    string title;
    unordered_map<string,double> required;
};

// ---------------- SKILL GRAPH ----------------
class SkillGraph {
public:
    unordered_map<string, vector<string>> adj;

    void addEdge(string a, string b) {
        adj[a].push_back(b);
        if (!adj.count(b)) adj[b];
    }

    vector<string> getPath(unordered_set<string> &userSkills, string target) {
        unordered_map<string, vector<string>> rev;

        for (auto &p : adj) {
            for (auto &s : p.second)
                rev[s].push_back(p.first);
            if (!rev.count(p.first)) rev[p.first];
        }

        queue<string> q;
        unordered_map<string,string> parent;
        unordered_set<string> visited;

        q.push(target);
        visited.insert(target);

        string meet = "";
        while (!q.empty()) {
            string cur = q.front(); q.pop();

            if (userSkills.count(cur)) {
                meet = cur;
                break;
            }

            for (auto &p : rev[cur]) {
                if (!visited.count(p)) {
                    visited.insert(p);
                    parent[p] = cur;
                    q.push(p);
                }
            }
        }

        if (meet == "") return {};

        vector<string> path;
        path.push_back(meet);
        while (meet != target) {
            meet = parent[meet];
            path.push_back(meet);
        }
        return path;
    }
};

// ---------------- MATCHER ----------------
class Matcher {
public:
    static double score(User &u, Job &j) {
        double total = 0, have = 0;
        for (auto &p : j.required) {
            total += p.second;
            if (u.skills.count(p.first)) have += p.second;
        }
        return (total == 0) ? 0 : (have / total) * 100;
    }
};

// ---------------- CAREER PORTAL ----------------
class CareerPortal {
public:
    vector<User> users;
    vector<Job> jobs;
    SkillGraph graph;

    // ---------- FILE HANDLING ----------
    void saveUsers() {
        ofstream fout("users.txt");
        for (auto &u : users) {
            fout << u.username << " " << u.password << "\n";
            fout << u.skills.size() << "\n";
            for (auto &s : u.skills)
                fout << s << "\n";
        }
        fout.close();
    }

    void loadUsers() {
        ifstream fin("users.txt");
        if (!fin) return;

        while (true) {
            User u;
            int n;
            fin >> u.username >> u.password;
            if (fin.fail()) break;
            fin >> n;
            for (int i = 0; i < n; i++) {
                string s;
                fin >> s;
                u.skills.insert(s);
            }
            users.push_back(u);
        }
        fin.close();
    }

    // ---------- AUTH ----------
    void signup() {
        string u,p;
        cout << "Username: ";
        cin >> u;

        for (auto &x : users)
            if (x.username == u) {
                cout << "Username already exists!\n";
                return;
            }

        cout << "Password: ";
        cin >> p;

        User user;
        user.username = u;
        user.password = p;
        users.push_back(user);

        saveUsers();
        cout << "Signup successful!\n";
    }

    int login() {
        string u,p;
        cout << "Username: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;

        for (int i = 0; i < users.size(); i++) {
            if (users[i].username == u && users[i].password == p)
                return i;
        }
        cout << "Invalid login!\n";
        return -1;
    }

    // ---------- USER MENU ----------
    void userMenu(int id) {
        while (true) {
            cout << "\n1) View Skills\n2) Add Skill\n3) Learning Path\n4) Logout\nChoose: ";
            int ch;
            cin >> ch;

            if (ch == 1) {
                for (auto &s : users[id].skills) cout << s << " ";
                cout << "\n";
            }
            else if (ch == 2) {
                string s;
                cout << "Enter skill: ";
                cin >> s;
                users[id].skills.insert(s);
                saveUsers();
            }
            else if (ch == 3) {
                string target;
                cout << "Target skill: ";
                cin >> target;
                auto path = graph.getPath(users[id].skills, target);
                if (path.empty()) cout << "No path found\n";
                else {
                    for (int i = 0; i < path.size(); i++)
                        cout << i+1 << ") " << path[i] << "\n";
                }
            }
            else if (ch == 4) break;
        }
    }
};

// ---------------- MAIN ----------------
int main() {
    CareerPortal sys;
    sys.loadUsers();

    sys.graph.addEdge("Arrays","DSA");
    sys.graph.addEdge("DSA","Algorithms");
    sys.graph.addEdge("Stats","ML");
    sys.graph.addEdge("LinearAlgebra","ML");

    while (true) {
        cout << "\n1) Login\n2) Signup\n3) Exit\nChoose: ";
        int op;
        cin >> op;

        if (op == 1) {
            int id = sys.login();
            if (id != -1) sys.userMenu(id);
        }
        else if (op == 2) sys.signup();
        else break;
    }
    return 0;
}
