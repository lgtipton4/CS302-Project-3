#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <iomanip>

#include "disjoint.h"

using namespace std;

class Superball {
  public:
    Superball(int argc, char **argv);
    int r;
    int c;
    int mss;
    int empty;
    vector <int> board;
    vector <int> goals;
    vector <int> colors;
    DisjointSetByRankWPC* dset;
};

void usage(const char *s) 
{
  fprintf(stderr, "usage: sb-analyze rows cols min-score-size colors\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

Superball::Superball(int argc, char **argv)
{
  int i, j;
  string s;

  if (argc != 5) usage(NULL);

  if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
  if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

  colors.resize(256, 0);

  for (i = 0; i < (int) strlen(argv[4]); i++) {
    if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
    if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
    if (colors[argv[4][i]] != 0) usage("Duplicate color");
    colors[argv[4][i]] = 2+i;
    colors[toupper(argv[4][i])] = 2+i;
  }

  board.resize(r*c);
  goals.resize(r*c, 0);

  empty = 0;

  for (i = 0; i < r; i++) {
    if (!(cin >> s)) {
      fprintf(stderr, "Bad board: not enough rows on standard input\n");
      exit(1);
    }
    if ((int) s.size() != c) {
      fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
      exit(1);
    }
    for (j = 0; j < c; j++) {
      if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
        fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
        exit(1);
      }
      board[i*c+j] = s[j];
      if (board[i*c+j] == '.') empty++;
      if (board[i*c+j] == '*') empty++;
      if (isupper(board[i*c+j]) || board[i*c+j] == '*') {
        goals[i*c+j] = 1;
        board[i*c+j] = tolower(board[i*c+j]);
      }
    }
  }
}

int main(int argc, char **argv)
{
  Superball *s;
 
  s = new Superball(argc, argv);

  DisjointSetByRankWPC ds(s->r*s->c);

  for(int i = 0; i < s->r; i++) {
    for(int j = 0; j < s->c; j++) {
      // element (i, j) = board(i*c + j)
      // if same color union (need to perform find on each set to unioh the heads)
      if(s->board[i*s->c + j] == s->board[i*s->c + j + 1] && j != s->c - 1) { // value to the right of the cell
        ds.Union(ds.Find(i*s->c + j), ds.Find(i*s->c + j + 1)); // accessing element instead of set
      }
      if(s->board[i*s->c + j] == s->board[(i+1) * s->c + j] && i!= s->r-1) { // value under the cell
        ds.Union(ds.Find(i * s->c + j), ds.Find((i+1) * s->c + j));
      }
      // after this run through each one and see if there is a goal cell, if there is we can say we can score it.
    }
  }
  // Find size via looping through again
  map<int, int> count;
  for(int i = 0; i < s->r * s->c; i++) {
    int cell = ds.Find(i); // do i need board here
    //cout << cell << endl;
    if(count.find(cell) == count.end()) {
      count[cell] = 0;
    }
    count[cell] += 1;
    //cout << "in count" << endl;
  }
  //cout << count.size();
  set<int> outputted;
  cout << "Scoring sets:" << endl;
  for(int i = 0; i < s->r; i++) {
    for(int j = 0; j < s->c; j++) {
      if((s->goals[i*s->c + j] == 1) && (s->board[i*s->c + j] != '*') && (count[ds.Find(i * s->c + j)] >= s->mss) && (outputted.find(ds.Find(i*s->c + j)) == outputted.end())) { // this never evaluates to true
        outputted.insert(ds.Find(i * s->c + j));
        cout << "  Size: " << setw(2) << count[ds.Find(i * s->c + j)] << "  Char: " << (char)(s->board[i * s->c + j]) << "  Scoring Cell: " << i << "," << j << endl;
      }
    }
  }
  //ds.Print();
}
