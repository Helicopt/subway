#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#define maxn 1000
#define inf 1000000000
#define fst first
#define scn second

using namespace std;
typedef pair<int, int> PA;
typedef pair<int*, int> PAP;
typedef struct edge
{
	int a, nxt;
}E;
typedef struct node
{
	int g, de;
	char s[64];
	vector<int> v,con;
}ND;

const string subway_file("beijing-subway.txt");

class subway_machine {

private:
	int n, m, tot;//n is the line number, m is the station number, tot is the edge number
	char path[256];
	char lines[128][64];
	vector<int> line_g[128];
	map<string, int> Sta;
	map<PA, PAP> paths;
	ND stas[maxn];
	E e[maxn*maxn >> 1];
	int et[maxn],tg[maxn],maxtime,lim=4000000;
	int ansq[maxn], ans,anst[maxn],an;

	void addE(int x, int y) {
		++tot;
		e[tot].a = y;
		e[tot].nxt = stas[x].g;
		++stas[x].de;
		stas[x].g = tot;
		stas[y].con.push_back(x);
	}

	int getLID(const char *s) {
		for (int i = 1; i <= n; ++i)
			if (!strcmp(lines[i], s)) return i;
		return 0;
	}

	int add_station(const char *s, int lid) {
		string ss(s);
		int k;
		if (Sta.find(ss) != Sta.end()) {
			k = Sta[ss];
			if (lid>0) stas[k].v.push_back(lid);
		}
		else {
			++m;
			strcpy_s(stas[m].s, s);
			stas[m].v.clear();
			stas[m].con.clear();
			stas[m].v.push_back(lid);
			stas[m].g = 0;
			stas[m].de = 0;
			Sta[ss] = m;
			k = m;
		}
		return k;
	}

	int init() {
		m = 0, tot = 0;
		char s[64];
		FILE* f;
		fopen_s(&f, path, "r");
		if (f == NULL) {
			fprintf_s(stderr, "Error: file '%s' does not exists or access permission denied.\n", subway_file.data());
			return 1;
		}
		else {
			n = 0;
			while (fscanf_s(f, "%s", s, 64) != EOF) {
				//fprintf_s(stderr, "dealing with %s\n", s);
				++n;
				s[63] = '\0';
				strcpy_s(lines[n], s);
				int k;
				fscanf_s(f, "%d", &k);
				//fprintf_s(stderr, "loading %d stations\n", k);
				int tp = k < 0;
				k = abs(k);
				for (int i = 1; i <= k; ++i) {
					fscanf_s(f, "%s", s, 64);
					//fprintf_s(stderr, "dealing with %s\n", s);
					bool ow = false;
					if (s[strlen(s) - 1] == '*') {
						s[strlen(s) - 1] = '\0';
						ow = true;
					}
					int id = add_station(s, n);
					line_g[n].push_back(id);
					if (i > 1) {
						addE(line_g[n][i - 2], line_g[n][i-1]);
						if (ow == false)
							addE(line_g[n][i-1], line_g[n][i - 2]);
					}
				}
				if (tp) {
					fscanf_s(f, "%s", s, 64);
					bool ow = false;
					if (s[strlen(s) - 1] == '*') {
						s[strlen(s) - 1] = '\0';
						ow = true;
					}
					int cyc = add_station(s,-1);
					addE(line_g[n][k - 1], cyc);
					if (ow == false)
						addE(cyc, line_g[n][k - 1]);
				}
			}
		}
		fclose(f);
		return 0;
	}

	vector<int> inters(vector<int> x, vector<int> y) {
		vector<int>::iterator it1, it2;
		it1 = x.begin();
		it2 = y.begin();
		vector<int> res;
		res.clear();
		while (it1 != x.end() && it2 != y.end()) {
			if (*it1 == *it2) {
				res.push_back(*it1);
				++it1;
				++it2;
			}
			else if (*it1 < *it2) ++it1;
			else ++it2;
		}
		return res;
	}

