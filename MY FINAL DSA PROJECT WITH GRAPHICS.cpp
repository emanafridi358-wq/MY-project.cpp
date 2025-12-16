#include <bits/stdc++.h>
#include <windows.h>
using namespace std;

/* ============ COLOR FUNCTIONS ============ */

void setColor(int c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void title(string t) {
    setColor(11);
    cout << "\n============================================\n";
    cout << "   " << t << "\n";
    cout << "============================================\n";
    setColor(7);
}

void loading() {
    setColor(14);
    cout << "\nLoading ";
    for (int i = 0; i < 20; i++) {
        cout << char(219);
        Sleep(50);
    }
    setColor(10);
    cout << " Done!\n";
    setColor(7);
}

/* ============ USER ============ */

struct User {
    string username;
    string password;
    set<string> skills;
};

/* ============ JOB ============ */

struct Job {
    string title;
    map<string,double> required;
};

/* ============ SKILL GRAPH ============ */

class SkillGraph {
public:
    map<string, vector<string> > adj;

    void addEdge(string a, string b) {
        adj[a].push_back(b);
        if (adj.find(b) == adj.end())
            adj[b];
    }

    vector<string> getPath(set<string> &userSkills, string target) {
        map<string, vector<string> > rev;

        // Build reverse graph (C++98)
        for (map<string, vector<string> >::iterator it = adj.begin();
             it != adj.end(); ++it) {

            for (int i = 0; i < it->second.size(); i++) {
                string x = it->second[i];
                rev[x].push_back(it->first);
            }
            if (rev.find(it->first) == rev.end())
                rev[it->first];
        }

        queue<string> q;
        map<string,string> parent;
        set<string> visited;

        q.push(target);
        visited.insert(target);

        string meet = "";

        while (!q.empty()) {
            string cur = q.front();
            q.pop();

            if (userSkills.count(cur)) {
                meet = cur;
                break;
            }

            for (int i = 0; i < rev[cur].size(); i++) {
                string p = rev[cur][i];
                if (!visited.count(p)) {
                    visited.insert(p);
                    parent[p] = cur;
                    q.push(p);
                }
            }
        }

        if (meet == "") return vector<string>();

        vector<string> path;
        path.push_back(meet);

        while (meet != target) {
            meet = parent[meet];
            path.push_back(meet);
        }
        return path;
    }
};

/* ============ MATCHER ============ */

class Matcher {
public:
    static double score(User &u, Job &j) {
        double total = 0, have = 0;

        for (map<string,double>::iterator it = j.required.begin();
             it != j.required.end(); ++it) {
            total += it->second;
            if (u.skills.count(it->first))
                have += it->second;
        }
        if (total == 0) return 0;
        return (have / total) * 100;
    }
};

/* ============ CAREER PORTAL ============ */

class CareerPortal {
public:
    vector<User> users;
    vector<Job> jobs;
    SkillGraph graph;

    void saveUsers() {
        ofstream fout("users.txt");
        for (int i = 0; i < users.size(); i++) {
            fout << users[i].username << " " << users[i].password << "\n";
            fout << users[i].skills.size() << "\n";
            for (set<string>::iterator it = users[i].skills.begin();
                 it != users[i].skills.end(); ++it)
                fout << *it << "\n";
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

    void signup() {
        system("cls");
        title("NEW USER REGISTRATION");
        string u,p;
        cout << "Username: ";
        cin >> u;

        for (int i = 0; i < users.size(); i++)
            if (users[i].username == u) {
                setColor(12);
                cout << "Username already exists!\n";
                setColor(7);
                system("pause");
                return;
            }

        cout << "Password: ";
        cin >> p;

        User user;
        user.username = u;
        user.password = p;
        users.push_back(user);
        saveUsers();

        loading();
        setColor(10);
        cout << "Signup Successful!\n";
        setColor(7);
        system("pause");
    }

    int login() {
        system("cls");
        title("USER LOGIN");
        string u,p;
        cout << "Username: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;

        for (int i = 0; i < users.size(); i++) {
            if (users[i].username == u &&
                users[i].password == p) {
                loading();
                setColor(10);
                cout << "Login Successful!\n";
                setColor(7);
                system("pause");
                return i;
            }
        }
        setColor(12);
        cout << "Invalid Login!\n";
        setColor(7);
        system("pause");
        return -1;
    }

    void userMenu(int id) {
        while (true) {
            system("cls");
            title("USER DASHBOARD");
            setColor(14);
            cout << "1) View Skills\n";
            cout << "2) Add Skill\n";
            cout << "3) Learning Path\n";
            cout << "4) Job Recommendation\n";
            cout << "5) Logout\n";
            setColor(7);
            cout << "Choose: ";

            int ch;
            cin >> ch;
            system("cls");

            if (ch == 1) {
                title("YOUR SKILLS");
                for (set<string>::iterator it = users[id].skills.begin();
                     it != users[id].skills.end(); ++it)
                    cout << " - " << *it << "\n";
            }
            else if (ch == 2) {
                title("ADD SKILL");
                string s;
                cout << "Enter skill: ";
                cin >> s;
                users[id].skills.insert(s);
                saveUsers();
                cout << "Skill Added!\n";
            }
            else if (ch == 3) {
                title("LEARNING PATH");
                string target;
                cout << "Target Skill: ";
                cin >> target;

                vector<string> path =
                    graph.getPath(users[id].skills, target);

                if (path.empty())
                    cout << "No Learning Path Found\n";
                else {
                    for (int i = 0; i < path.size(); i++) {
                        cout << "[" << path[i] << "]";
                        if (i != path.size()-1) cout << " ==> ";
                    }
                    cout << "\n";
                }
            }
            else if (ch == 4) {
                title("JOB MATCH RESULTS");
                for (int i = 0; i < jobs.size(); i++)
                    cout << jobs[i].title << " : "
                         << Matcher::score(users[id], jobs[i]) << "%\n";
            }
            else if (ch == 5) break;

            system("pause");
        }
    }
};

/* ============ MAIN ============ */

int main() {
    CareerPortal sys;
    sys.loadUsers();

    // Skill Graph
    sys.graph.addEdge("Arrays","DSA");
    sys.graph.addEdge("DSA","Algorithms");
    sys.graph.addEdge("Stats","ML");
    sys.graph.addEdge("LinearAlgebra","ML");

    // Jobs
    Job j1; j1.title="Data Analyst";
    j1.required["Stats"]=40; j1.required["SQL"]=30; j1.required["Python"]=30;

    Job j2; j2.title="ML Engineer";
    j2.required["LinearAlgebra"]=30; j2.required["ML"]=40; j2.required["Python"]=30;

    Job j3; j3.title="Software Engineer";
    j3.required["DSA"]=40; j3.required["Algorithms"]=30; j3.required["C++"]=30;

    Job j4; j4.title="Web Developer";
    j4.required["HTML"]=25; j4.required["CSS"]=25;
    j4.required["JavaScript"]=30; j4.required["SQL"]=20;

    Job j5; j5.title="Cyber Security Analyst";
    j5.required["Networks"]=40; j5.required["OS"]=30; j5.required["Security"]=30;

    sys.jobs.push_back(j1);
    sys.jobs.push_back(j2);
    sys.jobs.push_back(j3);
    sys.jobs.push_back(j4);
    sys.jobs.push_back(j5);

    while (true) {
        system("cls");
        title("CAREER GUIDANCE SYSTEM");
        cout << "1) Login\n2) Signup\n3) Exit\nChoose: ";
        int op; cin >> op;

        if (op == 1) {
            int id = sys.login();
            if (id != -1) sys.userMenu(id);
        }
        else if (op == 2) sys.signup();
        else break;
    }
    return 0;
}

