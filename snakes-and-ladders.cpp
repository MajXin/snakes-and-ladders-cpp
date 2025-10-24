#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <algorithm>
using namespace std;

void clearScreen() { system("cls"); }

int randInt(int a,int b){
    static mt19937 g((unsigned)time(nullptr));
    uniform_int_distribution<int>d(a,b);
    return d(g);
}

vector<int> buildZigZagNumbers(int rows,int cols){
    int n=rows*cols;
    vector<int> grid(rows*cols);
    int cur=n;
    for(int r=0;r<rows;r++){
        vector<int> row;
        for(int c=0;c<cols;c++) row.push_back(cur--);
        if(r%2==1) reverse(row.begin(),row.end());
        for(int c=0;c<cols;c++) grid[r*cols+c]=row[c];
    }
    return grid;
}

void placeSnakesLadders(int rows,int cols,unordered_map<int,int>&sn,unordered_map<int,int>&lad,int snCount,int ladCount){
    sn.clear(); lad.clear();
    int n=rows*cols;
    unordered_set<int> usedStarts,usedEnds;
    for(int i=0;i<snCount;i++){
        for(int t=0;t<200;t++){
            int head=randInt(2,n-1);
            int tail=randInt(1,head-1);
            if(usedStarts.count(head)||usedEnds.count(tail)) continue;
            if(usedStarts.count(tail)||usedEnds.count(head)) continue;
            sn[head]=tail; usedStarts.insert(head); usedEnds.insert(tail); break;
        }
    }
    for(int i=0;i<ladCount;i++){
        for(int t=0;t<200;t++){
            int bottom=randInt(1,n-2);
            int top=randInt(bottom+1,n);
            if(usedStarts.count(bottom)||usedEnds.count(top)) continue;
            if(usedStarts.count(top)||usedEnds.count(bottom)) continue;
            lad[bottom]=top; usedStarts.insert(bottom); usedEnds.insert(top); break;
        }
    }
}

void printGrid(int rows,int cols,const vector<int>&grid,const vector<pair<string,int>>&players,const unordered_map<int,int>&sn,const unordered_map<int,int>&lad){
    int n=rows*cols;
    int cellW = max(6, (int)to_string(n).size()+4);
    string hsep;
    for(int c=0;c<cols;c++){ hsep += "+"; hsep += string(cellW,'-'); } hsep += "+\n";
    unordered_map<int,string> tokenMap;
    for(const auto &p: players){
        if(p.second>0){
            if(tokenMap.count(p.second)) tokenMap[p.second] += p.first[0];
            else tokenMap[p.second] = string(1,p.first[0]);
        }
    }
    for(int r=0;r<rows;r++){
        cout << hsep;
        for(int line=0; line<3; ++line){
            for(int c=0;c<cols;c++){
                cout << "|";
                int idx = r*cols + c;
                int cell = grid[idx];
                if(line==0){
                    string s = to_string(cell);
                    cout << setw(cellW-1) << right << s;
                } else if(line==1){
                    string mid="";
                    if(tokenMap.count(cell)) mid = "P:" + tokenMap[cell];
                    cout << setw(cellW-1) << left << mid;
                } else {
                    string m="";
                    if(sn.count(cell)) m = "S->" + to_string(sn.at(cell));
                    else if(lad.count(cell)) m = "L->" + to_string(lad.at(cell));
                    cout << setw(cellW-1) << left << m;
                }
                cout << " ";
            }
            cout << "|\n";
        }
    }
    cout << hsep;
    cout << "\nLegend: P = players (initials)  S = snake head -> tail  L = ladder bottom -> top\n\n";
    cout << "Players:\n";
    for(const auto &p: players) cout << " " << p.first << " at " << p.second << "\n";
    cout << "\n";
}

int main(){
    srand((unsigned)time(nullptr));
    int rows=10, cols=10;
    int snCount=6, ladCount=6;
    cout << "Enter rows and cols (press Enter for 10 10): ";
    string line; getline(cin,line);
    if(!line.empty()){
        stringstream ss(line); int r,c; if(ss>>r>>c){ if(r>=2&&r<=20) rows=r; if(c>=2&&c<=20) cols=c; }
    }
    cout << "Enter number of snakes and ladders (press Enter for defaults 6 6): ";
    getline(cin,line);
    if(!line.empty()){ stringstream ss(line); int a,b; if(ss>>a>>b){ if(a>=0) snCount=a; if(b>=0) ladCount=b; } }
    vector<int> grid = buildZigZagNumbers(rows,cols);
    unordered_map<int,int> snakes, ladders;
    placeSnakesLadders(rows,cols,snakes,ladders,snCount,ladCount);
    pair<string,int> p1={"You",0}, p2={"Bot",0};
    vector<pair<string,int>> players={p1,p2};
    int turn=0;
    bool finished=false;
    while(!finished){
        clearScreen();
        printGrid(rows,cols,grid,players,snakes,ladders);
        cout << (players[turn].first) << "'s turn. Press Enter to roll.";
        if(players[turn].first=="Bot"){ Sleep(700); cout << "\nBot rolling...\n"; Sleep(700); }
        else getline(cin,line);
        int roll = randInt(1,6);
        cout << "Rolled: " << roll << "\n";
        if(players[turn].second==0){
            if(roll==6){ players[turn].second = 1; cout << players[turn].first << " unlocked and moved to 1\n"; }
            else cout << players[turn].first << " remains locked (need 6)\n";
        } else {
            int target = players[turn].second + roll;
            int n = rows*cols;
            if(target>n) cout << "Roll exceeds finish, no move\n";
            else { players[turn].second = target; cout << players[turn].first << " moved to " << players[turn].second << "\n";
                   if(snakes.count(players[turn].second)){ players[turn].second = snakes[players[turn].second]; cout << "Hit snake! Moved to " << players[turn].second << "\n"; }
                   else if(ladders.count(players[turn].second)){ players[turn].second = ladders[players[turn].second]; cout << "Climbed ladder! Moved to " << players[turn].second << "\n"; }
                 }
            if(players[turn].second==rows*cols){ cout << players[turn].first << " wins!\n"; finished=true; }
        }
        if(roll!=6) turn = 1-turn;
        if(!finished){ cout << "Press Enter to continue..."; getline(cin,line); }
    }
    return 0;
}