	void bfs2(int s, int t) {
		if (s == t)
		{
			printf_s("已到达\n");
			return;
		}
		queue<PA> q;
		vector<string> ans;
		ans.clear();
		while (!q.empty()) q.pop();
		map<PA, int> u, v;
		map<PA, PA> f;
		u.clear();
		v.clear();
		f.clear();
		for (int p = stas[s].g; p; p = e[p].nxt) {
			int y = e[p].a;
			PA k(s, y);
			q.push(k);
			f[k] = PA(0, s);
			u[k] = 0;
			v[k] = 1;
		}
		while (!q.empty()) {
			PA x = q.front();
			int step = u[x];
			int d = v[x];
			q.pop();
			for (int p = stas[x.scn].g; p; p = e[p].nxt) {
				PA y(x.scn, e[p].a);
				int stp = (inters(inters(stas[x.scn].v, stas[x.fst].v), inters(stas[y.scn].v, stas[y.fst].v)).size() == 0) + step;
				bool flag = u.find(y) == u.end() || stp < u[y] || stp == u[y] && (v.find(y) == v.end() || d + 1 < v[y]);
				if (flag) {
					q.push(y);
					v[y] = v[x] + 1;
					u[y] = stp;
					f[y] = x;
				}
			}
		}
		int mx = inf, md = inf, mi = 0;
		for (int i = 0; i<stas[t].con.size(); ++i) {
			int y = stas[t].con[i];
			PA k(y, t);
			if (u[k] < mx || u[k] == mx&&v[k] < md) mx = u[k], md = v[k], mi = y;
		}
		PA y(mi, t), x(t, t);
		while (y != PA(0, s)) {
			string tmp(stas[y.scn].s);
			vector<int> tk= inters(stas[x.scn].v, stas[x.fst].v);
			if (inters(inters(stas[y.scn].v, stas[y.fst].v), tk).size() == 0) {
				tmp += string("换乘")+string(lines[tk[0]]);
			}
			ans.push_back(tmp);
			x = y;
			y = f[y];
		}
		ans.push_back(stas[y.scn].s);
		printf_s("%llu\n", ans.size());
		for (unsigned int i = 1; i <= ans.size(); ++i) printf("%s\n", ans[ans.size() - i].c_str());
	}

	int bfs(int s, int t, bool pt = true, int *ret=NULL) {
		if (s == t)
		{
			printf_s("已到达\n");
			return 0;
		}
		queue<int> q;
		vector<string> ans;
		ans.clear();
		while (!q.empty()) q.pop();
		int v[maxn], u[maxn], f[maxn], tr[maxn];
		memset(v, 0x3e, sizeof(v));
		memset(u, 0x3e, sizeof(u));
		q.push(s);
		v[s] = 0;
		u[s] = 0;
		f[s] = 0;
		while (!q.empty()) {
			int x = q.front();
			int step = u[x];
			int d = v[x];
			q.pop();
			for (int p = stas[x].g; p; p = e[p].nxt) {
				int y = e[p].a;
				vector<int> tmp = inters(stas[x].v, stas[y].v);
				int stp = (inters(inters(stas[x].v, stas[abs(f[x])].v), tmp).size() == 0) + step;
				if (f[x] == 0) stp = 0;
				bool flag = v[x] + 1 < v[y];
				if (flag) {
					q.push(y);
					v[y] = v[x] + 1;
					u[y] = stp;
					f[y] = x;
					if (stp > step) f[y] = -f[y], tr[y]=tmp[0];
					if (y == t) {
						int nflag = 0;
						while (y) {
							string tmp(stas[y].s);
							if (nflag) tmp += string("换乘")+string(lines[nflag]);
							if (f[y] < 0) nflag = tr[y]; else nflag = 0;
							ans.push_back(tmp);
							if (!pt) *(ret++) = y;
							y = abs(f[y]);
						}
						if (pt) {
							printf_s("%llu\n", ans.size());
							for (unsigned int i = 1; i <= ans.size(); ++i) printf("%s\n", ans[ans.size() - i].c_str());
						}
						return (int)ans.size();
					}
				}
			}
		}
		return -1;
	}

	void printInfo(int * q, int cnt) {
		cout << cnt << endl;
		for (int i = 0; i < cnt; ++i) printf_s("%s\n",stas[q[i]].s);
	}

	void dfs(int x, int l, int s) {
		int flag = 0;
		if (l == m&&x == s) {
			if (an < ans) {
				ans = an;
				memcpy(ansq, anst, an*sizeof(int));
				//printInfo(ansq, ans);
				//exit(0);
			}
			return;
		}
		++maxtime;
		if (maxtime > lim) return;
		if (an + m - l >= ans || maxtime>lim) return;
		for (int p = stas[x].g; p; p = e[p].nxt) {
			int y = e[p].a;
			if (et[y] == 0 && (y != s || l == m - 1) && tg[y]) {
				flag = -y;
				anst[an++] = y;
				++et[y];
				dfs(y, l + 1, s);
				--et[y];
				--an;
			}
		}
		if (flag==0)
		for (int p = stas[x].g; p; p = e[p].nxt) {
			int y = e[p].a;
			if (et[y] == 0 && (y != s || l == m - 1)) {
				flag = y;
				anst[an++] = y;
				++et[y];
				dfs(y, l + 1, s);
				--et[y];
				--an;
			}
		}
		if (flag == 0) {
			int mx[2] = { 100100000,100100000 }, mi[2] = { -1,-1 };
			for (int y = 1; y <= m; ++y)
				if (et[y] == 0 && (y != s || l == m - 1)) {
					int *tmp;
					int cnt = 0;
					if (paths.find(PA(x, y)) == paths.end()) {
						tmp = (int*)malloc(m * sizeof(int));
						cnt = bfs(x, y, false, tmp);
						paths[PA(x, y)] = PAP(tmp, cnt);
					}
					if (cnt - 1 < mx[0]) mx[1] = mx[0], mi[1] = mi[0], mx[0] = cnt - 1, mi[0] = y;
					else if (cnt - 1 < mx[1]) mx[1] = cnt - 1, mi[1] = y;
				}
			int ant = an;
			for (int o = 0; o < 2; ++o) {
				if (o > 0 && mi[o] == -1) continue;
				if (o==0&&mi[o] == -1)
				{
					mi[o] = s;
					if (paths.find(PA(x, mi[o])) == paths.end()) {
						int *tmp = (int*)malloc(m * sizeof(int));
						int cnt = bfs(x, mi[o], false, tmp);
						//cout << cnt << endl;
						paths[PA(x, mi[o])] = PAP(tmp, cnt);
					}
				}
				int *kk = paths[PA(x, mi[o])].fst;
				int ks = paths[PA(x, mi[o])].scn;
				int nc = 0;
				for (int i = 1; i < ks; ++i) {
					int y = kk[ks - 1 - i];
					if (et[y] <= stas[y].de) {
						anst[an++] = y;
						if (et[y] == 0) ++nc;
						++et[y];
					}
					else {
						an = ant;
						for (int j = 1; j < i; ++j) --et[kk[ks - 1 - j]];
						return;
					}
				}
				dfs(mi[o], l + nc, s);
				an = ant;
				for (int j = 1; j < ks; ++j) --et[kk[ks - 1 - j]];
			}
		}
	}

