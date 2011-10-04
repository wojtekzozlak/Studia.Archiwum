//  Wojciech Żółtak : JiNPI - "Warm-Up Exercise" : 31.10.2010
#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<algorithm>
#include<cctype>
using namespace std;

typedef pair<string, string> StringPair;
typedef pair<int, StringPair> PeopleRecord;
typedef vector<PeopleRecord> PeopleVector;

bool not_space(char c)
/* Returns if char [c] is NOT a whitespace character. */
{
  return !isspace(c);
}


bool is_space(char c)
/* Returns if char [c] is a whitespace character. */
{
  return isspace(c);
}

vector<string> split(const string &s)
/* Splits string [s] into vector of strings using whitespaces as a delimiter. */
{
  typedef string::const_iterator iter;
  vector<string> ret;
  iter i, j;
  i = s.begin();
  while(i != s.end())
  {
    i = find_if(i, s.end(), not_space);
    j = find_if(i, s.end(), is_space);
    string word(i, j);
    if(word != "") ret.push_back(word);
    i = j;
  }
  return ret;
}

string merge(int a, int b, const vector<string> &v, const string &glue)
/* Merges vector of strings [v] into string using [glue]. */
{
  string ret;
  for(int i = a; i <= b; i++)
  {
    if(i > a) ret.append(glue);
    ret.append(v[i]);
  }
  return ret;
}

bool is_good(const string &s)
/* Checks if [string] is made of letters. */
{
  const char *c_str = s.c_str();
  for(int i = 0; i < s.size(); i++)
  {
    if(!(('a' <= c_str[i] && c_str[i] <= 'z') ||
         ('A' <= c_str[i] && c_str[i] <= 'Z')))
      return false;
  }
  return true;
}

bool parse(const string &line, PeopleVector &people, int &r1, int &r2)
/* Checks if [line] is a proper input data.
   If so - it adds parsed data into [people] vector. */
{
  vector<string> elts = split(line);
  int year;
  string names, surname;

  if(elts.size() < 2) return false;
  // maybe it's a reverse pattern?
  if(elts.size() == 2 && r1 == 0) 
  {
    int b1, b2;
    string buff(elts[0]);
    stringstream intParser(buff.append(" ").append(elts[1]));
    intParser >> b1 >> b2;
    if(!intParser.fail() && b1 != 0 && b2 != 0 && b1 < b2)
    {
      r1 = b1;
      r2 = b2;
      return true;
    }
  }
  for(int i = 0; i < elts.size() - 1; i++) if(!is_good(elts[i])) return false;
  stringstream intParser(elts[elts.size()-1]);
  intParser >> year;
  if(intParser.fail()) return false;

  names = merge(0, elts.size()-3, elts, " ");
  surname = elts[elts.size()-2];
  people.push_back(PeopleRecord(year, StringPair(surname, names)));

  return true;
}

void print(PeopleRecord person)
/* Prints person data into standard output. */
{
  cout << person.first << " ";
  cout << person.second.first << " " << person.second.second << endl;
}

void error(const string& line, int lineN)
/* Prints error message about [line, lineN] into standard output. */
{
  cerr << "error in line " << lineN << ": " << line << endl;
}

int main(int argc, char* argv[])
{
  string line;
  PeopleVector people;

  int r1, r2 = 0;
  int lineN = 1;
  while(getline(cin, line))
  {
    if(!parse(line, people, r1, r2)) error(line, lineN);
    lineN++;
  }

  sort(people.begin(), people.end());

  if(r1 != 0 && r2 != 0)
  {
    PeopleVector::iterator beg, end;
    beg = lower_bound(people.begin(), people.end(), PeopleRecord(r1, StringPair("", "")));
    end = upper_bound(people.begin(), people.end(), PeopleRecord(r2+1, StringPair("", "")));
    reverse(beg, end);
  }

  for_each(people.begin(), people.end(), print);
  
  return 0;
}