	void dfs2(int x, int f) {
		tg[x] = 1;
		for (int p = stas[x].g; p; p = e[p].nxt) {
			int y = e[p].a;
			if (y != f) {
				if (stas[y].de < 3&&!tg[y]) dfs2(y, x);
			}
		}
	}

	public:

		subway_machine() {}

		subway_machine(string file) {
			strcpy_s(path, file.data());
		}

		bool initialize(string file=string("")) {
			if (file!="") strcpy_s(path, file.data());
			return init()==1;
		}

		void do_main(string s) {
			int k = getLID(s.data());
			if (k) {
				for (unsigned int i = 0; i < line_g[k].size(); ++i) printf_s("%s\n", stas[line_g[k][i]].s);
			}
			else fprintf_s(stderr, "line %s not found.\n", s.c_str());
		}

		void do_b(string t1, string t2) {
			if (Sta.find(t1) != Sta.end() && Sta.find(t2) != Sta.end()) {
				int k1 = Sta[t1];
				int k2 = Sta[t2];
				//cout << k1 << "," << k2 << endl;
				bfs(k1, k2);
			}
			else fprintf_s(stderr, "station not found.\n");
		}

		void do_c(string t1, string t2) {
			if (Sta.find(t1) != Sta.end() && Sta.find(t2) != Sta.end()) {
				int k1 = Sta[t1];
				int k2 = Sta[t2];
				//cout << k1 << "," << k2 << endl;
				bfs2(k1, k2);
			}
			else fprintf_s(stderr, "station not found.\n");
		}

		void do_a(string t1) {
			if (Sta.find(t1) != Sta.end()) {
				int k1 = Sta[t1];
				//cout << k1 << endl;
				memset(et, 0, sizeof(et));
				memset(tg, 0, sizeof(tg));
				maxtime = 0;
				for (int i = 1; i <= m; ++i) {
					if (stas[i].de == 1&&!tg[i]) {
						dfs2(i,0);
					}
				}
				an = 1;
				ans = 1001;
				anst[0] = k1;
				paths.clear();
				dfs(k1, 0, k1);
				cout << ans << endl;
				for (int i = 0; i < ans; ++i) {
					printf("%s\n", stas[ansq[i]].s);
				}
			}
			else fprintf_s(stderr, "station not found.\n");
		}

		void usage() {
			printf_s(
				"usage: \n\
  <empty> wait for input and show the stations on that line\n\
  -b station1 station2 \t find the way from sta.1 to sta.2 \n \t\t\t via the least stations\n\
  -c station1 station2 \t find the way from sta.1 to sta.2 \n \t\t\t which transfers the least and via the least stations\n\
  -a station \t\t go for a cycle trip start at this station\n\
");
		}

};

subway_machine sm(subway_file);
int main(int argc, char *argv[])
{
	string cmd_exit("exit");
	if (sm.initialize()) return 1;
	int flag = 1;
	if (argc == 1) {
		flag = 0;
		string s;
		while (1) {
			cin >> s;
			if (s == cmd_exit) break;
			sm.do_main(s);
		}
	}
	if (argc == 4 && !strcmp(argv[1], "-b")) {
		flag = 0;
		//puts("-b function");
		string t1(argv[2]);
		string t2(argv[3]);
		sm.do_b(t1, t2);
	}
	if (argc == 4 && !strcmp(argv[1], "-c")) {
		flag = 0;
		//puts("-c function");
		string t1(argv[2]);
		string t2(argv[3]);
		sm.do_c(t1, t2);
	}
	if (argc == 3 && !strcmp(argv[1], "-a")) {
		flag = 0;
		//puts("-a function");
		string t1(argv[2]);
		sm.do_a(t1);
	}
	if (flag) sm.usage();
	return 0;
}
